#include "test/wasm.h"

#define WIDTH 256
#define HEIGHT 256

typedef unsigned char byte;

unsigned int buffer[WIDTH * HEIGHT];


static unsigned int f2b(float value) {
  // TODO unhack when min and max work correctly.
  if (value < 0.000001f) {
    value = 0.0f;
  }
  if(value > 0.999999f) {
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
    // TODO unhack when min and max work correctly.
    if (value < 0.000001f) {
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

class Intersection {
 public:
  Vec3 pos;
  Vec3 normal;
};

static int intersect(const Vec3& pos, const Vec3& dir, Vec3* normal) {
  // The sphere.
  const float radius = 4.0f;
  // TODO movement.
  Vec3 center(0.0f, 0.0f, -6.0f);

  Vec3 offset(pos);
  offset.sub(center);

  float dot = dir.dot(offset);
  float partial = dot * dot + radius * radius - offset.dot(offset);
  if (partial >= 0.0f) {
    float d = -dot - sqrtF32(partial);
    if (d >= 0.0f) {
      Vec3 n(pos);
      n.scaledAdd(dir, d);
      n.sub(center);
      n.normalize();
      *normal = n;
      return 1;
    }
  }
  return 0;
}

Vec3 light;
Vec3 normal;
Vec3 pos;
Vec3 dir;

static void emitImage(unsigned int* p, int width, int height) {
  light = Vec3(20.0f, 20.0f, 15.0f);
  light.normalize();

  for (int j = 0; j < height; j++) {
    const float y = 0.5f - j / (float)height;
    for (int i = 0; i < width; i++) {
      const float x = i / (float)width - 0.5f;

      pos = Vec3(x, y, 0.0f);
      dir = Vec3(x, y, -0.5f);
      dir.normalize();

      // Compute the half vector;
      Vec3 half(dir);
      half.scale(-1.0f);
      half.add(light);
      half.normalize();

      // Light accumulation
      Vec3 color(0.0f, 0.0f, 0.0f);

      // Surface diffuse.
      Vec3 diffuseColor(0.7f, 0.7f, 0.7f);

      Vec3 env;
      if (intersect(pos, dir, &normal)) {
        float ambientScale = 0.2f;
        sampleEnv(normal, &env);
	env.scale(ambientScale);
	color.scaledAdd(diffuseColor, env);

        float diffuse = normal.nlDot(light);
	color.scaledAdd(diffuseColor, diffuse);

        float specular = normal.nlDot(half);
        // Take it to the 64th power, manually.
        specular = specular * specular;
        specular = specular * specular;
        specular = specular * specular;
        specular = specular * specular;
        specular = specular * specular;
        specular = specular * specular;

        specular = specular * 0.6f;

        color.scaledAdd(Vec3(1.0f, 1.0f, 1.0f), specular);
      } else {
        sampleEnv(dir, &env);
        color.add(env);
      }
      unsigned int pixel = packColor(color.x, color.y, color.z, 255);
      *p++ = pixel;
    }
  }
}

int main() {
  emitImage(buffer, WIDTH, HEIGHT);
  flipBuffer((void*)buffer, WIDTH, HEIGHT);
  return 0;
}
