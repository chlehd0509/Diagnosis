#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "tensorflow/contrib/lite/kernels/register.h"
#include "tensorflow/contrib/lite/model.h"
#include "tensorflow/contrib/lite/string_util.h"


#define Case_Num 5



unsigned int wordlist_size = 53;




char *Case_Str[Case_Num] = 
{
        "Connect Fail",
        "DHCP Error",
        "IP Conflict",
        "Wrong Password",
        "Wifi-Success"
};



int main(int argc, char *argv[]){

	const int num_threads = 1;
	std::vector<int> sizes = {wordlist_size};



        if (argc != 4){
            printf ("input : [exe] [model file] [wordlist file] [log file]\n");
            exit(0); 
        }

	const char *graph_path = argv[1];
        const char *wordfile = argv[2];
        const char *logfile = argv[3];

        size_t size;
        std::vector<float> bits_val;

        std::vector<std::string> word_arr;

//        printf ("Read word file...\n");
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

//        printf ("Read log file...\n");
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

//        printf ("Make Wordlist...\n");
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

                size_t pos = logstr.find (wordstr[i], 0); 
                float bit_coll = 0.0f;
                while(pos != std::string::npos){ 
                        pos = logstr.find (wordstr[i], pos+1); 
                        bit_coll = bit_coll + 1.0f; 
                }

                bits_val.push_back(bit_coll);

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

        for (int i = 0;i < wordlist_size;i++) {
                interpreter->typed_input_tensor<float>(0)[i] = bits_val[i];
        }

	if(interpreter->Invoke() != kTfLiteOk){
		std::printf("Failed to invoke!\n");
		exit(0);
	}



	float* output = interpreter->typed_output_tensor<float>(0);

        output[3] = output[3] * 0.5;

        float Max_val = output[0];
        int Max_Index = 0;
        
        for (int i = 0;i < Case_Num;i++) {
                printf ("\n %f \n", output[i]);
                if (Max_val < output[i]) { Max_val = output[i]; Max_Index = i; }
        }

        printf ("Diagnosis Result : [%s]\n", Case_Str[Max_Index]);



	return 0;
}
