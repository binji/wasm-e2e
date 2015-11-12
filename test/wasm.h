#ifdef __cplusplus
extern "C" {
#endif

int puts(const char*);
void print(int);
float sqrtF32(float) asm("llvm.sqrt.f32");
void flipBuffer(const void*, int, int);

#ifdef __cplusplus
}
#endif
