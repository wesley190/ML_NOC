//sc_fifo.cpp
#include <systemc.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
SC_MODULE(alexnet4){
    sc_port<sc_fifo_out_if<string> > str;
    string str1 = "conv1_bias.txt";
    void readtxt3();

    SC_CTOR(alexnet4){
        SC_METHOD(readtxt3);
    }
};