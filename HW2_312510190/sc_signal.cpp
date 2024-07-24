//sc_signal.cpp
#include <systemc.h>
#include <clkreset.h>
#include <Monitor.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sc_fifo.h>
#include <sc_buffer.h>

using namespace std;

SC_MODULE(alexnet1){
    sc_port<sc_signal_out_if<string> > str;
    string str1 = "dog.txt";
    void readtxt1(){
        //send the file name to the next module
        str->write(str1);
        //wait(1, SC_NS);
    }
    SC_CTOR(alexnet1){
        SC_METHOD(readtxt1);
    }
};

/*SC_MODULE(alexnet3){
    sc_port<sc_signal_out_if<string> > str;
    string str1 = "conv1_weight.txt";
    void readtxt2(){
        //send the file name to the next module
        str->write(str1);
        //wait(1, SC_NS);
    }
    SC_CTOR(alexnet3){
        SC_METHOD(readtxt2);
    }
};

SC_MODULE(alexnet4){
    sc_port<sc_fifo_out_if<string> > str;
    string str1 = "conv1_bias.txt";
    void readtxt3(){
        //send the file name to the next module
        str->write(str1);
        //wait(1, SC_NS);
    }
    SC_CTOR(alexnet4){
        SC_METHOD(readtxt3);
    }
};*/

