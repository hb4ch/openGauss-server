#include <stddef.h>
typedef void* ONNXEnvHandle;
typedef void* ONNXModelHandle;
ONNXEnvHandle ONNXEnvCreate(void) { return NULL; }
void ONNXEnvRelease(ONNXEnvHandle h) { (void)h; }
ONNXModelHandle ONNXLoadModel(ONNXEnvHandle e, const char* m, const char* t, int* d)
    { (void)e;(void)m;(void)t;if(d)*d=768;return NULL; }
void ONNXUnloadModel(ONNXModelHandle h) { (void)h; }
int ONNXEmbeddingInfer(ONNXModelHandle h, const char* t, float* e, int d)
    { (void)h;(void)t;(void)e;(void)d;return 0; }
int ONNXEmbeddingInferBatch(ONNXModelHandle h, char** t, int n, float** e, int d)
    { (void)h;(void)t;(void)n;(void)e;(void)d;return 0; }
int ONNXGetEmbeddingDim(ONNXModelHandle h) { (void)h;return 768; }
