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





        if (argc != 2){
            printf ("input : [exe] [log file]\n");
            exit(0); 
        }

        unsigned int conj_bit = 16;
        unsigned int wordlist_size = 128;

	const char *graph_path = argv[1];
        const char *wordfile = argv[2];
        const char *logfile = argv[3];
        size_t size;
        std::vector<float> bits_val;

        std::vector<std::string> word_arr;

        printf ("Read word file...\n");
        std::ifstream t1(wordfile);
        if (!t1) {
                printf ("There's no word file\n");
                exit(0);
        }
        t1.seekg(0, std::ios::end);
        size = t1.tellg();
        std::string wordstr(size, ' ');
        t1.seekg(0);
        t1.read(&wordstr[0], size); 

        printf ("Read log file...\n");
        std::ifstream t2(logfile);
        if (!t2) {
                printf ("There's no log file\n");
                exit(0);
        }
        t2.seekg(0, std::ios::end);
        size = t2.tellg();
        std::string logstr(size, ' ');
        t2.seekg(0);
        t2.read(&logstr[0], size); 

        printf ("Make Wordlist...\n");
        unsigned int len = wordstr.length();
        unsigned int bef_place = 0;
        for (unsigned int i = 0;i < len;i++) {
                if (wordstr[i] == '\n') { 
                        wordstr[i] = 0; 
                        word_arr.push_back(&wordstr[bef_place]);
                        bef_place = i+1;
                }
        }

        if (word_arr.size() != wordlist_size) {
                printf ("Word Count is not 128\n");
                exit(0);
        }


        for (unsigned int i = 0;i < logstr.length();i++) {
                if (logstr[i] == '\n') { logstr[i] = ' '; }
                else if ((logstr[i] >= 'A') && (logstr[i] <= 'Z')) {
                        logstr[i] += ('a' - 'A');
                }
        }



        for (int i = 0;i < wordlist_size;i++) {

                if ((i%conj_bit == 0) && (i != 0)) {
                    bits_val.push_back(bit_coll);
                    bit_coll = 0.0f;
                }

                if (logstr.find(word_arr[i]) != std::string::npos) { 
                    bit_coll = (bit_coll * 2.0f) + 1.0f;
                }
                else { 
                    bit_coll = (bit_coll * 2.0f);
                }

        }







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
/*
	std::printf("Type eight float numbers : ");
	std::scanf("%f %f %f %f %f %f %f %f", &x, &y, &z, &x2, &y2, &z2, &x3, &y3);
	interpreter->typed_input_tensor<float>(0)[0] = x;
	interpreter->typed_input_tensor<float>(0)[1] = y;
	interpreter->typed_input_tensor<float>(0)[2] = z;
	interpreter->typed_input_tensor<float>(0)[3] = x2;
	interpreter->typed_input_tensor<float>(0)[4] = y2;
	interpreter->typed_input_tensor<float>(0)[5] = z2;
	interpreter->typed_input_tensor<float>(0)[6] = x3;
	interpreter->typed_input_tensor<float>(0)[7] = y3;
*/
        for (int i = 0;i < 8;i++) {
                interpreter->typed_input_tensor<float>(0)[i] = bits_val[i];
        }

	if(interpreter->Invoke() != kTfLiteOk){
		std::printf("Failed to invoke!\n");
		exit(0);
	}
	float* output = interpreter->typed_output_tensor<float>(0);
	printf("output = %f\n", output[0]);
	return 0;
}
