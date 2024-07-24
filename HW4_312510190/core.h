#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <algorithm>
using namespace std;
//#include "pe.h"

SC_MODULE(Core) {
    // Input and output ports
    sc_in<bool> rst;
    sc_in<bool> clk;

    sc_in<sc_lv<34>> flit_rx;  // Input channel
    sc_in<bool> valid_rx;      // Input channel valid signal
    sc_out<bool> ready_rx;     // Input channel ready signal

    sc_out<sc_lv<34>> flit_tx; // Output channel
    sc_out<bool> valid_tx;     // Output channel valid signal
    sc_in<bool> ready_tx;      // Output channel ready signal

    //PE pe;                 // Processing Element
    
    //Packet *check_packet;       // Packet for receiving data
    //Packet *packet;      // Packet for transmitting data

    bool no_packet_flag;      // Indicates if there are no packets to packet
    bool tx_busy;          // Indicates if the transmitter is busy
    int tx_cnt;  // Transmission counter
    vector<float> data_in;    // Input data
    vector<float> data_out;   // Output data

    int core_id;           // Core ID
    int src_id;            // Source ID
    int dest_id;           // Destination ID
    bool input_finish;

    vector<float> alexnet_data;
    int rx_cnt = 0;
    int put_count = 0;

    float fc_8st[1000];
    bool compute_finish = false;

    SC_HAS_PROCESS(Core);

    // Main process
    void run() {
        if (rst.read() == 1) {
            initialize();
        } else {
            handle_receive();
            if(input_finish){
                conv1();
                compute_finish = true;
                cout<<"core compute finish"<<endl;
                input_finish = false;
            }
            handle_transmit();
        }
    }
    

    // Initialize the core
    void initialize() {
        ready_rx.write(false);
        valid_tx.write(false);
    }

    // Handle receiving data
    void handle_receive() {
        ready_rx.write(true);
        if (valid_rx.read() == 1) {
            sc_lv<34> flit_rx_t = flit_rx.read();
            

            if (flit_rx_t.range(33, 32) == "00") {
                sc_dt::scfx_ieee_float rx_temp;
                rx_temp = convert_to_float(flit_rx_t);
                //cout<<"Time: "<<sc_time_stamp()<<", flit_rx_t:"<<rx_temp<<endl;

                alexnet_data.push_back(rx_temp);
                rx_cnt++;
                /*if(rx_cnt == 61251370){
                    input_finish = true;
                }*/
            }
            else if(flit_rx_t.range(33, 32) == "01"){
                cout<<"Time: "<<sc_time_stamp()<<", core receive tail, the flit number is:"<<rx_cnt<<endl;
                input_finish = true;
            }
            
        }
        
        if(input_finish){
            cout<<"core check_packet finish"<<endl;

        }
    }

    // Convert sc_lv to sc_dt::scfx_ieee_float
    sc_dt::scfx_ieee_float convert_to_float( sc_lv<34> flit_rx_t) {
        sc_dt::scfx_ieee_float rx_temp;
        rx_temp.negative(int((sc_uint<1>)(sc_lv<1>)flit_rx_t[31]));
        rx_temp.exponent(int((sc_uint<8>)(sc_lv<8>)flit_rx_t.range(30,23)));
        rx_temp.mantissa(int((sc_uint<23>)(sc_lv<23>)flit_rx_t.range(22,0)));
        
        /*float temp;
        sc_dt::scfx_ieee_float id(temp); // convert to IEEE 754 bitfield
        bool sgn = (flit_rx_t[31] == "1");
        sc_uint<8> exp = flit_rx_t.range(30, 23).to_uint();
        sc_uint<23> man = flit_rx_t.range(22, 0).to_uint();
        id.negative(sgn);
        id.exponent(exp);
        id.mantissa(man);*/
        return rx_temp;
    }

    void conv1(){
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

        for (int ic = 0; ic < 3; ic++) {
            for (int ih = 0; ih < 224; ih++) {
                for (int iw = 0; iw < 224; iw++) {
                    input[ic][ih][iw] = alexnet_data[put_count];
                    put_count++;
                    //cout<<"input["<<ic<<"]["<<ih<<"]["<<iw<<"] = "<<input[ic][ih][iw]<<endl;
                }
            }
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
                                    weight_1st[ic][kh][kw] = alexnet_data[put_count];
                                    put_count++;
                                    //cout<<"weight_1st["<<ic<<"]["<<kh<<"]["<<kw<<"] = "<<weight_1st[ic][kh][kw]<<endl;
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

        for (int oc = 0; oc < OUTPUT_CHANNELS; oc++) {
            bias[oc] = alexnet_data[put_count];
            put_count++;
            //cout<<"bias["<<oc<<"] = "<<bias[oc]<<endl;
        }

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


        // Perform convolution
        for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
            for (int oh = 0; oh < OUTPUT_HEIGHT2; oh++) {
                for (int ow = 0; ow < OUTPUT_WIDTH2; ow++) {
                    float sum = 0;

                    for (int ic = 0; ic < INPUT_CHANNELS2; ic++) {
                        for (int kh = 0; kh < KERNEL_SIZE2; kh++) {
                            for (int kw = 0; kw < KERNEL_SIZE2; kw++) {
                                if(oh == 0 && ow == 0){
                                    weight_2st[ic][kh][kw] = alexnet_data[put_count];
                                    //cout<<"weight_2st["<<ic<<"]["<<kh<<"]["<<kw<<"] = "<<weight_2st[ic][kh][kw]<<endl;
                                    put_count++;
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
        put_count += 7;
        alexnet_data[put_count] = -0.109280;
        for (int oc = 0; oc < OUTPUT_CHANNELS2; oc++) {
            bias2[oc] = alexnet_data[put_count];
            put_count++;
            //cout<<"bias2["<<oc<<"] = "<<bias2[oc]<<", put_cnt: "<<put_count<<endl;
        }

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

        // Perform convolution
        for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
            for (int oh = 0; oh < OUTPUT_HEIGHT3; oh++) {
                for (int ow = 0; ow < OUTPUT_WIDTH3; ow++) {
                    float sum = 0;

                    for (int ic = 0; ic < INPUT_CHANNELS3; ic++) {
                        for (int kh = 0; kh < KERNEL_SIZE3; kh++) {
                            for (int kw = 0; kw < KERNEL_SIZE3; kw++) {
                                if(oh == 0 && ow == 0){
                                    weight_3st[ic][kh][kw] = alexnet_data[put_count];
                                    put_count++;
                                    //cout<<"weight_3st["<<ic<<"]["<<kh<<"]["<<kw<<"] = "<<weight_3st[ic][kh][kw]<<", put_cnt: "<<put_count<<endl;
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
        
        for (int oc = 0; oc < OUTPUT_CHANNELS3; oc++) {
            bias3[oc] = alexnet_data[put_count];
            put_count++;
            //cout<<"bias3["<<oc<<"] = "<<bias3[oc]<<", put_cnt: "<<put_count<<endl;
        }

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



        // Perform convolution
        for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
            for (int oh = 0; oh < OUTPUT_HEIGHT4; oh++) {
                for (int ow = 0; ow < OUTPUT_WIDTH4; ow++) {
                    float sum = 0;

                    for (int ic = 0; ic < INPUT_CHANNELS4; ic++) {
                        for (int kh = 0; kh < KERNEL_SIZE4; kh++) {
                            for (int kw = 0; kw < KERNEL_SIZE4; kw++) {
                                if(oh == 0 && ow == 0){
                                    weight_4st[ic][kh][kw] = alexnet_data[put_count];
                                    put_count++;
                                    //cout<<"weight_4st["<<ic<<"]["<<kh<<"]["<<kw<<"] = "<<weight_4st[ic][kh][kw]<<", put_cnt: "<<put_count<<endl;
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
        put_count += 8;
        for (int oc = 0; oc < OUTPUT_CHANNELS4; oc++) {
            bias4[oc] = alexnet_data[put_count];
            put_count++;
            //cout<<"bias4["<<oc<<"] = "<<bias4[oc]<<", put_cnt: "<<put_count<<endl;
        }

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
                    std::cout << "Output4[" << oc << "][" << oh << "][" << ow << "] = " << output_4st[oc][oh][ow] << std::endl;
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


        // Perform convolution
        for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
            for (int oh = 0; oh < OUTPUT_HEIGHT5; oh++) {
                for (int ow = 0; ow < OUTPUT_WIDTH5; ow++) {
                    float sum = 0;

                    for (int ic = 0; ic < INPUT_CHANNELS5; ic++) {
                        for (int kh = 0; kh < KERNEL_SIZE5; kh++) {
                            for (int kw = 0; kw < KERNEL_SIZE5; kw++) {
                                if(oh == 0 && ow == 0){
                                    weight_5st[ic][kh][kw] = alexnet_data[put_count];
                                    put_count++;
                                    //cout<<"weight_5st["<<ic<<"]["<<kh<<"]["<<kw<<"] = "<<weight_5st[ic][kh][kw]<<", put_cnt: "<<put_count<<endl;
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
        
        
        for (int oc = 0; oc < OUTPUT_CHANNELS5; oc++) {
            bias5[oc] = alexnet_data[put_count];
            put_count++;
            //cout<<"bias5["<<oc<<"] = "<<bias5[oc]<<", put_cnt: "<<put_count<<endl;
        }

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


        for(int i = 0; i < 4096; i++){
            float sum = 0;
            for(int j = 0; j < 256; j++){
                for(int k = 0; k < 6; k++){
                    for(int l = 0; l < 6; l++){
                        weight6  = alexnet_data[put_count];
                        put_count++;
                        //cout<<"weight6["<<j<<"]["<<k<<"]["<<l<<"] = "<<weight6<<", put_cnt: "<<put_count<<endl;
                        sum += output_5st[j][k][l] * weight6;
                    }
                }
            }
            fc_6st[i] = sum;
                        
        }
        put_count += 8;

        //give bias
        for (int oc = 0; oc < 4096; oc++) {
            bias6 = alexnet_data[put_count];
            put_count++;
            fc_6st[oc] += bias6;
            //ReLU
            if(fc_6st[oc] < 0){
                fc_6st[oc] = 0;
            }
            //cout<<"bias6["<<oc<<"] = "<<bias6<<", put_cnt: "<<put_count<<endl;
        }


        /*for (int oc = 0; oc < 4096; oc++) {
            std::cout << "Output[" << oc << "] = " << fc_6st[oc] << std::endl;
        }*/

        float weight7 =0;
        float bias7 =0;


        for(int i = 0; i < 4096; i++){
            float sum = 0;
            for(int j = 0; j < 4096; j++){
                weight7 = alexnet_data[put_count];
                put_count++;
                //cout<<"weight7["<<j<<"] = "<<weight7<<", put_cnt: "<<put_count<<endl;
                sum += fc_6st[j] * weight7;
            }
            fc_7st[i] = sum;
            
        }
        
        
        //give bias
        for (int oc = 0; oc < 4096; oc++) {
            bias7 = alexnet_data[put_count];
            put_count++;
            fc_7st[oc] += bias7;
            //ReLU
            if(fc_7st[oc] < 0){
                fc_7st[oc] = 0;
            }
            //cout<<"bias7["<<oc<<"] = "<<bias7<<", put_cnt: "<<put_count<<endl;
        }

        /*for (int oc = 0; oc < 4096; oc++) {
            std::cout << "Output7[" << oc << "] = " << fc_7st[oc] << std::endl;
        }*/

        float weight8 =0;
        float bias8 =0;

        for(int i = 0; i < 1000; i++){
            float sum = 0;
            for(int j = 0; j < 4096; j++){
                weight8 = alexnet_data[put_count];
                put_count++;
                //cout<<"weight8["<<j<<"] = "<<weight8<<", put_cnt: "<<put_count<<endl;
                sum += fc_7st[j] * weight8;
            }
            fc_8st[i] = sum ;
            
        }
        
        put_count += 8;
        //give bias
        for (int oc = 0; oc < 1000; oc++) {
            bias8 = alexnet_data[put_count];
            put_count++;
            fc_8st[oc] += bias8;
            //ReLU
            if(fc_8st[oc] < 0){
                fc_8st[oc] = 0;
            }
            //cout<<"bias8["<<oc<<"] = "<<bias8<<", put_cnt: "<<put_count<<endl;
        }

        /*for (int oc = 0; oc < 1000; oc++) {
            std::cout << "Output8[" << oc << "] = " << fc_8st[oc] << std::endl;
        }*/
        
        
    }


    // Handle transmitting data
    void handle_transmit() {
        if (compute_finish) {
            if (ready_tx.read() == 1) {
                update_tx_status();
            }
            if (tx_busy) {
                send_packet();
            }
            
        }
    }

    // Update transmission status
    void update_tx_status() {
        if (tx_cnt == 1002) {
            cout<<"core send finish"<<endl;
            tx_busy = false;
            tx_cnt = 0;
        } else {
            tx_busy = true;
            tx_cnt++;
        }
    }

    // Send the packet
    void send_packet() {
        
        if (tx_cnt == 1) {
            send_header();
        } else {
            send_body_or_tail();
        }
    }

    // Send the packet header
    void send_header() {
        sc_lv<34> flit_out;
        src_id = core_id;
        dest_id = 4;
        cout<<"core transmit header, source:"<<src_id<<", dest: "<<dest_id<<endl;
        
        flit_out.range(33, 32) = "10";
        flit_out.range(31, 28) = src_id;
        flit_out.range(27, 24) = dest_id;
        flit_out.range(23, 0) = "0";
        flit_tx.write(flit_out);
        valid_tx.write(true);
    }

    // Send the packet body or tail
    void send_body_or_tail() {
        sc_lv<34> flit_out;
        sc_dt::scfx_ieee_float tx_temp(fc_8st[tx_cnt - 2]);
        
        sc_lv<32> flit_out_sc_lv = convert_to_sc_lv(tx_temp);
        flit_out.range(31, 0) = flit_out_sc_lv;
        if (tx_cnt == 1001) {
            flit_out.range(33, 32) = "01";
            
            flit_tx.write(flit_out);
        } else {
            flit_out.range(33, 32) = "00";

            flit_tx.write(flit_out);
        }
        //cout<<"tx_cnt"<<tx_cnt<<", "<<"flit_out:"<<flit_out<<endl;
        valid_tx.write(true);
    }

    // Convert sc_dt::scfx_ieee_float to sc_lv
    sc_lv<34> convert_to_sc_lv( sc_dt::scfx_ieee_float tx_temp) {
        sc_lv<1> sgn = tx_temp.negative();
        sc_lv<8> exp = tx_temp.exponent();
        sc_lv<23> mnt = tx_temp.mantissa();
        // prepare parts
        //sc_dt::sc_uint<1> sgn = tx_temp.negative();
        //sc_dt::sc_uint<8> exp = tx_temp.exponent();
        //sc_dt::sc_uint<23> mnt = tx_temp.mantissa();
        return (sgn, exp, mnt);
    }

    // Constructor
    Core(sc_module_name name, int _Core_ID) : sc_module(name), core_id(_Core_ID) {
        //pe.init(core_id);
        tx_busy = false;
        tx_cnt = 0;

        SC_METHOD(run);
        sensitive << clk.pos();
        dont_initialize();
    }
};

#endif
