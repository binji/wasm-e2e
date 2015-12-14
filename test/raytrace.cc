#include "test/wasm.h"

#define WIDTH 256
#define HEIGHT 256

typedef unsigned char byte;

unsigned int buffer[WIDTH * HEIGHT];

const byte permute[256] = {104, 69, 63, 141, 115, 14, 55, 219, 91, 103, 143, 149, 53, 105, 116, 196, 107, 11, 226, 239, 188, 108, 100, 224, 253, 140, 158, 134, 38, 126, 216, 148, 137, 43, 200, 164, 194, 242, 251, 18, 28, 101, 106, 217, 57, 183, 179, 87, 198, 119, 175, 174, 202, 77, 62, 146, 117, 98, 150, 132, 182, 176, 78, 236, 59, 113, 17, 247, 191, 227, 49, 230, 167, 144, 252, 3, 228, 170, 215, 204, 45, 32, 21, 131, 68, 72, 128, 114, 250, 1, 88, 235, 58, 95, 0, 65, 205, 233, 111, 96, 110, 172, 40, 173, 186, 48, 220, 121, 83, 35, 244, 36, 34, 130, 127, 125, 142, 246, 147, 76, 39, 187, 189, 90, 237, 52, 24, 243, 192, 207, 255, 163, 248, 7, 5, 156, 211, 15, 160, 6, 109, 37, 92, 249, 94, 66, 60, 231, 177, 23, 152, 93, 12, 221, 197, 245, 112, 229, 254, 122, 74, 171, 44, 70, 165, 64, 8, 82, 133, 33, 184, 26, 206, 213, 4, 10, 73, 166, 209, 159, 138, 201, 169, 124, 151, 154, 71, 190, 56, 89, 139, 193, 27, 212, 234, 185, 25, 20, 155, 145, 54, 51, 208, 29, 210, 223, 61, 67, 31, 13, 99, 225, 199, 81, 85, 118, 2, 181, 97, 75, 180, 9, 86, 195, 120, 232, 161, 30, 129, 222, 47, 123, 218, 214, 80, 102, 16, 84, 240, 41, 136, 22, 238, 168, 157, 241, 203, 79, 42, 178, 153, 19, 50, 135, 46, 162};

static unsigned int f2b(float value) {
  if (value < 0.0f) {
    value = 0.0f;
  }
  if(value > 1.0f) {
    value = 1.0f;
  }
  return (int)(value * 255);
}

// Convert a linear color value to a gamma-space byte.
// Square root approximates gamma-correct rendering.
static unsigned int l2g(float value) {
  return f2b(sqrtF32(value));
}

static unsigned int packColor(float r, float g, float b, float a) {
  return f2b(a) << 24 | l2g(b) << 16 | l2g(g) << 8 | l2g(r);
}

class Vec3 {
 public:
  Vec3(): x(0.0f), y(0.0f), z(0.0f) {}
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  float x;
  float y;
  float z;

  void add(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
  }

  void scaledAdd(const Vec3& other, float scale) {
    x += other.x * scale;
    y += other.y * scale;
    z += other.z * scale;
  }

  void scaledAdd(const Vec3& other, const Vec3& scale) {
    x += other.x * scale.x;
    y += other.y * scale.y;
    z += other.z * scale.z;
  }

  void sub(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
  }

  void scale(float other) {
    x *= other;
    y *= other;
    z *= other;
  }

  void scale(const Vec3& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
  }

  float dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  float nlDot(const Vec3& other) const {
    float value = dot(other);
    if (value < 0.0f) {
      value = 0.0f;
    }
    return value;
  }

  float length() const {
    return sqrtF32(x * x + y * y + z * z);
  }

  void normalize() {
    scale(1.0f / length());
  }

  void blend(const Vec3& other, float amt, Vec3* out) {
    float keep = 1.0f - amt;
    *out = Vec3(x * keep + other.x * amt, y * keep + other.y * amt, z * keep + other.z * amt);
  }

};

// TODO return structures.
static void sampleEnv(const Vec3& dir, Vec3* out) {
  float amt = dir.y * 0.5f + 0.5f;
  Vec3(0.1f, 1.0f, 0.1f).blend(Vec3(0.1f, 0.1f, 1.0f), amt, out);
}

struct Sphere {
  Vec3 center;
  float radius;
};

class Intersection {
 public:
  Vec3 pos;
  Vec3 normal;
  float distance;
};

Intersection intersection;

static int intersectSphere(const Vec3& pos, const Vec3& dir, const Sphere& sphere, Intersection* intersection) {
  Vec3 offset(pos);
  offset.sub(sphere.center);

  float dot = dir.dot(offset);
  float partial = dot * dot + sphere.radius * sphere.radius - offset.dot(offset);
  if (partial >= 0.0f) {
    float d = -dot - sqrtF32(partial);
    if (d > 0.0f) {
      if (intersection) {
	// Intersection positon.
	Vec3 p(pos);
	p.scaledAdd(dir, d);

	// Intersection normal.
	Vec3 n(p);
	n.sub(sphere.center);
	n.normalize();

	intersection->pos = p;
	intersection->normal = n;
	intersection->distance = d;
      }
      return 1;
    }
  }
  return 0;
}

