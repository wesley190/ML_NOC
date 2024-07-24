#ifndef _HPG_Router_proxy_h_H_
#define _HPG_Router_proxy_h_H_

#include "systemc.h"

SC_HDL_MODULE( Router) {
    sc_in<bool> rst;
    sc_in<bool> clk;
    sc_out<sc_lv<34> > out_flit0;
    sc_out<sc_lv<34> > out_flit1;
    sc_out<sc_lv<34> > out_flit2;
    sc_out<sc_lv<34> > out_flit3;
    sc_out<sc_lv<34> > out_flit4;
    sc_out<bool> out_valid0;
    sc_out<bool> out_valid1;
    sc_out<bool> out_valid2;
    sc_out<bool> out_valid3;
    sc_out<bool> out_valid4;
    sc_in<bool> out_ready0;
    sc_in<bool> out_ready1;
    sc_in<bool> out_ready2;
    sc_in<bool> out_ready3;
    sc_in<bool> out_ready4;
    sc_in<sc_lv<34> > in_flit0;
    sc_in<sc_lv<34> > in_flit1;
    sc_in<sc_lv<34> > in_flit2;
    sc_in<sc_lv<34> > in_flit3;
    sc_in<sc_lv<34> > in_flit4;
    sc_in<bool> in_valid0;
    sc_in<bool> in_valid1;
    sc_in<bool> in_valid2;
    sc_in<bool> in_valid3;
    sc_in<bool> in_valid4;
    sc_out<bool> in_ready0;
    sc_out<bool> in_ready1;
    sc_out<bool> in_ready2;
    sc_out<bool> in_ready3;
    sc_out<bool> in_ready4;

    std::string hpg_log_lib;
    std::string hpg_module_name;
    std::string hpg_hdl_src_path;

    std::string libraryName() { return hpg_log_lib; }

    std::string moduleName() { return hpg_module_name; }

    cwr_hdlLangType hdl_language_type() { return sc_hdl_module::cwr_verilog; }

    void getVerilogSourceFiles(std::vector<std::string>& verilog_files) {
        verilog_files.push_back(hpg_hdl_src_path + std::string("/RAID2/COURSE/mlchip/mlchip004/final/router.v"));
    }

    Router(sc_module_name name, int Router_ID = 0, const char* hdlSrcPath="") : 
        sc_hdl_module(name), hpg_log_lib("CWR_HDL_WORK"), hpg_module_name("Router"), hpg_hdl_src_path()
        , rst("rst"), clk("clk"), out_flit0("out_flit0"), out_flit1("out_flit1")
        , out_flit2("out_flit2"), out_flit3("out_flit3"), out_flit4("out_flit4")
        , out_valid0("out_valid0"), out_valid1("out_valid1"), out_valid2("out_valid2")
        , out_valid3("out_valid3"), out_valid4("out_valid4"), out_ready0("out_ready0")
        , out_ready1("out_ready1"), out_ready2("out_ready2"), out_ready3("out_ready3")
        , out_ready4("out_ready4"), in_flit0("in_flit0"), in_flit1("in_flit1"), in_flit2("in_flit2")
        , in_flit3("in_flit3"), in_flit4("in_flit4"), in_valid0("in_valid0"), in_valid1("in_valid1")
        , in_valid2("in_valid2"), in_valid3("in_valid3"), in_valid4("in_valid4")
        , in_ready0("in_ready0"), in_ready1("in_ready1"), in_ready2("in_ready2")
        , in_ready3("in_ready3"), in_ready4("in_ready4") {

        if (hdlSrcPath != 0 && strlen(hdlSrcPath) != 0) {
          hpg_hdl_src_path = std::string(hdlSrcPath) + "/";
        }

        ncwr_set_hdl_param("Router_ID", Router_ID);

    }
};

#endif
