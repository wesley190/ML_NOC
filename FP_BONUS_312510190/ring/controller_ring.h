#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "systemc.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

SC_MODULE( Controller ) {
    sc_in  < bool >  rst;
    sc_in  < bool >  clk;
    
    // to ROM
    sc_out < int >   layer_id;       // '0' means input data
    sc_out < bool >  layer_id_type;  // '0' means weight, '1' means bias (for layer_id == 0, we don't care this signal)
    sc_out < bool >  layer_id_valid;

    // from ROM
    sc_in  < float > data;
    sc_in  < bool >  data_valid;
    
    // to router0
    sc_out < sc_lv<34> > flit_tx;
    sc_out < bool > valid_tx;
    sc_in  < bool > ready_tx;

    // from router0
    sc_in  < sc_lv<34> > flit_rx;
    sc_in  < bool > valid_rx;
    sc_out < bool > ready_rx;

    int cnt_layer=0;
    bool cnt_type=0;
    int ready_to_receive=0;
    bool stop_receive=0;
    int rx_dara_cnt = 0;
    int tx_dara_cnt = 0;
    int tx_cnt = 0;
    int layer_tmp = 0;

    float fc_8st[1000];

    float store_buff[37752832];
    int alexnet_layer = 0;
    int tx_amount_one_channel = 0;
    int input_size = 0;

    bool print_flag=0;
    int i =0;

    void run(){
        if(rst.read() == 1){
            initialize();
        }
        else{
            receive_from_ROM();
            transmit_to_router();
            receive_from_router0();
            result();
        }
    }

    void initialize(){
        valid_tx.write(false);
        ready_rx.write(false);
        ready_to_receive = 0;
        stop_receive = 0;

    }
    
    void receive_from_ROM(){
        //layer_id.write(cnt_layer);
        //layer_id_type.write(cnt_type);
        //layer_id_valid.write(0);

        switch(alexnet_layer) {
            case 0:
                layer_id.write(0);
                layer_id_valid.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<0<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 150528;
                break;

            case 1:
            
                layer_id.write(1);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<1<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 64;
                break;

            case 2:
                layer_id.write(1);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<1<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 3*11*11*64;
                input_size = 3*11*11;
                break;

            case 3:
                layer_id.write(2);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<2<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 192;
                break;

            case 4:
                layer_id.write(2);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<2<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 64*5*5*192;
                input_size = 64*5*5;
                break;

            case 5:
                layer_id.write(3);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<3<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 384;
                break;

            case 6:
                layer_id.write(3);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<3<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 192*3*3*384;
                input_size = 192*3*3;
                break;

            case 7:
                layer_id.write(4);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<4<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 256;
                break;

            case 8:
                layer_id.write(4);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<4<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 384*3*3*256;
                input_size = 384*3*3;
                break;

            case 9:
                layer_id.write(5);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<5<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 256;
                break;

            case 10:
                layer_id.write(5);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<5<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 256*3*3*256;
                input_size = 256*3*3;
                break;

            case 11:
                layer_id.write(6);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<6<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 4096;
                break;

            case 12:
                layer_id.write(6);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<6<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 9216*4096;
                input_size = 9216;
                break;

            case 13:
                layer_id.write(7);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<7<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 4096;
                break;

            case 14:
                layer_id.write(7);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<7<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 4096*4096;
                input_size = 4096;
                break;

            case 15:
                layer_id.write(8);
                layer_id_valid.write(1);
                layer_id_type.write(1);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<8<<", type: "<<1<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 1000;
                break;

            case 16:
                layer_id.write(8);
                layer_id_valid.write(1);
                layer_id_type.write(0);
                cout<<"the time: "<<sc_time_stamp()<< ", controller start to receive data, layer: "<<8<<", type: "<<0<<endl;
                alexnet_layer = -1;
                tx_amount_one_channel = 4096*1000;
                input_size = 4096;
                break;

            default:             
                layer_id.write(0);
                layer_id_type.write(0);
                layer_id_valid.write(0);
                if (data_valid.read() == 1) {
                    //cout<<"the time: "<<sc_time_stamp()<<", controller receive data"<<data.read()<<endl;
                    store_buff[rx_dara_cnt] = data;
                    rx_dara_cnt++;
                }
                break;
        }
    }

    void transmit_to_router(){
        if(ready_tx.read() == 1 && valid_tx == 1){
            //cout<<"the time: "<<sc_time_stamp()<<", the controller ready to transmit to router0 "<<endl;    
            if(tx_dara_cnt == tx_amount_one_channel-1){
                rx_dara_cnt = 0;
                tx_dara_cnt = 0;
                tx_cnt = 0;
                layer_tmp++;
                alexnet_layer = layer_tmp;
            }
            else if(tx_cnt == input_size && layer_tmp%2 == 0 && layer_tmp != 0){
                tx_cnt = 0;
                tx_dara_cnt++;
            }
            else{
                if(tx_cnt != 0){
                    tx_dara_cnt++;
                }
                tx_cnt++;
            }
            
        }
        if(rx_dara_cnt != 0){
            if(tx_cnt == 0){ // Begin of packet
                // transmit header
                sc_lv<34> flit_out;
                flit_out.range(33, 28) = "101111";
                flit_out.range(21, 0) = "0";
                if(layer_tmp == 0){   // transmit image
                    flit_out.range(27, 24) = "0000";
                    flit_out.range(23, 22) = "00";
                    flit_tx.write(flit_out);
                    //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit image header flit to core: "<<0<<endl;
                }
                else if(layer_tmp % 2 == 0){ // transmit weight
                    if(layer_tmp < 11){
                        flit_out.range(27, 24) = "0000";
                        flit_out.range(23, 22) = "01";
                        flit_tx.write(flit_out);
                        // cout<<"the time: "<<sc_time_stamp()<< ", controller transmit weight header flit to core: "<<0<<endl;
                    }
                    else if(layer_tmp < 13){ // FC to core 1
                        flit_out.range(27, 24) = "0001";
                        flit_out.range(23, 22) = "01";
                        flit_tx.write(flit_out);
                        // cout<<"the time: "<<sc_time_stamp()<< ", controller transmit weight header flit to core: "<<1<<endl;
                    }
                    else if(layer_tmp < 15){ // FC to core 4
                        flit_out.range(27, 24) = "0010";
                        flit_out.range(23, 22) = "01";
                        flit_tx.write(flit_out);
                        //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit weight header flit to core: "<<4<<endl;
                    }
                    else{   // FC to core 5
                        flit_out.range(27, 24) = "0011";
                        flit_out.range(23, 22) = "01";
                        flit_tx.write(flit_out);
                        //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit weight header flit to core: "<<5<<endl;
                    }
                }
                else{ // transmit bias
                    if(layer_tmp < 11){
                        flit_out.range(27, 24) = "0000";
                        flit_out.range(23, 22) = "10";
                        flit_tx.write(flit_out);
                        // cout<<"the time: "<<sc_time_stamp()<< ", controller transmit bias header flit to core: "<<0<<endl;
                    }
                    else if(layer_tmp < 13){ // FC to core 1
                        flit_out.range(27, 24) = "0001";
                        flit_out.range(23, 22) = "10";
                        flit_tx.write(flit_out);
                        // cout<<"the time: "<<sc_time_stamp()<< ", controller transmit bias header flit to core: "<<1<<endl;
                    }
                    else if(layer_tmp < 15){ // FC to core 4
                        flit_out.range(27, 24) = "0010";
                        flit_out.range(23, 22) = "10";
                        flit_tx.write(flit_out);
                        //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit bias header flit to core: "<<4<<endl;
                    }
                    else{ // FC to core 5
                        flit_out.range(27, 24) = "0011";
                        flit_out.range(23, 22) = "10";
                        flit_tx.write(flit_out);
                        //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit bias header flit to core: "<<5<<endl;
                    }
                }
                valid_tx.write(1);
            }
            else {
                //cout<<"time: "<<sc_time_stamp()<<", tx_dara_cnt: "<<tx_dara_cnt<<", tx_cnt: "<<tx_cnt<<", layer_tmp: "<<layer_tmp<<"\n";
                sc_lv<32> flit_out_sc_lv = convert_to_sc_lv(store_buff[tx_dara_cnt]);
                
                valid_tx.write(1);
                if((tx_cnt == input_size && layer_tmp%2 == 0 && layer_tmp != 0) || tx_dara_cnt == (tx_amount_one_channel-1)){ // End of Packet
                    flit_tx.write(("01", flit_out_sc_lv));
                    //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit flit : "<<("01", flit_out_sc_lv)<<endl;
                }
                else {
                    flit_tx.write(("00", flit_out_sc_lv));
                    //cout<<"the time: "<<sc_time_stamp()<< ", controller transmit flit : "<<("00", flit_out_sc_lv)<<endl;
                }
            }   
        }
        else{
            valid_tx.write(0);
        }

    }

    // Convert sc_dt::scfx_ieee_float to sc_lv
    sc_lv<32> convert_to_sc_lv( sc_dt::scfx_ieee_float tx_temp) {
        sc_lv<1> sgn = tx_temp.negative();
        sc_lv<8> exp = tx_temp.exponent();
        sc_lv<23> mnt = tx_temp.mantissa();
        
        return (sgn, exp, mnt);
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

    void receive_from_router0(){
        ready_rx.write(true);
        if (valid_rx.read() == 1) {
            //cout<<"core check_packet flit_rx_t"<<endl;
            sc_lv<34> flit_rx_t = flit_rx.read();
            //cout<<"the time: "<<sc_time_stamp()<<", the controller receive from router0 flit_rx_t:"<<flit_rx_t<<endl;
            //std::string flit_type = flit_rx_t.range(33, 32).to_string();

            if (flit_rx_t.range(33, 32) == "10") {
                //cout<<"the time: "<<sc_time_stamp()<< ", controller receive header: "<<endl;
            } else {
                sc_dt::scfx_ieee_float rx_temp;
                rx_temp = convert_to_float(flit_rx_t);

                if (flit_rx_t.range(33, 32) == "01") {
                    // End of packet
                    fc_8st[999] = rx_temp;
                    print_flag = true;

                } else {
                    // Body of packet
                    fc_8st[i] = rx_temp;
                    i++;
                }
            }
        }
    }

    void result(){
        if(print_flag == 1){
            print_flag = 0;
            float f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0;
            int idx1 = 0, idx2 = 0, idx3 = 0, idx4 = 0, idx5 = 0;
            string s1, s2, s3, s4, s5, temp_str;

            ifstream class_name_file("/RAID2/COURSE/mlchip/mlchip004/hw4/data/imagenet_classes.txt");
            if(!class_name_file){
                cout << "Can't open imagenet_classes.txt for reading\n";
            }
            cout<<"||=========================================================||\n";
            cout<<left<<"||  "<<setw(3)<<"idx"<<"  |    "<<setw(7)<<"value"<<"   |      "<<setw(20)<<"class name"<<"        ||\n";
            cout<<"||=========================================================||\n";
            for(int i=0;i<1000;i++){
                getline(class_name_file, temp_str);
                if(fc_8st[i] > f1){
                    f5 = f4;
                    f4 = f3;
                    f3 = f2;
                    f2 = f1;
                    f1 = fc_8st[i];
                    idx5 = idx4;
                    idx4 = idx3;
                    idx3 = idx2;
                    idx2 = idx1;
                    idx1 = i;
                    s5 = s4;
                    s4 = s3;
                    s3 = s2;
                    s2 = s1;
                    s1 = temp_str;
                }
                else if(fc_8st[i] > f2){
                    f5 = f4;
                    f4 = f3;
                    f3 = f2;
                    f2 = fc_8st[i];
                    idx5 = idx4;
                    idx4 = idx3;
                    idx3 = idx2;
                    idx2 = i;
                    s5 = s4;
                    s4 = s3;
                    s3 = s2;
                    s2 = temp_str;
                }
                else if(fc_8st[i] > f3){
                    f5 = f4;
                    f4 = f3;
                    f3 = fc_8st[i];
                    idx5 = idx4;
                    idx4 = idx3;
                    idx3 = i;
                    s5 = s4;
                    s4 = s3;
                    s3 = temp_str;
                }
                else if(fc_8st[i] > f4){
                    f5 = f4;
                    f4 = fc_8st[i];
                    idx5 = idx4;
                    idx4 = i;
                    s5 = s4;
                    s4 = temp_str;
                }
                else if(fc_8st[i] > f5){
                    f5 = fc_8st[i];
                    idx5 = i;
                    s5 = temp_str;
                }
            }
            cout<<left<<"||  "<<setw(3)<<idx1<<"  |    "<<setw(7)<<f1<<"   |      "<<setw(20)<<s1<<"        ||\n";
            cout<<left<<"||  "<<setw(3)<<idx2<<"  |    "<<setw(7)<<f2<<"   |      "<<setw(20)<<s2<<"        ||\n";
            cout<<left<<"||  "<<setw(3)<<idx3<<"  |    "<<setw(7)<<f3<<"   |      "<<setw(20)<<s3<<"        ||\n";
            cout<<left<<"||  "<<setw(3)<<idx4<<"  |    "<<setw(7)<<f4<<"   |      "<<setw(20)<<s4<<"        ||\n";
            cout<<left<<"||  "<<setw(3)<<idx5<<"  |    "<<setw(7)<<f5<<"   |      "<<setw(20)<<s5<<"        ||\n";
            cout<<"||=========================================================||\n";
            class_name_file.close();
            cout<<"the time: "<<sc_time_stamp()<<", controller of ring topology finish the simulation\n";
            sc_stop();  
        }
          
    }

    SC_CTOR( Controller ){
        SC_METHOD( run );
        sensitive << clk.pos() ;
        dont_initialize();
    }

};

#endif