#include <stdio.h>
#include <string>
#include <vector>
#include "tensorflow/contrib/lite/kernels/register.h"
#include "tensorflow/contrib/lite/model.h"
#include "tensorflow/contrib/lite/string_util.h"



int main(){
	const char graph_path[20] = "Plus_Model.tflite\0";
	const int num_threads = 1;
	std::string input_layer_type = "float";
	std::vector<int> sizes = {8};
	float x,y,z,x2,y2,z2,x3,y3;

	std::unique_ptr<tflite::FlatBufferModel> model(
		tflite::FlatBufferModel::BuildFromFile(graph_path));

	if(!model){
		printf("Failed to mmap model\n");
		exit(0);
	}

	tflite::ops::builtin::BuiltinOpResolver resolver;
	std::unique_ptr<tflite::Interpreter> interpreter;
	tflite::InterpreterBuilder(*model, resolver)(&interpreter);

	if(!interpreter){
		printf("Failed to construct interpreter\n");
		exit(0);
	}

	if(num_threads != 1){
		interpreter->SetNumThreads(num_threads);
	}

	interpreter->ResizeInputTensor(0, sizes);

	float* input = interpreter->typed_input_tensor<float>(0);

	if(interpreter->AllocateTensors() != kTfLiteOk){
		printf("Failed to allocate tensors\n");
		exit(0);
	}

	//read two numbers
	std::printf("Type eight float numbers : ");
	std::scanf("%f %f %f", &x, &y, &z, &x2, &y2, &z2, &x3, &y3);
	interpreter->typed_input_tensor<float>(0)[0] = x;
	interpreter->typed_input_tensor<float>(0)[1] = y;
	interpreter->typed_input_tensor<float>(0)[2] = z;
	interpreter->typed_input_tensor<float>(0)[3] = x2;
	interpreter->typed_input_tensor<float>(0)[4] = y2;
	interpreter->typed_input_tensor<float>(0)[5] = z2;
	interpreter->typed_input_tensor<float>(0)[6] = x3;
	interpreter->typed_input_tensor<float>(0)[7] = y3;

	if(interpreter->Invoke() != kTfLiteOk){
		std::printf("Failed to invoke!\n");
		exit(0);
	}
	float* output = interpreter->typed_output_tensor<float>(0);
	printf("output = %f\n", output[0]);
	return 0;
}