SC_MODULE(alexnet2){
    
    float input[3][224][224];
    float weight_1st[3][11][11];
    float output_1st[64][27][27];
    float weight_2st[64][5][5];
    float output_2st[192][13][13];
    float weight_3st[192][3][3];
    float output_3st[384][13][13];
    float weight_4st[384][3][3];
    float output_4st[256][13][13];
    float weight_5st[256][3][3];
    float output_5st[256][6][6];
    float fc_6st[4096];
    float fc_7st[4096];
    float fc_8st[1000];
    sc_port<sc_signal_in_if<string> > str_1;
    sc_port<sc_signal_in_if<string> > str_2;
    sc_port<sc_fifo_in_if<string> > str_3;
    sc_out <float> output_final[1000];


void conv1(){
    //first conv layer
    int INPUT_CHANNELS = 3;
    int INPUT_HEIGHT = 224;
    int INPUT_WIDTH = 224;
    int OUTPUT_CHANNELS = 64;
    int OUTPUT_HEIGHT = 55;
    int OUTPUT_WIDTH = 55;
    int KERNEL_SIZE = 11;
    int ZERO_PADDING = 1;
    int STRIDE_SIZE = 4;
    int pool_size = 3;
    int pool_stride = 2;
    int pool_output_height = 27;
    int pool_output_width = 27;

    float output[OUTPUT_CHANNELS][OUTPUT_HEIGHT][OUTPUT_WIDTH];
    float bias[OUTPUT_CHANNELS];
    // Create convolution module
    //Convolution convolution("convolution");

    // Read the file name from the previous module
    string str1 = str_1->read();
    cout<<"Time is "<<sc_time_stamp()<<", sc_signal reads "<<str1<<endl;
    // Load input data from file
    std::ifstream input_file(str1);
    if (input_file.is_open()) {
        float temp;
        for (int ic = 0; ic < 3; ic++) {
            for (int ih = 0; ih < 224; ih++) {
                for (int iw = 0; iw < 224; iw++) {
                    input_file >> temp;
                    input[ic][ih][iw] = temp;
                }
            }
        }
        input_file.close();
    } else {
        std::cout << "Failed to open input.txt" << std::endl;
        return ;
    }


    // Read the file name from the alexnet3
    string str2 = str_2->read();
    cout<<"Time is "<<sc_time_stamp()<<", sc_buffer reads "<<str2<<endl;
    // Load weight data from file
    std::ifstream weight_file(str2);
    if (!weight_file.is_open()) {
        std::cout << "Failed to open weight.txt" << std::endl;
        return ;
    }

    // Read the file name from the alexnet4
    string str3 = str_3->read();
    cout<<"Time is "<<sc_time_stamp()<<", sc_fifo reads "<<str3<<endl;
    // Load bias data from file
    std::ifstream bias_file(str3);
    if (bias_file.is_open()) {
        float temp;
        for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
            bias_file >> temp;
            bias[oc] = temp;
        }
        bias_file.close();
    } else {
        std::cout << "Failed to open weight.txt" << std::endl;
        return ;
    }

    // Perform convolution
    for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH; ow++) {
                float sum = 0;

                for (int ic = 0; ic < INPUT_CHANNELS; ic++) {
                    for (int kh = 0; kh < KERNEL_SIZE; kh++) {
                        for (int kw = 0; kw < KERNEL_SIZE; kw++) {
                            if(oh == 0 && ow == 0){
                                weight_file >> weight_1st[ic][kh][kw];
                            }
                            int ih = oh * STRIDE_SIZE + kh - ZERO_PADDING;
                            int iw = ow * STRIDE_SIZE + kw - ZERO_PADDING;

                            if (ih >= 0 && ih < INPUT_HEIGHT && iw >= 0 && iw < INPUT_WIDTH) {
                                sum += input[ic][ih][iw] * weight_1st[ic][kh][kw];
                            }
                        }
                    }
                }

               output[oc][oh][ow] = sum;
            }
        }
    }
    weight_file.close();

    // Add bias
    for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH; ow++) {
                output[oc][oh][ow] += bias[oc];
            }
        }
    }

    // Apply ReLU
    for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH; ow++) {
                if (output[oc][oh][ow] < 0) {
                    output[oc][oh][ow] = 0;
                }
            }
        }
    }

    // Max pooling
    
    for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
        for (int oh = 0; oh < pool_output_height; oh++) {
            for (int ow = 0; ow < pool_output_width; ow++) {
                float max_value = output[oc][oh * pool_stride][ow * pool_stride];

                for (int kh = 0; kh < pool_size; kh++) {
                    for (int kw = 0; kw < pool_size; kw++) {
                        float value = output[oc][oh * pool_stride + kh][ow * pool_stride + kw];
                        if (value > max_value) {
                            max_value = value;
                        }
                    }
                }

                output_1st[oc][oh][ow] = max_value;
            }
        }
    }
    /*for (int oc = 0; oc < 64; oc++) {
        for (int oh = 0; oh < 27; oh++) {
            for (int ow = 0; ow < 27; ow++) {
                std::cout << "Output[" << oc << "][" << oh << "][" << ow << "] = " << output_1st[oc][oh][ow] << std::endl;
            }
        }
    }*/

    int INPUT_CHANNELS2 = 64;
    int INPUT_HEIGHT2 = 27;
    int INPUT_WIDTH2 = 27;
    int OUTPUT_CHANNELS2 = 192;
    int OUTPUT_HEIGHT2 = 27;
    int OUTPUT_WIDTH2 = 27;
    int KERNEL_SIZE2 = 5;
    int ZERO_PADDING2 = 2;
    int STRIDE_SIZE2 = 1;
    int pool_size2 = 3;
    int pool_stride2 = 2;
    int pool_output_height2 = 13;
    int pool_output_width2 = 13;

    float output2[OUTPUT_CHANNELS2][OUTPUT_HEIGHT2][OUTPUT_WIDTH2];
    float bias2[OUTPUT_CHANNELS2];
    // Create convolution module
    //Convolution convolution("convolution");


    // Load weight data from file
    std::ifstream weight_file2("conv2_weight.txt");
    if (!weight_file2.is_open()) {
        std::cout << "Failed to open conv2_weight.txt" << std::endl;
        return ;
    }

    // Load bias2 data from file
    std::ifstream bias_file2("conv2_bias.txt");
    if (bias_file2.is_open()) {
        float temp;
        for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
            bias_file2 >> temp;
            bias2[oc] = temp;
        }
        bias_file2.close();
    } else {
        std::cout << "Failed to open conv2_bias.txt" << std::endl;
        return ;
    }

    // Perform convolution
    for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT2; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH2; ow++) {
                float sum = 0;

                for (int ic = 0; ic < INPUT_CHANNELS2; ic++) {
                    for (int kh = 0; kh < KERNEL_SIZE2; kh++) {
                        for (int kw = 0; kw < KERNEL_SIZE2; kw++) {
                            if(oh == 0 && ow == 0){
                                weight_file2 >> weight_2st[ic][kh][kw];
                            }
                            int ih = oh * STRIDE_SIZE2 + kh - ZERO_PADDING2;
                            int iw = ow * STRIDE_SIZE2 + kw - ZERO_PADDING2;

                            if (ih >= 0 && ih < INPUT_HEIGHT2 && iw >= 0 && iw < INPUT_WIDTH2) {
                                sum += output_1st[ic][ih][iw] * weight_2st[ic][kh][kw];
                            }
                        }
                    }
                }

               output2[oc][oh][ow] = sum;
            }
        }
    }
    weight_file2.close();

    // Add bias2
    for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT2; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH2; ow++) {
                output2[oc][oh][ow] += bias2[oc];
            }
        }
    }

    // Apply ReLU
    for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT2; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH2; ow++) {
                if (output2[oc][oh][ow] < 0) {
                    output2[oc][oh][ow] = 0;
                }
            }
        }
    }

    // Max pooling
    
    for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
        for (int oh = 0; oh < pool_output_height2; oh++) {
            for (int ow = 0; ow < pool_output_width2; ow++) {
                float max_value = output2[oc][oh * pool_stride2][ow * pool_stride2];

                for (int kh = 0; kh < pool_size2; kh++) {
                    for (int kw = 0; kw < pool_size2; kw++) {
                        float value = output2[oc][oh * pool_stride2 + kh][ow * pool_stride2 + kw];
                        if (value > max_value) {
                            max_value = value;
                        }
                    }
                }

                output_2st[oc][oh][ow] = max_value;
            }
        }
    }

    /*for (int oc = 0; oc < 192; oc++) {
        for (int oh = 0; oh < 13; oh++) {
            for (int ow = 0; ow < 13; ow++) {
                std::cout << "output2[" << oc << "][" << oh << "][" << ow << "] = " << output_2st[oc][oh][ow] << std::endl;
            }
        }
    }*/

    int INPUT_CHANNELS3 = 192;
    int INPUT_HEIGHT3 = 13;
    int INPUT_WIDTH3 = 13;
    int OUTPUT_CHANNELS3 = 384;
    int OUTPUT_HEIGHT3 = 13;
    int OUTPUT_WIDTH3 = 13;
    int KERNEL_SIZE3 = 3;
    int ZERO_PADDING3 = 1;
    int STRIDE_SIZE3 = 1;
    

    float bias3[OUTPUT_CHANNELS3];
    // Create convolution module
    //Convolution convolution("convolution");


    // Load weight data from file
    std::ifstream weight_file3("conv3_weight.txt");
    if (!weight_file3.is_open()) {
        std::cout << "Failed to open conv3_weight.txt" << std::endl;
        return ;
    }

    // Load bias data from file
    std::ifstream bias_file3("conv3_bias.txt");
    if (bias_file3.is_open()) {
        float temp;
        for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
            bias_file3 >> temp;
            bias3[oc] = temp;
        }
        bias_file3.close();
    } else {
        std::cout << "Failed to open conv3_bias.txt" << std::endl;
        return ;
    }

    // Perform convolution
    for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT3; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH3; ow++) {
                float sum = 0;

                for (int ic = 0; ic < INPUT_CHANNELS3; ic++) {
                    for (int kh = 0; kh < KERNEL_SIZE3; kh++) {
                        for (int kw = 0; kw < KERNEL_SIZE3; kw++) {
                            if(oh == 0 && ow == 0){
                                weight_file3 >> weight_3st[ic][kh][kw];
                            }
                            int ih = oh * STRIDE_SIZE3 + kh - ZERO_PADDING3;
                            int iw = ow * STRIDE_SIZE3 + kw - ZERO_PADDING3;

                            if (ih >= 0 && ih < INPUT_HEIGHT3 && iw >= 0 && iw < INPUT_WIDTH3) {
                                sum += output_2st[ic][ih][iw] * weight_3st[ic][kh][kw];
                            }
                        }
                    }
                }

               output_3st[oc][oh][ow] = sum;
            }
        }
    }
    weight_file3.close();

    // Add bias
    for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT3; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH3; ow++) {
                output_3st[oc][oh][ow] += bias3[oc];
            }
        }
    }

    // Apply ReLU
    for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT3; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH3; ow++) {
                if (output_3st[oc][oh][ow] < 0) {
                    output_3st[oc][oh][ow] = 0;
                }
            }
        }
    }

    /*for (int oc = 0; oc < 384; oc++) {
        for (int oh = 0; oh < 13; oh++) {
            for (int ow = 0; ow < 13; ow++) {
                std::cout << "Output[" << oc << "][" << oh << "][" << ow << "] = " << output_3st[oc][oh][ow] << std::endl;
            }
        }
    }*/

    int INPUT_CHANNELS4 = 384;
    int INPUT_HEIGHT4 = 13;
    int INPUT_WIDTH4 = 13;
    int OUTPUT_CHANNELS4 = 256;
    int OUTPUT_HEIGHT4 = 13;
    int OUTPUT_WIDTH4 = 13;
    int KERNEL_SIZE4 = 3;
    int ZERO_PADDING4 = 1;
    int STRIDE_SIZE4 = 1;
    

    float bias4[OUTPUT_CHANNELS4];
    // Create convolution module
    //Convolution convolution("convolution");


    // Load weight data from file
    std::ifstream weight_file4("conv4_weight.txt");
    if (!weight_file4.is_open()) {
        std::cout << "Failed to open conv4_weight.txt" << std::endl;
        return ;
    }

    // Load bias data from file
    std::ifstream bias_file4("conv4_bias.txt");
    if (bias_file4.is_open()) {
        float temp;
        for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
            bias_file4 >> temp;
            bias4[oc] = temp;
        }
        bias_file4.close();
    } else {
        std::cout << "Failed to open conv4_bias.txt" << std::endl;
        return ;
    }

    // Perform convolution
    for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT4; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH4; ow++) {
                float sum = 0;

                for (int ic = 0; ic < INPUT_CHANNELS4; ic++) {
                    for (int kh = 0; kh < KERNEL_SIZE4; kh++) {
                        for (int kw = 0; kw < KERNEL_SIZE4; kw++) {
                            if(oh == 0 && ow == 0){
                                weight_file4 >> weight_4st[ic][kh][kw];
                            }
                            int ih = oh * STRIDE_SIZE4 + kh - ZERO_PADDING4;
                            int iw = ow * STRIDE_SIZE4 + kw - ZERO_PADDING4;

                            if (ih >= 0 && ih < INPUT_HEIGHT4 && iw >= 0 && iw < INPUT_WIDTH4) {
                                sum += output_3st[ic][ih][iw] * weight_4st[ic][kh][kw];
                            }
                        }
                    }
                }

               output_4st[oc][oh][ow] = sum;
            }
        }
    }
    weight_file4.close();

    // Add bias
    for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT4; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH4; ow++) {
                output_4st[oc][oh][ow] += bias4[oc];
            }
        }
    }

    // Apply ReLU
    for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT4; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH4; ow++) {
                if (output_4st[oc][oh][ow] < 0) {
                    output_4st[oc][oh][ow] = 0;
                }
            }
        }
    }

    /*for (int oc = 0; oc < 256; oc++) {
        for (int oh = 0; oh < 13; oh++) {
            for (int ow = 0; ow < 13; ow++) {
                std::cout << "Output[" << oc << "][" << oh << "][" << ow << "] = " << output_4st[oc][oh][ow] << std::endl;
            }
        }
    }*/

    int INPUT_CHANNELS5 = 256;
    int INPUT_HEIGHT5 = 13;
    int INPUT_WIDTH5 = 13;
    int OUTPUT_CHANNELS5 = 256;
    int OUTPUT_HEIGHT5 = 13;
    int OUTPUT_WIDTH5 = 13;
    int KERNEL_SIZE5 = 3;
    int ZERO_PADDING5 = 1;
    int STRIDE_SIZE5 = 1;
    int pool_size5 = 3;
    int pool_stride5 = 2;
    int pool_output_height5 = 6;
    int pool_output_width5 = 6;

    float output5[OUTPUT_CHANNELS5][OUTPUT_HEIGHT5][OUTPUT_WIDTH5];
    float bias5[OUTPUT_CHANNELS5];
    // Create convolution module
    //Convolution convolution("convolution");


    // Load weight data from file
    std::ifstream weight_file5("conv5_weight.txt");
    if (!weight_file5.is_open()) {
        std::cout << "Failed to open conv5_weight.txt" << std::endl;
        return ;
    }

    // Load bias5 data from file
    std::ifstream bias_file5("conv5_bias.txt");
    if (bias_file5.is_open()) {
        float temp;
        for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
            bias_file5 >> temp;
            bias5[oc] = temp;
        }
        bias_file5.close();
    } else {
        std::cout << "Failed to open conv5_bias.txt" << std::endl;
        return ;
    }

    // Perform convolution
    for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT5; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH5; ow++) {
                float sum = 0;

                for (int ic = 0; ic < INPUT_CHANNELS5; ic++) {
                    for (int kh = 0; kh < KERNEL_SIZE5; kh++) {
                        for (int kw = 0; kw < KERNEL_SIZE5; kw++) {
                            if(oh == 0 && ow == 0){
                                weight_file5 >> weight_5st[ic][kh][kw];
                            }
                            int ih = oh * STRIDE_SIZE5 + kh - ZERO_PADDING5;
                            int iw = ow * STRIDE_SIZE5 + kw - ZERO_PADDING5;

                            if (ih >= 0 && ih < INPUT_HEIGHT5 && iw >= 0 && iw < INPUT_WIDTH5) {
                                sum += output_4st[ic][ih][iw] * weight_5st[ic][kh][kw];
                            }
                        }
                    }
                }

               output5[oc][oh][ow] = sum;
            }
        }
    }
    weight_file5.close();

    // Add bias5
    for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT5; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH5; ow++) {
                output5[oc][oh][ow] += bias5[oc];
            }
        }
    }

    // Apply ReLU
    for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
        for (int oh = 0; oh < OUTPUT_HEIGHT5; oh++) {
            for (int ow = 0; ow < OUTPUT_WIDTH5; ow++) {
                if (output5[oc][oh][ow] < 0) {
                    output5[oc][oh][ow] = 0;
                }
            }
        }
    }

    // Max pooling
    
    for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
        for (int oh = 0; oh < pool_output_height5; oh++) {
            for (int ow = 0; ow < pool_output_width5; ow++) {
                float max_value = output5[oc][oh * pool_stride5][ow * pool_stride5];

                for (int kh = 0; kh < pool_size5; kh++) {
                    for (int kw = 0; kw < pool_size5; kw++) {
                        float value = output5[oc][oh * pool_stride5 + kh][ow * pool_stride5 + kw];
                        if (value > max_value) {
                            max_value = value;
                        }
                    }
                }

                output_5st[oc][oh][ow] = max_value;
            }
        }
    }

    /*for (int oc = 0; oc < 256; oc++) {
        for (int oh = 0; oh < 6; oh++) {
            for (int ow = 0; ow < 6; ow++) {
                std::cout << "output5[" << oc << "][" << oh << "][" << ow << "] = " << output_5st[oc][oh][ow] << std::endl;
            }
        }
    }*/

    float weight6 =0;
    float bias6 =0;

    // Load weight data from file
    std::ifstream weight_file6("fc6_weight.txt");
    if (!weight_file6.is_open()) {
        std::cout << "Failed to open fc6_weight.txt" << std::endl;
        return ;
    }

    // Load bias data from file
    std::ifstream bias_file6("fc6_bias.txt");
    if (!bias_file6.is_open()) {
        std::cout << "Failed to open fc6_bias.txt" << std::endl;
        return ;
    }

    for(int i = 0; i < 4096; i++){
        float sum = 0;
        for(int j = 0; j < 256; j++){
            for(int k = 0; k < 6; k++){
                for(int l = 0; l < 6; l++){
                    weight_file6 >> weight6;
                    sum += output_5st[j][k][l] * weight6;
                }
            }
        }
        bias_file6 >> bias6;
        fc_6st[i] = sum + bias6;
        //ReLU
        if(fc_6st[i] < 0){
            fc_6st[i] = 0;
        }
    }
    weight_file6.close();
    bias_file6.close();

    /*for (int oc = 0; oc < 4096; oc++) {
        std::cout << "Output[" << oc << "] = " << fc_6st[oc] << std::endl;
    }*/

    float weight7 =0;
    float bias7 =0;


    // Load weight data from file
    std::ifstream weight_file7("fc7_weight.txt");
    if (!weight_file7.is_open()) {
        std::cout << "Failed to open fc7_weight.txt" << std::endl;
        return ;
    }

    // Load bias data from file
    std::ifstream bias_file7("fc7_bias.txt");
    if (!bias_file7.is_open()) {
        std::cout << "Failed to open fc7_bias.txt" << std::endl;
        return ;
    }

    for(int i = 0; i < 4096; i++){
        float sum = 0;
        for(int j = 0; j < 4096; j++){
            weight_file7 >> weight7;
            sum += fc_6st[j] * weight7;
        }
        bias_file7 >> bias7;
        fc_7st[i] = sum + bias7;
        //ReLU
        if(fc_7st[i] < 0){
            fc_7st[i] = 0;
        }
    }
    weight_file7.close();
    bias_file7.close();

    /*for (int oc = 0; oc < 4096; oc++) {
        std::cout << "Output[" << oc << "] = " << fc_7st[oc] << std::endl;
    }*/

    float weight8 =0;
    float bias8 =0;


    // Load weight data from file
    std::ifstream weight_file8("fc8_weight.txt");
    if (!weight_file8.is_open()) {
        std::cout << "Failed to open fc8_weight.txt" << std::endl;
        return ;
    }

    // Load bias data from file
    std::ifstream bias_file8("fc8_bias.txt");
    if (!bias_file8.is_open()) {
        std::cout << "Failed to open fc8_bias.txt" << std::endl;
        return ;
    }

    for(int i = 0; i < 1000; i++){
        float sum = 0;
        for(int j = 0; j < 4096; j++){
            weight_file8 >> weight8;
            sum += fc_7st[j] * weight8;
        }
        bias_file8 >> bias8;
        fc_8st[i] = sum + bias8;
        //ReLU
        if(fc_8st[i] < 0){
            fc_8st[i] = 0;
        }
    }
    weight_file8.close();
    bias_file8.close();

    /*for (int oc = 0; oc < 1000; oc++) {
        std::cout << "Output[" << oc << "] = " << fc_8st[oc] << std::endl;
    }*/
    for(int i = 0; i < 1000; i++){
        output_final[i].write(fc_8st[i]);
    }
}


    SC_CTOR(alexnet2) {
        SC_METHOD(conv1);
        sensitive << str_1;
        dont_initialize();
    }
};


