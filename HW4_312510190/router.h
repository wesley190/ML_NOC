#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"

SC_MODULE( Router ) {
    sc_in  < bool >  rst;
    sc_in  < bool >  clk;

    // destination port: 0: local, 1: East, 2: South, 3: West, 4: North 
    sc_out < sc_lv<34> >  out_flit[5];
    sc_out < bool >  out_valid[5];
    sc_in  < bool >  out_ready[5];

    sc_in  < sc_lv<34> >  in_flit[5];
    sc_in  < bool >  in_valid[5];
    sc_out < bool >  in_ready[5];

    // input buffer
    sc_lv<34> flit_buffer[5][8];
    
    // input buffer status
    bool buffer_empty[5];
    bool buffer_full[5];

    // output channel status
    sc_uint<3> channel_select[5];
    bool out_busy[5];
    bool out_valid_tmp[5];
    sc_uint<3> addr_tx;
    sc_uint<3> addr_rx;
    sc_uint<3> port;

    // read and write address for each input buffer
    sc_uint<4>  r_addr_buf[5];
    sc_uint<4>  w_addr_buf[5];

    

    int Router_ID;
    SC_HAS_PROCESS(Router);
    void run(){
        if(rst.read() == 1){
            initialize_router();
        }
        else{
            handle_rx();
            habdle_tx();
            xy_routing();

        }
    }

    void initialize_router(){
        for(int i = 0; i < 5; i++){
            out_valid[i].write(0);
            in_ready[i].write(0);
        }
    }

    void handle_rx(){
        for(int i = 0; i < 5; i++){
            if( in_valid[i].read() == 1){
                if(in_ready[i] == 1 ){
                    //cout<<"the time: "<<sc_time_stamp()<<", Router"<<Router_ID<<" receive"<<in_flit[i]<<endl;
                    addr_rx = w_addr_buf[i].range(2,0);
                    flit_buffer[i][addr_rx] = in_flit[i];
                    w_addr_buf[i] += 1;
                }
            }
        }
    }

    void habdle_tx(){
        for(int i = 0; i < 5; i++){
            if(out_valid[i] == 1){
                if(out_ready[i].read() == 1){ // see whether the output channel accept flit
                    // cout<<"addr_tx: "<<r_addr_buf[channel_select[i]].range(2,0)<<endl;
                     addr_tx = r_addr_buf[channel_select[i]].range(2,0);
                     //cout<<"port: "<<channel_select[i]<<endl;
                     port = channel_select[i];
                    auto is_last_flit = flit_buffer[port][addr_tx][32];

                    //cout<<"the time: "<<sc_time_stamp()<<", Router"<<Router_ID<<" transmit"<<in_flit[i]<<endl;
                    //cout << "Router" << Router_ID << " Out" << i << " to " << channel_select[i] <<": " << sc_time_stamp() << flit_buffer[port][addr_tx] << "\n";
                    out_busy[i] = (is_last_flit == 1)? 0 : 1;
                    
                    r_addr_buf[channel_select[i]] += 1;
                }
            }
        }
    }

    void xy_routing(){
        // check input buffer status
        for(int i = 0; i < 5; i++){
            bool is_empty = (r_addr_buf[i] == w_addr_buf[i]);
            bool is_full = (r_addr_buf[i].range(2,0) == w_addr_buf[i].range(2,0) && r_addr_buf[i][3] != w_addr_buf[i][3]);
            //check whether the input buffer is empty or full
            buffer_empty[i] = (is_empty == 1)? 1 : 0;
            buffer_full[i] = (is_full == 1)? 1 : 0;
            if(buffer_full[i] == 1){
                
                //cout <<"the time: "<<sc_time_stamp()<< ", Router" << Router_ID << " input channel " << i << " is full.\n";
                in_ready[i].write(0);
            }
            else{
                in_ready[i].write(1);
            }
            
        }
        
        // Output selection for the local output channel
        if (out_busy[0] == 0) {
            bool found_flit = false;
            // Iterate over all input buffers
            for (int i = 0; i < 5; i++) {
                // output selection 
                if (buffer_empty[i] == 0) {
                    sc_lv<34> flit = flit_buffer[i][r_addr_buf[i].range(2,0)];
                    int flit_router_id = int((sc_uint<4>)(sc_lv<4>)flit.range(27,24));
                    bool is_head_flit = flit[33] == 1;

                    if (flit_router_id == Router_ID && is_head_flit) {
                        channel_select[0] = i;
                        out_valid_tmp[0] = 1;
                        found_flit = true;
                        break;// found the flit
                    }
                }
            }
            // If no valid flit found, set the output to invalid
            if (!found_flit) {
                channel_select[0] = 5;
                out_valid_tmp[0] = 0;
            }
        } else {
            // Output channel is busy, retain current buffer selection
            channel_select[0] = channel_select[0];
            out_valid_tmp[0] = 1;
        }

        //1: East output channel
        // xy routing
        if (out_busy[1] == 0) {
            bool found_flit = false;
            // Iterate over all input buffers
            for (int i = 0; i < 5; i++) {
                // output selection 
                if (buffer_empty[i] == 0) {
                    sc_lv<34> flit = flit_buffer[i][r_addr_buf[i].range(2,0)];
                    int xy_route_direct = int((sc_uint<4>)(sc_lv<4>)flit.range(27,24)) % 4;
                    bool is_head_flit = flit[33] == 1;
                    if (xy_route_direct > Router_ID%4 && is_head_flit) {
                        channel_select[1] = i;
                        out_valid_tmp[1] = 1;
                        found_flit = true;
                        break;// found the flit
                    }
                }
            }
            // If no valid flit found, set the output to invalid
            if (!found_flit) {
                channel_select[1] = 5;
                out_valid_tmp[1] = 0;
            }
        } else {
            // Output channel is busy, retain current buffer selection
            channel_select[1] = channel_select[1];
            out_valid_tmp[1] = 1;
        }

        //3: West output channel
        if (out_busy[3] == 0) {
            bool found_flit = false;
            // Iterate over all input buffers
            for (int i = 0; i < 5; i++) {
                // output selection 
                if (buffer_empty[i] == 0) {
                    sc_lv<34> flit = flit_buffer[i][r_addr_buf[i].range(2,0)];
                    int xy_route_direct = int((sc_uint<4>)(sc_lv<4>)flit.range(27,24)) % 4;
                    bool is_head_flit = flit[33] == 1;
                    if (xy_route_direct < Router_ID%4 && is_head_flit) {
                        channel_select[3] = i;
                        out_valid_tmp[3] = 1;
                        found_flit = true;
                        break;// found the flit
                    }
                }
            }
            // If no valid flit found, set the output to invalid
            if (!found_flit) {
                channel_select[3] = 5;
                out_valid_tmp[3] = 0;
            }
        } else {
            // Output channel is busy, retain current buffer selection
            channel_select[3] = channel_select[3];
            out_valid_tmp[3] = 1;
        }

        //2: South output channel
        if (out_busy[2] == 0) {
            bool found_flit = false;
            // Iterate over all input buffers
            for (int i = 0; i < 5; i++) {
                // output selection 
                if (buffer_empty[i] == 0) {
                    sc_lv<34> flit = flit_buffer[i][r_addr_buf[i].range(2,0)];
                    int xy_route_direct = int((sc_uint<4>)(sc_lv<4>)flit.range(27,24)) / 4;
                    bool is_head_flit = flit[33] == 1;
                    bool select_correct = (channel_select[1] != i && channel_select[3] != i);
                    if (xy_route_direct > Router_ID/4 && is_head_flit && select_correct) {
                        channel_select[2] = i;
                        out_valid_tmp[2] = 1;
                        found_flit = true;
                        break;// found the flit
                    }
                }
            }
            // If no valid flit found, set the output to invalid
            if (!found_flit) {
                channel_select[2] = 5;
                out_valid_tmp[2] = 0;
            }
        } else {
            // Output channel is busy, retain current buffer selection
            channel_select[2] = channel_select[2];
            out_valid_tmp[2] = 1;
        }
        
        //4: North output channel
        if (out_busy[4] == 0) {
            bool found_flit = false;
            // Iterate over all input buffers
            for (int i = 0; i < 5; i++) {
                // output selection 
                if (buffer_empty[i] == 0) {
                    sc_lv<34> flit = flit_buffer[i][r_addr_buf[i].range(2,0)];
                    int xy_route_direct = int((sc_uint<4>)(sc_lv<4>)flit.range(27,24)) / 4;
                    bool is_head_flit = flit[33] == 1;
                    bool select_correct = (channel_select[1] != i && channel_select[3] != i);
                    if (xy_route_direct < Router_ID/4 && is_head_flit && select_correct) {
                        channel_select[4] = i;
                        out_valid_tmp[4] = 1;
                        found_flit = true;
                        break;// found the flit
                    }
                }
            }
            // If no valid flit found, set the output to invalid
            if (!found_flit) {
                channel_select[4] = 5;
                out_valid_tmp[4] = 0;
            }
        } else {
            // Output channel is busy, retain current buffer selection
            channel_select[4] = channel_select[4];
            out_valid_tmp[4] = 1;
        }

        for(int i = 0; i < 5; i++){
            // out_valid
            out_valid[i].write(out_valid_tmp[i]);
            // output data
            if(out_valid_tmp[i] == 1){
                 addr_tx = r_addr_buf[channel_select[i]].range(2,0);
                 port = channel_select[i];
                    
                out_flit[i].write(flit_buffer[port][addr_tx]);
                //cout<<"Router"<<Router_ID<<" Out"<<i<<": "<<sc_time_stamp()<<flit_buffer[port][addr_tx]<<endl;
            }
            else{
                out_flit[i].write(0);
            }
        }
    }
    

    Router (sc_module_name name, int _Router_ID): sc_module(name), Router_ID(_Router_ID){
        for(int i = 0; i < 5; i++){
            r_addr_buf[i] = 0;
            w_addr_buf[i] = 0;
            out_busy[i] = 0;
        }

		SC_METHOD(run);
        sensitive << clk.pos();
        dont_initialize();
	}
};

#endif