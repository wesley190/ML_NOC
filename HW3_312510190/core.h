#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"

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

    PE pe;                 // Processing Element
    
    Packet *check_packet;       // Packet for receiving data
    Packet *packet;      // Packet for transmitting data

    bool no_packet_flag;      // Indicates if there are no packets to packet
    bool tx_busy;          // Indicates if the transmitter is busy
    int tx_cnt;  // Transmission counter
    vector<float> data_in;    // Input data
    vector<float> data_out;   // Output data

    int core_id;           // Core ID
    int src_id;            // Source ID
    int dest_id;           // Destination ID
    bool input_finish;

    SC_HAS_PROCESS(Core);

    // Main process
    void run() {
        if (rst.read() == 1) {
            initialize();
        } else {
            handle_receive();
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
            cout<<"core check_packet flit_rx_t"<<endl;
            sc_lv<34> flit_rx_t = flit_rx.read();
            cout<<"flit_rx_t:"<<flit_rx_t<<endl;
            //std::string flit_type = flit_rx_t.range(33, 32).to_string();

            if (flit_rx_t.range(33, 32) == "10") {
                // Begin of packet
                check_packet = new Packet;
                check_packet->source_id = int((sc_uint<4>)(sc_lv<4>)flit_rx_t.range(31, 28));
                check_packet->dest_id = int((sc_uint<4>)(sc_lv<4>)flit_rx_t.range(27, 24));
            } else {
                sc_dt::scfx_ieee_float rx_temp;
                rx_temp = convert_to_float(flit_rx_t);

                if (flit_rx_t.range(33, 32) == "01") {
                    // End of packet
                    data_in.push_back(rx_temp);
                    input_finish = true;

                } else {
                    // Body of packet
                    data_in.push_back(rx_temp);
                }
            }
        }
        if(input_finish){
            cout<<"core check_packet finish"<<endl;
            check_packet->datas = data_in;
            pe.check_packet(check_packet);
            delete check_packet;
            input_finish = false;
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

    // Handle transmitting data
    void handle_transmit() {
        if (!no_packet_flag) {
            if (ready_tx.read() == 1) {
                update_tx_status();
            }
            if (!tx_busy) {
                packet = pe.get_packet();
            }
            if (!packet) {
                no_packet_flag = true;
                valid_tx.write(false);
            }
            else{
                send_packet();
            }
            
        }
    }

    // Update transmission status
    void update_tx_status() {
        if (tx_cnt == data_out.size()) {
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
        
        if (tx_cnt == 0) {
            send_header();
        } else {
            send_body_or_tail();
        }
    }

    // Send the packet header
    void send_header() {
        sc_lv<34> flit_out;
        src_id = packet->source_id;
        dest_id = packet->dest_id;
        data_out = packet->datas;
        cout<<"source:"<<src_id<<"dest:"<<dest_id<<endl;
        
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
        sc_dt::scfx_ieee_float tx_temp(data_out[tx_cnt - 1]);
        
        sc_lv<32> flit_out_sc_lv = convert_to_sc_lv(tx_temp);
        flit_out.range(31, 0) = flit_out_sc_lv;
        if (tx_cnt == data_out.size()) {
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
        pe.init(core_id);
        tx_busy = false;
        no_packet_flag = false;
        tx_cnt = 0;

        SC_METHOD(run);
        sensitive << clk.pos();
        dont_initialize();
    }
};

#endif