Vec3 light;
Vec3 normal;
Vec3 pos;
Vec3 eye;
Vec3 env;

class Material {
public:
  Vec3 diffuseColor;
};

const struct Material material[] = {
  {
    Vec3(0.7f, 0.7f, 0.7f)
  },
  {
    Vec3(0.9f, 0.1f, 0.1f)
  },
  {
    Vec3(0.9f, 0.1f, 0.9f)
  }
};

const int num_objects = 3;

struct Sphere scene[num_objects] = {
  {Vec3(-2.0f, -1.0f, -6.0f), 3.0f},
  {Vec3(2.0f, -1.0f, -6.0f), 3.0f},
  {Vec3(0.0f, 2.0f, -6.0f), 3.0f},
};

static void shade(const Vec3& eye, const Vec3& light, const float lightVisibility, const Vec3& normal, const Material& material, Vec3* color) {
  // Surface diffuse.
  float ambientScale = 0.2f;

  // Ambient color
  sampleEnv(normal, &env);
  env.scale(ambientScale);
  color->scaledAdd(material.diffuseColor, env);

  if (lightVisibility <= 0.0f) {
    return;
  }

  // Diffuse color
  float diffuse = normal.nlDot(light);
  color->scaledAdd(material.diffuseColor, diffuse * lightVisibility);

  // Specular color

  // Compute the half vector;
  Vec3 half(eye);
  half.scale(-1.0f);
  half.add(light);
  half.normalize();

  float specular = normal.nlDot(half);
  // Take it to the 64th power, manually.
  specular = specular * specular;
  specular = specular * specular;
  specular = specular * specular;
  specular = specular * specular;
  specular = specular * specular;
  specular = specular * specular;

  specular = specular * 0.6f;

  color->scaledAdd(Vec3(1.0f, 1.0f, 1.0f), specular * lightVisibility);
}

Vec3 color;

static void cast(const Vec3& pos, const Vec3& eye, const Vec3& light, Vec3* color) {
  int closest = -1;
  Intersection closestIntersection;
  closestIntersection.distance = 1e9;

  for (int o = 0; o < num_objects; o++) {
    float distance;
    if (intersectSphere(pos, eye, scene[o], &intersection)) {
      if (intersection.distance < closestIntersection.distance) {
	closest = o;
	closestIntersection = intersection;
      }
    }
  }

  // Light accumulation
  *color = Vec3(0.0f, 0.0f, 0.0f);
  if (closest != -1) {

    // Shadow ray.
    float lightVisibility = 1.0f;
    Vec3 towardsLight(light);
    //towardsLight.scale(-1.0f);
    for (int o = 0; o < num_objects; o++) {
      if (intersectSphere(closestIntersection.pos, towardsLight, scene[o], 0)) {
	lightVisibility = 0.0f;
	break;
      }
    }

    shade(eye, light, lightVisibility, closestIntersection.normal, material[closest], color);
  } else {
    sampleEnv(eye, &env);
    color->add(env);
  }
}

static byte rand2(int a, int b) {
  return permute[(permute[a & 255] + b) & 255];
}

static byte rand3(int a, int b, int c) {
  return permute[(rand2(a, b) + c) & 255];
}

static void emitImage(unsigned int* p, int width, int height) {
  //light = Vec3(20.0f, 20.0f, 15.0f);
  light = Vec3(10.0f, 20.0f, 5.0f);
  light.normalize();

  for (int j = 0; j < height; j++) {
    const float y = 0.5f - j / (float)height;
    for (int i = 0; i < width; i++) {
      const float x = i / (float)width - 0.5f;

      Vec3 accumulate(0.0f, 0.0f, 0.0f);

      const int num_samples = 16;

      for (int s = 0; s < num_samples; s++) {
	// The position from which the ray is being emitted.
	// (A 1 x 1 square centered around the z axis.)
	float xJitter = (signed char)rand3(s, i, j) / 128.0f / (float) width * 0.5f;
	float yJitter = (signed char)rand3(s + 128, i, j) / 128.0f / (float) height * 0.5f;
	pos = Vec3(x + xJitter, y + yJitter, 0.0f);
	// The direction in which the ray is being emitted, assuming the focal
	// point is 0.5 behind the image plane.
	eye = pos;
	eye.z -= 0.5f;
	eye.normalize();

	cast(pos, eye, light, &color);

	accumulate.add(color);
      }

      accumulate.scale(1.0f / num_samples);

      unsigned int pixel = packColor(accumulate.x, accumulate.y, accumulate.z, 255);
      *p++ = pixel;
    }
  }
}

int main() {
  emitImage(buffer, WIDTH, HEIGHT);
  flipBuffer((void*)buffer, WIDTH, HEIGHT);
  return 0;
}
