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

    //bool no_packet_flag;      // Indicates if there are no packets to packet
    //bool tx_busy;          // Indicates if the transmitter is busy
    //int tx_cnt;  // Transmission counter


    int core_id;           // Core ID
    int src_id;            // Source ID
    int dest_id;           // Destination ID

    double alexnet_buff1  [150528]; 
    double alexnet_buff2  [193600];
    double fc_7st  [4096];
    double fc_8st  [1000];
    double weight_buff [9216];
    double bias_buff [4096];
    

    bool read_image_finish=0;
    bool read_weight_finish=0;
    bool read_bias_finish=0;
    bool compute_finish=0;
    bool tx_finish=0;
    bool print_flag = 1;
   

    int rx_src_id=0;
    int read_file_tpye=0; // 0 for image, 1 for weight, 2 for bias
    int rx_cnt=0;
    int out_channel_cnt=0;
    int alexnet_layer=0;
    int tx_cnt=0;



    SC_HAS_PROCESS(Core);

    // Main process
    void run() {
        if (rst.read() == 1) {
            initialize();
        } else {
            handle_receive();
            conv1();
            
            handle_transmit();
        }
    }

    // Initialize the core
    void initialize() {
        ready_rx.write(false);
        valid_tx.write(false);
        if(core_id == 0){
            alexnet_layer = 1;
        }
        else if(core_id == 1){
            alexnet_layer = 6;
        }
        else if(core_id == 4){
            alexnet_layer = 7;
        }
        else{
            alexnet_layer = 8;
        }
    }

    // Handle receiving data
    void handle_receive() {
        if(read_image_finish == 0 || read_weight_finish == 0 || read_bias_finish == 0){
            ready_rx.write(1);
            if(valid_rx.read() == 1){
                sc_lv<34> flit_rx_t = flit_rx.read();
                sc_dt::scfx_ieee_float rx_temp;
                rx_temp = convert_to_float(flit_rx_t);
                //cout<<"core id: "<<core_id<<", receive flit:"<<rx_temp<<endl;
                
                if(flit_rx_t.range(33,32) == "10"){// header flit
                    rx_src_id = int((sc_uint<4>)(sc_lv<4>)flit_rx.read().range(31,28));
                    read_file_tpye = int((sc_uint<2>)(sc_lv<2>)flit_rx.read().range(23,22));
                    if(print_flag == 1){
                        cout<<"core id: "<<core_id<<", receive header of type:"<<read_file_tpye<< ", from source:"<<rx_src_id<<endl;
                        print_flag = 0;
                    }
                    //cout<<"core id: "<<core_id<<", receive header of type:"<<read_file_tpye<< ", from source:"<<rx_src_id<<endl;
                    rx_cnt = 0;
                }
                else if(flit_rx_t.range(33,32) == "01"){// End of Packet
                    //cout<<"Time: "<<sc_time_stamp()<<", core receive tail, the flit number is:"<<rx_cnt<<endl;
                    switch(read_file_tpye){
                        case 0:
                            read_image_finish = 1;
                            break;
                        case 1:
                            read_weight_finish = 1;
                            break;
                        case 2:
                            read_bias_finish = 1;
                            break;
                    }
                    rx_cnt++;
                }
                else{
                    //tx body fit
                    rx_cnt++;
                }
                // store in input buffer
                if(rx_cnt > 0){
                    if(read_file_tpye == 0){ 
                        switch(core_id){
                            case 0:
                                alexnet_buff1[rx_cnt-1] = float(rx_temp);
                                //cout<<"core id: "<<core_id<<", receive image flit:"<<rx_temp<<endl;
                                break;
                            case 1:
                                alexnet_buff1[rx_cnt-1] = float(rx_temp);
                                //cout<<"core id: "<<core_id<<", receive image flit:"<<rx_temp<<endl;
                                break;
                            case 4:
                                alexnet_buff2[rx_cnt-1] = float(rx_temp);
                                //cout<<"core id: "<<core_id<<", receive image flit:"<<rx_temp<<endl;
                                break;
                            case 5:
                                fc_7st[rx_cnt-1] = float(rx_temp);
                                //cout<<"core id: "<<core_id<<", receive image flit:"<<rx_temp<<endl;
                                break;
                        }
                        //alexnet_buff1[rx_cnt-1] = float(rx_temp);
                    }
                    else if(read_file_tpye == 1){ // weight
                        weight_buff[rx_cnt-1] = float(rx_temp);
                        //cout<<"core id: "<<core_id<<", receive weight flit:"<<rx_temp<<endl;
                    }
                    else if(read_file_tpye == 2){ // bias
                        bias_buff[rx_cnt-1] = float(rx_temp);
                        //cout<<"core id: "<<core_id<<", receive bias flit:"<<rx_temp<<endl;
                    }
                }
            }
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
    int oc, oh, ow, ic, kh, kw, ih, iw;
    void conv1(){
        if(read_image_finish == 1 && read_weight_finish == 1 && read_bias_finish == 1 && compute_finish == 0){
            
            ready_rx.write(0);

            // Layer parameters
            const int INPUT_HEIGHT[] = {224, 27, 13, 13, 13, 9216, 4096, 4096};
            const int INPUT_WIDTH[] = {224, 27, 13, 13, 13, 9216, 4096, 4096};
            const int KERNEL_SIZE[] = {11, 5, 3, 3, 3, 1, 1, 1};
            const int STRIDE_SIZE[] = {4, 1, 1, 1, 1, 1, 1, 1};
            const int ZERO_PADDING[] = {2, 2, 1, 1, 1, 0, 0, 0};
            const int OUTPUT_CHANNELS[] = {64, 192, 384, 256, 256, 4096, 4096, 1000};
            const int OUTPUT_HEIGHT[] = {55, 27, 13, 13, 13, 9216, 4096, 4096};
            const int OUTPUT_WIDTH[] = {55, 27, 13, 13, 13, 9216, 4096, 4096};
            const int MAXPOOL_SIZE[] = {27, 13, 6, 6, 6, 1, 1, 1};

            // Current layer parameters
            int input_height = INPUT_HEIGHT[alexnet_layer - 1];
            int input_width = INPUT_WIDTH[alexnet_layer - 1];
            int kernel_size = KERNEL_SIZE[alexnet_layer - 1];
            int stride_size = STRIDE_SIZE[alexnet_layer - 1];
            int zero_padding = ZERO_PADDING[alexnet_layer - 1];
            int output_channels = OUTPUT_CHANNELS[alexnet_layer - 1];
            int output_height = OUTPUT_HEIGHT[alexnet_layer - 1];
            int output_width = OUTPUT_WIDTH[alexnet_layer - 1];
            int maxpool_out = MAXPOOL_SIZE[alexnet_layer - 1];

            int in_map_addr;
            int out_map_addr;
            int conv_addr;
            int maxpool_addr;

            // =========  Layer 1  ==========
            if(alexnet_layer == 1){ 
               //int INPUT_HEIGHT = 224;
                //int INPUT_WIDTH = 224;
                //int STRIDE_SIZE = 4;
                //int ZERO_PADDING = 2;                
                for (oh = 0; oh < 55 ; oh++ ){
                    for (ow = 0; ow < 55; ow++ ){
                        // Conv
                        out_map_addr = out_channel_cnt*output_height*output_width+oh*output_width+ow;
                        alexnet_buff2[out_map_addr] = bias_buff[out_channel_cnt];
                        for (ic = 0; ic < 3; ic++){
                            for (kh = 0; kh < 11; kh++){
                                for (kw = 0; kw < 11; kw++){
                                    // stride = 4, zero padding = 2
                                    ih = stride_size*ow + kw-zero_padding;
                                    iw = stride_size*oh + kh-zero_padding;

                                    in_map_addr = ic*input_height*input_width+iw*input_width+ih;
                                    conv_addr = ic*kernel_size*kernel_size+kh*kernel_size+kw;

                                    if (ih >= 0 && ih < input_height && iw >= 0 && iw < input_width){
                                        alexnet_buff2[out_map_addr] += weight_buff[conv_addr] * alexnet_buff1[in_map_addr];
                                    }
                                }
                            }
                        }
                        
                        // ReLU
                        if (alexnet_buff2[out_map_addr] < 0){
                            alexnet_buff2[out_map_addr] = 0;
                        }
                        //cout<<alexnet_buff2[out_channel_cnt*55*55+oh*55+ow]<<endl;
                    }
                }
                //cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 63){ // finish conv
                    // Maxpool
                    for (oc = 0; oc < 64 ; oc++){
                        for (oh = 0; oh < 27 ; oh++ ){
                            for (ow = 0; ow < 27; ow++ ){
                                maxpool_addr = oc*maxpool_out*maxpool_out+oh*maxpool_out+ow;

                                alexnet_buff2[maxpool_addr] = alexnet_buff2[oc*55*55+oh*2*55+ow*2];
                                for (ic = 0; ic < 3; ic++){
                                    for (kh = 0; kh < 3; kh++){

                                        alexnet_buff2[maxpool_addr] = max(alexnet_buff2[maxpool_addr], alexnet_buff2[oc*55*55+(oh*2+ic)*55+ow*2+kh]);
                                        
                                    }
                                }
        
                            }
                        }
                    }

                    /*for (int oc = 0; oc < 64; oc++) {
                        for (int oh = 0; oh < 27; oh++) {
                            for (int ow = 0; ow < 27; ow++) {
                                std::cout << "alexnet_buff2[" << oc*27*27+oh*27+ow << "] = " << alexnet_buff2[oc*27*27+oh*27+ow] << std::endl;
                            }
                        }
                    }*/
                    alexnet_layer++; // Next Layer
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
                
            }
            // =========  Layer 2  ==========
            else if(alexnet_layer == 2){ // Layer 2
                //int INPUT_HEIGHT = 27;
                //int INPUT_WIDTH = 27;
                //int STRIDE_SIZE = 1;
                //int ZERO_PADDING = 2;
                for (oh = 0; oh < 27 ; oh++ ){
                    for (ow = 0; ow < 27; ow++ ){
                        // Conv
                        out_map_addr = out_channel_cnt*output_height*output_width+oh*output_width+ow;
                        alexnet_buff1[out_map_addr] = bias_buff[out_channel_cnt];
                        for (ic = 0; ic < 64; ic++){
                            for (kh = 0; kh < 5; kh++){
                                for (kw = 0; kw < 5; kw++){
                                    ih = ow + kw-zero_padding;
                                    iw = oh + kh-zero_padding;

                                    in_map_addr = ic*input_height*input_width+iw*input_width+ih;
                                    conv_addr = ic*kernel_size*kernel_size+kh*kernel_size+kw;

                                    if (ih >= 0 && ih < input_height && iw >= 0 && iw < input_width){
                                        alexnet_buff1[out_map_addr] +=  weight_buff[conv_addr] * alexnet_buff2[in_map_addr];
                                    }
                                }
                            }
                        }
                        // ReLU
                        if (alexnet_buff1[out_map_addr] < 0){
                            alexnet_buff1[out_map_addr] = 0;
                        }
                        // cout << alexnet_buff2[oc][oh][ow] << " ";
                    }
                }
                cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 191){ // finish conv
                    // Maxpool
                    for (oc = 0; oc < 192 ; oc++){
                        for (oh = 0; oh < 13 ; oh++ ){
                            for (ow = 0; ow < 13; ow++ ){
                                maxpool_addr = oc*maxpool_out*maxpool_out+oh*maxpool_out+ow;
                                alexnet_buff1[maxpool_addr] = alexnet_buff1[oc*27*27+oh*2*27+ow*2];
                                for (ic = 0; ic < 3; ic++){
                                    for (kh = 0; kh < 3; kh++){

                                        alexnet_buff1[maxpool_addr] = max(alexnet_buff1[maxpool_addr], alexnet_buff1[oc*27*27+(oh*2+ic)*27+(ow*2+kh)]);

                                    }
                                }
                            }
                        }
                    }
                    /*for (int oc = 0; oc < 192; oc++) {
                        for (int oh = 0; oh < 13; oh++) {
                            for (int ow = 0; ow < 13; ow++) {
                                std::cout << "alexnet_buff1[" << 13*13*oc + 13*oh + ow<< "] = " << alexnet_buff1[13*13*oc + 13*oh + ow] << std::endl;
                            }
                        }
                    }*/
                    alexnet_layer++; // Next Layer
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                    
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            // =========  Layer 3  ==========
            else if(alexnet_layer == 3){ // Layer 3
                //int INPUT_HEIGHT = 13;
                //int INPUT_WIDTH = 13;
                //int STRIDE_SIZE = 1;
                //int ZERO_PADDING = 1;
                for (oh = 0; oh < 13 ; oh++ ){
                    for (ow = 0; ow < 13; ow++ ){
                        // Conv
                        out_map_addr = out_channel_cnt*output_height*output_width+oh*output_width+ow;
                        alexnet_buff2[out_map_addr] = bias_buff[out_channel_cnt];
                        for (ic = 0; ic < 192; ic++){
                            for (kh = 0; kh < 3; kh++){
                                for (kw = 0; kw < 3; kw++){
                                    ih = ow + kw-zero_padding;
                                    iw = oh + kh-zero_padding;

                                    in_map_addr = ic*input_height*input_width+iw*input_width+ih;
                                    conv_addr = ic*kernel_size*kernel_size+kh*kernel_size+kw;

                                    if (ih >= 0 && ih < input_height && iw >= 0 && iw < input_width){
                                        alexnet_buff2[out_map_addr] +=  weight_buff[conv_addr] * alexnet_buff1[in_map_addr];
                                    }
                                }
                            }
                        }
                        // ReLU
                        if (alexnet_buff2[out_map_addr] < 0){
                            alexnet_buff2[out_map_addr] = 0;
                        }
                    }
                }
                
                //cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 383){ // finish conv

                    /*for (int oc = 0; oc < 384; oc++) {
                        for (int oh = 0; oh < 13; oh++) {
                            for (int ow = 0; ow < 13; ow++) {
                                std::cout << "Output3[" << oc << "][" << oh << "][" << ow << "] = " << output_3st[oc][oh][ow] << std::endl;
                            }
                        }
                    }*/
                    alexnet_layer++; // Next Layer
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            // =========  Layer 4  ==========
            else if(alexnet_layer == 4){ // Layer 4
                //int INPUT_HEIGHT = 13;
                //int INPUT_WIDTH = 13;
                //int STRIDE_SIZE = 1;
                //int ZERO_PADDING = 1;
                for (oh = 0; oh < 13 ; oh++ ){
                    for (ow = 0; ow < 13; ow++ ){
                        // Conv
                        out_map_addr = out_channel_cnt*output_height*output_width+oh*output_width+ow;
                        alexnet_buff1[out_map_addr] = bias_buff[out_channel_cnt];
                        for (ic = 0; ic < 384; ic++){
                            for (kh = 0; kh < 3; kh++){
                                for (kw = 0; kw < 3; kw++){
                                    ih = ow + kw-zero_padding;
                                    iw = oh + kh-zero_padding;

                                    in_map_addr = ic*input_height*input_width+iw*input_width+ih;
                                    conv_addr = ic*kernel_size*kernel_size+kh*kernel_size+kw;

                                    if (ih >= 0 && ih < input_height && iw >= 0 && iw < input_width){
                                        alexnet_buff1[out_map_addr] +=  weight_buff[conv_addr] * alexnet_buff2[in_map_addr];
                                    }
                                }
                            }
                        }
                        // ReLU
                        if (alexnet_buff1[out_map_addr] < 0){
                            alexnet_buff1[out_map_addr] = 0;
                        }
                    }
                }
                
                cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 255){ // finish conv

                    /*for (int oc = 0; oc < 256; oc++) {
                        for (int oh = 0; oh < 13; oh++) {
                            for (int ow = 0; ow < 13; ow++) {
                                std::cout << "Output4[" << oc << "][" << oh << "][" << ow << "] = " << output_4st[oc][oh][ow] << std::endl;
                            }
                        }
                    }*/
                    alexnet_layer++; // Next Layer
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            // =========  Layer 5  ==========
            else if(alexnet_layer == 5){ // Layer 5
                //int INPUT_HEIGHT = 13;
                //int INPUT_WIDTH = 13;
                //int STRIDE_SIZE = 1;
                //int ZERO_PADDING = 1;
                for (oh = 0; oh < 13 ; oh++ ){
                    for (ow = 0; ow < 13; ow++ ){
                        // Conv
                        out_map_addr = out_channel_cnt*output_height*output_width+oh*output_width+ow;
                        alexnet_buff2[out_map_addr] = bias_buff[out_channel_cnt];
                        for (ic = 0; ic < 256; ic++){
                            for (kh = 0; kh < 3; kh++){
                                for (kw = 0; kw < 3; kw++){
                                    ih = ow + kw-zero_padding;
                                    iw = oh + kh-zero_padding;

                                    in_map_addr = ic*input_height*input_width+iw*input_width+ih;
                                    conv_addr = ic*kernel_size*kernel_size+kh*kernel_size+kw;

                                    if (ih >= 0 && ih < input_height && iw >= 0 && iw < input_width){
                                        alexnet_buff2[out_map_addr] += weight_buff[conv_addr] * alexnet_buff1[in_map_addr];
                                    }
                                }
                            }
                        }
                        // ReLU
                        if (alexnet_buff2[out_map_addr] < 0){
                            alexnet_buff2[out_map_addr] = 0;
                        }
                    }
                }
                //cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 255){ // finish conv
                    // Maxpool
                    for (oc = 0; oc < 256 ; oc++){
                        for (oh = 0; oh < 6 ; oh++ ){
                            for (ow = 0; ow < 6; ow++ ){
                                maxpool_addr = oc*maxpool_out*maxpool_out+oh*maxpool_out+ow;
                                alexnet_buff2[maxpool_addr] = alexnet_buff2[oc*13*13+oh*2*13+ow*2];
                                for (ic = 0; ic < 3; ic++){
                                    for (kh = 0; kh < 3; kh++){
                                        alexnet_buff2[maxpool_addr] = max(alexnet_buff2[maxpool_addr], alexnet_buff2[oc*13*13+(oh*2+ic)*13+ow*2+kh]);

                                    }
                                }
                                //cout << alexnet_buff2[oc*6*6+oh*6+ow] << " ";
                            }
                        }
                    }
                    /*for (int oc = 0; oc < 256; oc++) {
                        for (int oh = 0; oh < 6; oh++) {
                            for (int ow = 0; ow < 6; ow++) {
                                std::cout << "alexnet_buff2[" << oc*6*6+oh*6+ow << "] = " << alexnet_buff2[oc*6*6+oh*6+ow] << std::endl;
                            }
                        }
                    }*/

                    // Next Layer => FC 
                    out_channel_cnt = 0;
                    compute_finish = 1;
                }
                else{
                    out_channel_cnt++; // Next output channel
                    read_weight_finish = 0;
                } 
            }
            else if(alexnet_layer == 6){ // Layer 6
                // =========  Layer 6  ==========
                alexnet_buff2[out_channel_cnt] = bias_buff[out_channel_cnt];
                for (oh = 0; oh < 9216; oh++){
                    alexnet_buff2[out_channel_cnt] += alexnet_buff1[oh] *weight_buff[oh];
                }
                // ReLU
                if (alexnet_buff2[out_channel_cnt] < 0){
                    alexnet_buff2[out_channel_cnt] = 0;
                }
                //cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl; 
                if(out_channel_cnt == 4095){
                    /*for (int oc = 0; oc < 4096; oc++) {
                        std::cout << "Output[" << oc << "] = " << fc_6st[oc] << std::endl;
                    }*/
                    compute_finish = 1;
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            else if(alexnet_layer == 7){ // Layer 7
                // =========  Layer 7  ==========
                fc_7st[out_channel_cnt] = bias_buff[out_channel_cnt];
                for (oh = 0; oh < 4096; oh++){
                    fc_7st[out_channel_cnt] += alexnet_buff2[oh] * weight_buff[oh];
                }
                // ReLU
                if (fc_7st[out_channel_cnt] < 0){
                    fc_7st[out_channel_cnt] = 0;
                }
                cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 4095){
                    /*for (int oc = 0; oc < 4096; oc++) {
                        std::cout << "Output7[" << oc << "] = " << fc_7st[oc] << std::endl;
                    }*/
                    compute_finish = 1;
                    read_bias_finish = 0; 
                    out_channel_cnt = 0;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            else if(alexnet_layer == 8){ // Layer 8
                // =========  Layer 8  ==========
                fc_8st[out_channel_cnt] = bias_buff[out_channel_cnt];
                for (oh = 0; oh < 4096; oh++){
                    fc_8st[out_channel_cnt]  += fc_7st[oh] * weight_buff[oh];
                    
                }
                
                cout << out_channel_cnt << ": " << fc_8st[out_channel_cnt] <<"\n" ;
                cout<<"alexnet layer: "<<alexnet_layer<<", finished output channel: "<<out_channel_cnt<<endl;
                if(out_channel_cnt == 999){
                    /*for (int oc = 0; oc < 1000; oc++) {
                        std::cout << "fc_8st[" << oc << "] = " << fc_8st[oc] << std::endl;
                    }*/
                    out_channel_cnt = 0;
                    compute_finish = 1;
                }
                else{
                    out_channel_cnt++; // Next output channel
                }
                read_weight_finish = 0;
            }
            
        }
    }


    // Handle transmitting data
    void handle_transmit() {
        if(compute_finish == 1 && tx_finish == 0){
            if(ready_tx.read() == 1 && valid_tx == 1){
                tx_cnt++;
            }
            // transmit the packet
            sc_lv<34> flit_out;
            if(tx_cnt == 0){ // Begin of packet
                
                src_id = core_id;
                switch(core_id){
                    case 0:
                        dest_id = 1;
                        break;
                    case 1:
                        dest_id = 4;
                        break;
                    case 4:
                        dest_id = 5;
                        break;
                    case 5:
                        dest_id = 9;
                        break;
                }
                    
                
                cout<<"core id: "<<core_id<<", transmit header, source:"<<src_id<<", dest: "<<dest_id<<endl;
            
                flit_out.range(33, 32) = "10";
                flit_out.range(31, 28) = src_id;
                flit_out.range(27, 24) = dest_id;
                flit_out.range(23, 0) = "0";
                flit_tx.write(flit_out);
                valid_tx.write(true);
                
            }
            else {
                // floating point to sc_uint / sc_lv 
                float t_temp;
                switch(core_id){
                    case 0:
                        t_temp = alexnet_buff2[tx_cnt-1];
                        break;
                    case 1:
                        t_temp = alexnet_buff2[tx_cnt-1];
                        break;
                    case 4:
                        t_temp = fc_7st[tx_cnt-1];
                        break;
                    case 5:
                        t_temp = fc_8st[tx_cnt-1];
                        break;
                }

                sc_dt::scfx_ieee_float t_data(t_temp);

                sc_lv<32> flit_out_sc_lv = convert_to_sc_lv(t_data);
                flit_out.range(31, 0) = flit_out_sc_lv;
                //cout<<"tx_cnt"<<tx_cnt<<", "<<"flit_out:"<<flit_out<<endl;
                valid_tx.write(1);
                //transmit finish, send tail flit
                if((tx_cnt == 9216 && core_id == 0) || (tx_cnt == 4096 && core_id == 1) ||
                 (tx_cnt == 4096 && core_id == 4) || (tx_cnt == 1000 && core_id == 5)){ 
                    flit_out.range(33, 32) = "01";
                    flit_tx.write(flit_out);
                    tx_finish = 1;
                    //cout<<"core id: "<<core_id<<", transmit tail, source:"<<src_id<<", dest: "<<dest_id<<endl;
                }
                else {
                    flit_out.range(33, 32) = "00";
                    flit_tx.write(flit_out);
                }
            }
        }
        else{
            valid_tx.write(0);
        }
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
        //tx_busy = false;
        tx_cnt = 0;

        SC_METHOD(run);
        sensitive << clk.pos();
        dont_initialize();
    }
};

#endif