int sc_main(int argc, char* argv[]) {
    // Create signals
    sc_signal<bool> reset, clock;
    sc_signal<float> output_final[1000];
    sc_signal<string> conv1_txt;
    sc_buffer<string> conv2_txt;
    sc_fifo<string> conv3_txt;

    // Instantiate the modules
    alexnet1 module1("module1");
    alexnet2 module2("module2");
    alexnet3 module3("module3");
    alexnet4 module4("module4");
    

    Reset m_Reset( "m_Reset", 10 );
    Clock m_clock( "m_clock", 5, 40 );
    Monitor m_Monitor( "m_Monitor" );
    m_Reset( reset );
	m_clock( clock );
    m_Monitor.rst( reset );

    module1.str(conv1_txt);
    module2.str_1(conv1_txt);
    module3.str(conv2_txt);
    module2.str_2(conv2_txt);
    module4.str(conv3_txt);
    module2.str_3(conv3_txt);

    for(int i = 0; i < 1000; i++){
        module2.output_final[i](output_final[i]);
        m_Monitor.fc_8st[i](output_final[i]);
    }

   
    // Run simulation
    sc_start(500, SC_NS );

    return 0;
    
    /*sc_signal<float> ***output;
    // dynamic allocate 3-D array memory
    output = new sc_signal<float>**[192];
    for (int i = 0; i < 192; i++){
        output[i] = new sc_signal<float>*[13];
        for (int j = 0; j < 13; j++){
            output[i][j] = new sc_signal<float>[13];
        }
    }
    // operate
    for(int i = 0; i < 192; i++){
        for(int j = 0; j < 13; j++){
            for(int k = 0; k < 13; k++){
                output[i][j][k].write(0);
            }
        }
    }*/
}