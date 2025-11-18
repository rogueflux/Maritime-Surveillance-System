#ifndef TENSORFLOW_LITE_MICRO_MICRO_INTERPRETER_H_
#define TENSORFLOW_LITE_MICRO_MICRO_INTERPRETER_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  kTfLiteOk = 0,
  kTfLiteError = 1
} TfLiteStatus;

typedef struct TfLiteContext TfLiteContext;
typedef struct TfLiteTensor TfLiteTensor;
typedef struct TfLiteModel TfLiteModel;
typedef struct TfLiteInterpreter TfLiteInterpreter;
typedef struct TfLiteInterpreterOptions TfLiteInterpreterOptions;

// Function prototypes
TfLiteInterpreter* TfLiteInterpreterCreate(const TfLiteModel* model, const TfLiteInterpreterOptions* options);
TfLiteStatus TfLiteInterpreterInvoke(TfLiteInterpreter* interpreter);
TfLiteTensor* TfLiteInterpreterGetInputTensor(const TfLiteInterpreter* interpreter, int32_t input_index);
const TfLiteTensor* TfLiteInterpreterGetOutputTensor(const TfLiteInterpreter* interpreter, int32_t output_index);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // TENSORFLOW_LITE_MICRO_MICRO_INTERPRETER_H_
