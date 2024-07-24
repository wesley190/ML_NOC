#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "systemc.h"

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
    bool ready_to_receive=0;
    bool stop_receive=0;
    int tx_cnt=0;

    vector<float> fc_8st;
    bool print_flag=0;

    void run(){
        if(rst.read() == 1){
            initialize();
        }
        else{
            receive_from_ROM_transmit_to_router();
            receive_from_router0();
            result();
        }
    }

    void initialize(){
        valid_tx.write(false);
        ready_rx.write(false);
    }
    
    void receive_from_ROM_transmit_to_router(){
        //layer_id.write(cnt_layer);
        //layer_id_type.write(cnt_type);
        layer_id_valid.write(0);

        if(data_valid.read()){
            //cout<<"the time: "<<sc_time_stamp()<< "controller receive data"<<data.read()<<endl;
            sc_lv<34> flit_out;
            sc_dt::scfx_ieee_float tx_temp(data.read());

            sc_lv<32> flit_out_sc_lv = convert_to_sc_lv(tx_temp);
            flit_out.range(31, 0) = flit_out_sc_lv;
            flit_out.range(33, 32) = "00";
            flit_tx.write(flit_out);
            tx_cnt++;
            //cout<<"tx_cnt"<<tx_cnt<<", "<<"flit_out:"<<flit_out<<endl;
            valid_tx.write(true);
        }
        else{
            //cout<<"the time: "<<sc_time_stamp()<< " test"<<endl;
            ready_to_receive ^= 1;
            valid_tx.write(false);
        }

        if(tx_cnt == 0){
            sc_lv<34> flit_out;
            int dest_id = 0;
            
            flit_out.range(33, 28) = "100000";
            flit_out.range(27, 24) = dest_id;
            flit_out.range(23, 0) = "0";
            flit_tx.write(flit_out);
            valid_tx.write(true);
            tx_cnt++;
        }
        else if(tx_cnt == 61251369){
            sc_lv<34> flit_out;
            
            flit_out.range(33, 28) = "010000";
            flit_out.range(27, 0) = "0";
            flit_tx.write(flit_out);
            valid_tx.write(true);
            tx_cnt++;
        }

        if(ready_to_receive && !stop_receive){
            cout<<"the time: "<<sc_time_stamp()<< ", controller ready to receive data, layer: "<<cnt_layer<<", type: "<<cnt_type<<endl;
            layer_id.write(cnt_layer);
            layer_id_type.write(cnt_type);
            layer_id_valid.write(1);
            
            if(cnt_layer == 0){
                cnt_layer++;
            }
            else if(cnt_type==0){
                cnt_type=1;
            }
            else{
                cnt_type=0;
                cnt_layer++;
                if(cnt_layer == 9){
                    stop_receive = 1;
                    //cout<<"the time: "<<sc_time_stamp()<< ", controller total transmit tx_cnt:"<<tx_cnt<<" data"<<endl;
                }
            }
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
                    fc_8st.push_back(rx_temp);
                    print_flag = true;

                } else {
                    // Body of packet
                    fc_8st.push_back(rx_temp);
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
        }    
    }

    SC_CTOR( Controller ){
        SC_METHOD( run );
        sensitive << clk.pos() ;
        dont_initialize();
    }

};

#endif