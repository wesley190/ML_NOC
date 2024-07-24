//sc_buffer.cpp
#include <systemc.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
SC_MODULE(alexnet3){
    sc_port<sc_signal_out_if<string> > str;
    string str1 = "conv1_weight.txt";
    void readtxt2();
    
    SC_CTOR(alexnet3){
        SC_METHOD(readtxt2);
    }
};

