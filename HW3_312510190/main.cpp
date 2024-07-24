#include "systemc.h"
#include "clockreset.h"
#include "core.h"
#include "pe.h"
#include "router.h"

using namespace std;


int sc_main(int argc, char* argv[])
{
    // =======================
    //   signals declaration
    // =======================
    sc_signal < bool > clk;
    sc_signal < bool > rst;
    // 16 core output
    sc_signal < sc_lv<34> > flit_tx[16];	
    sc_signal < bool > valid_tx[16];	        
    sc_signal < bool > ready_tx[16];	        
    // 16 router output, 5 output channel
    sc_signal < sc_lv<34> > out_flit[16][5];	
    sc_signal < bool > out_valid[16][5];	        
    sc_signal < bool > out_ready[16][5];	

    sc_signal < sc_lv<34> >  nonuse_flit ;	
    sc_signal < bool >      nonuse_valid;	        
    sc_signal < bool >      nonuse_ready[16];	


    // =======================
    //   modules declaration
    // =======================
    Clock m_clock("m_clock", 10);
    Reset m_reset("m_reset", 15);
    
    Core  m_Core0("m_Core0", 0 );
    Core  m_Core1("m_Core1", 1 );
    Core  m_Core2("m_Core2", 2 );
    Core  m_Core3("m_Core3", 3 );
    Core  m_Core4("m_Core4", 4 );
    Core  m_Core5("m_Core5", 5 );
    Core  m_Core6("m_Core6", 6 );
    Core  m_Core7("m_Core7", 7 );
    Core  m_Core8("m_Core8", 8 );
    Core  m_Core9("m_Core9", 9 );
    Core  m_Core10("m_Core10", 10);
    Core  m_Core11("m_Core11", 11);
    Core  m_Core12("m_Core12", 12);
    Core  m_Core13("m_Core13", 13);
    Core  m_Core14("m_Core14", 14);
    Core  m_Core15("m_Core15", 15);

    Router m_Router0("m_Router0",0 );
    Router m_Router1("m_Router1",1 );
    Router m_Router2("m_Router2",2 );
    Router m_Router3("m_Router3",3 );
    Router m_Router4("m_Router4",4 );
    Router m_Router5("m_Router5",5 );
    Router m_Router6("m_Router6",6 );
    Router m_Router7("m_Router7",7 );
    Router m_Router8("m_Router8",8 );
    Router m_Router9("m_Router9",9 );
    Router m_Router10("m_Router10",10);
    Router m_Router11("m_Router11",11);
    Router m_Router12("m_Router12",12);
    Router m_Router13("m_Router13",13);
    Router m_Router14("m_Router14",14);
    Router m_Router15("m_Router15",15);
    

    // =======================
    //   modules connection
    // =======================
    m_clock( clk );
    m_reset( rst );

    m_Core0( rst, clk, out_flit[0][0], out_valid[0][0], out_ready[0][0], flit_tx[0], valid_tx[0], ready_tx[0]);
    m_Core1( rst, clk, out_flit[1][0], out_valid[1][0], out_ready[1][0], flit_tx[1], valid_tx[1], ready_tx[1]);
    m_Core2( rst, clk, out_flit[2][0], out_valid[2][0], out_ready[2][0], flit_tx[2], valid_tx[2], ready_tx[2]);
    m_Core3( rst, clk, out_flit[3][0], out_valid[3][0], out_ready[3][0], flit_tx[3], valid_tx[3], ready_tx[3]);
    m_Core4( rst, clk, out_flit[4][0], out_valid[4][0], out_ready[4][0], flit_tx[4], valid_tx[4], ready_tx[4]);
    m_Core5( rst, clk, out_flit[5][0], out_valid[5][0], out_ready[5][0], flit_tx[5], valid_tx[5], ready_tx[5]);
    m_Core6( rst, clk, out_flit[6][0], out_valid[6][0], out_ready[6][0], flit_tx[6], valid_tx[6], ready_tx[6]);
    m_Core7( rst, clk, out_flit[7][0], out_valid[7][0], out_ready[7][0], flit_tx[7], valid_tx[7], ready_tx[7]);
    m_Core8( rst, clk, out_flit[8][0], out_valid[8][0], out_ready[8][0], flit_tx[8], valid_tx[8], ready_tx[8]);
    m_Core9( rst, clk, out_flit[9][0], out_valid[9][0], out_ready[9][0], flit_tx[9], valid_tx[9], ready_tx[9]);
    m_Core10( rst, clk, out_flit[10][0], out_valid[10][0], out_ready[10][0], flit_tx[10], valid_tx[10], ready_tx[10]);
    m_Core11( rst, clk, out_flit[11][0], out_valid[11][0], out_ready[11][0], flit_tx[11], valid_tx[11], ready_tx[11]);
    m_Core12( rst, clk, out_flit[12][0], out_valid[12][0], out_ready[12][0], flit_tx[12], valid_tx[12], ready_tx[12]);
    m_Core13( rst, clk, out_flit[13][0], out_valid[13][0], out_ready[13][0], flit_tx[13], valid_tx[13], ready_tx[13]);
    m_Core14( rst, clk, out_flit[14][0], out_valid[14][0], out_ready[14][0], flit_tx[14], valid_tx[14], ready_tx[14]);
    m_Core15( rst, clk, out_flit[15][0], out_valid[15][0], out_ready[15][0], flit_tx[15], valid_tx[15], ready_tx[15]);

    m_Router0.rst( rst );
    m_Router0.clk( clk );
    m_Router1.rst( rst );
    m_Router1.clk( clk );
    m_Router2.rst( rst );
    m_Router2.clk( clk );
    m_Router3.rst( rst );
    m_Router3.clk( clk );
    m_Router4.rst( rst );
    m_Router4.clk( clk );
    m_Router5.rst( rst );
    m_Router5.clk( clk );
    m_Router6.rst( rst );
    m_Router6.clk( clk );
    m_Router7.rst( rst );
    m_Router7.clk( clk );
    m_Router8.rst( rst );
    m_Router8.clk( clk );
    m_Router9.rst( rst );
    m_Router9.clk( clk );
    m_Router10.rst( rst );
    m_Router10.clk( clk );
    m_Router11.rst( rst );
    m_Router11.clk( clk );
    m_Router12.rst( rst );
    m_Router12.clk( clk );
    m_Router13.rst( rst );
    m_Router13.clk( clk );
    m_Router14.rst( rst );
    m_Router14.clk( clk );
    m_Router15.rst( rst );
    m_Router15.clk( clk );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router0.out_flit[i]( out_flit[0][i] );
        m_Router0.out_valid[i]( out_valid[0][i] );
        m_Router0.out_ready[i]( out_ready[0][i] );
    }
    // 0: local core
    m_Router0.in_flit[0](  flit_tx[0]  );
    m_Router0.in_valid[0]( valid_tx[0] );
    m_Router0.in_ready[0]( ready_tx[0] );
    // 1: East router
    m_Router0.in_flit[1](  out_flit[1][3] ); // from router 1, west router
    m_Router0.in_valid[1]( out_valid[1][3] );
    m_Router0.in_ready[1]( out_ready[1][3] );
    // 2: South router 
    m_Router0.in_flit[2](  out_flit[4][4] ); // from router 4, north router
    m_Router0.in_valid[2]( out_valid[4][4] );
    m_Router0.in_ready[2]( out_ready[4][4] );
    // 3: West router
    m_Router0.in_flit[3](   nonuse_flit );
    m_Router0.in_valid[3]( nonuse_valid );
    m_Router0.in_ready[3]( nonuse_ready[0] );
    // 4: North router
    m_Router0.in_flit[4](   nonuse_flit );
    m_Router0.in_valid[4]( nonuse_valid );
    m_Router0.in_ready[4]( nonuse_ready[1] );

    
    // output channel
    for(int i = 0; i < 5; i++){
        m_Router1.out_flit[i]( out_flit[1][i] );
        m_Router1.out_valid[i]( out_valid[1][i] );
        m_Router1.out_ready[i]( out_ready[1][i] );
    }
    // 0: local core
    m_Router1.in_flit[0](  flit_tx[1]  );
    m_Router1.in_valid[0]( valid_tx[1] );
    m_Router1.in_ready[0]( ready_tx[1] );
    // 1: East router
    m_Router1.in_flit[1](  out_flit[2][3] ); // from router 2, west router
    m_Router1.in_valid[1]( out_valid[2][3] );
    m_Router1.in_ready[1]( out_ready[2][3] );
    // 2: South router 
    m_Router1.in_flit[2](  out_flit[5][4] ); // from router 5, north router
    m_Router1.in_valid[2]( out_valid[5][4] );
    m_Router1.in_ready[2]( out_ready[5][4] );
    // 3: West router
    m_Router1.in_flit[3](  out_flit[0][1] ); // from router 0, east router
    m_Router1.in_valid[3]( out_valid[0][1] );
    m_Router1.in_ready[3]( out_ready[0][1] );
    // 4: North router
    m_Router1.in_flit[4](   nonuse_flit );
    m_Router1.in_valid[4]( nonuse_valid );
    m_Router1.in_ready[4]( nonuse_ready[2] );


    // output channel
    for(int i = 0; i < 5; i++){
        m_Router2.out_flit[i]( out_flit[2][i] );
        m_Router2.out_valid[i]( out_valid[2][i] );
        m_Router2.out_ready[i]( out_ready[2][i] );
    }
    // 0: local core
    m_Router2.in_flit[0](  flit_tx[2]  );
    m_Router2.in_valid[0]( valid_tx[2] );
    m_Router2.in_ready[0]( ready_tx[2] );
    // 1: East router
    m_Router2.in_flit[1](  out_flit[3][3] ); // from router 3, west router
    m_Router2.in_valid[1]( out_valid[3][3] );
    m_Router2.in_ready[1]( out_ready[3][3] );
    // 2: South router 
    m_Router2.in_flit[2](  out_flit[6][4] ); // from router 6, north router
    m_Router2.in_valid[2]( out_valid[6][4] );
    m_Router2.in_ready[2]( out_ready[6][4] );
    // 3: West router
    m_Router2.in_flit[3](  out_flit[1][1] ); // from router 1, east router
    m_Router2.in_valid[3]( out_valid[1][1] );
    m_Router2.in_ready[3]( out_ready[1][1] );
    // 4: North router
    m_Router2.in_flit[4](   nonuse_flit );
    m_Router2.in_valid[4]( nonuse_valid );
    m_Router2.in_ready[4]( nonuse_ready[3] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router3.out_flit[i]( out_flit[3][i] );
        m_Router3.out_valid[i]( out_valid[3][i] );
        m_Router3.out_ready[i]( out_ready[3][i] );
    }
    // 0: local core
    m_Router3.in_flit[0](  flit_tx[3]  );
    m_Router3.in_valid[0]( valid_tx[3] );
    m_Router3.in_ready[0]( ready_tx[3] );
    // 1: East router
    m_Router3.in_flit[1](   nonuse_flit ); 
    m_Router3.in_valid[1]( nonuse_valid );
    m_Router3.in_ready[1]( nonuse_ready[4] );
    // 2: South router 
    m_Router3.in_flit[2](  out_flit[7][4] ); // from router 7, north router
    m_Router3.in_valid[2]( out_valid[7][4] );
    m_Router3.in_ready[2]( out_ready[7][4] );
    // 3: West router
    m_Router3.in_flit[3](  out_flit[2][1] ); // from router 2, east router
    m_Router3.in_valid[3]( out_valid[2][1] );
    m_Router3.in_ready[3]( out_ready[2][1] );
    // 4: North router
    m_Router3.in_flit[4](   nonuse_flit );
    m_Router3.in_valid[4]( nonuse_valid );
    m_Router3.in_ready[4]( nonuse_ready[5] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router4.out_flit[i]( out_flit[4][i] );
        m_Router4.out_valid[i]( out_valid[4][i] );
        m_Router4.out_ready[i]( out_ready[4][i] );
    }
    // 0: local core
    m_Router4.in_flit[0](  flit_tx[4]  );
    m_Router4.in_valid[0]( valid_tx[4] );
    m_Router4.in_ready[0]( ready_tx[4] );
    // 1: East router
    m_Router4.in_flit[1](  out_flit[5][3] ); // from router 5, west router
    m_Router4.in_valid[1]( out_valid[5][3] );
    m_Router4.in_ready[1]( out_ready[5][3] );
    // 2: South router 
    m_Router4.in_flit[2](  out_flit[8][4] ); // from router 8, north router
    m_Router4.in_valid[2]( out_valid[8][4] );
    m_Router4.in_ready[2]( out_ready[8][4] );
    // 3: West router
    m_Router4.in_flit[3](   nonuse_flit ); 
    m_Router4.in_valid[3]( nonuse_valid );
    m_Router4.in_ready[3]( nonuse_ready[6] );
    // 4: North router
    m_Router4.in_flit[4](  out_flit[0][2] ); // from router 0, south router
    m_Router4.in_valid[4]( out_valid[0][2] );
    m_Router4.in_ready[4]( out_ready[0][2] );


    // output channel
    for(int i = 0; i < 5; i++){
        m_Router5.out_flit[i]( out_flit[5][i] );
        m_Router5.out_valid[i]( out_valid[5][i] );
        m_Router5.out_ready[i]( out_ready[5][i] );
    }
    // 0: local core
    m_Router5.in_flit[0](  flit_tx[5]  );
    m_Router5.in_valid[0]( valid_tx[5] );
    m_Router5.in_ready[0]( ready_tx[5] );
    // 1: East router
    m_Router5.in_flit[1](  out_flit[6][3] ); // from router 6, west router
    m_Router5.in_valid[1]( out_valid[6][3] );
    m_Router5.in_ready[1]( out_ready[6][3] );
    // 2: South router 
    m_Router5.in_flit[2](  out_flit[9][4] ); // from router 9, north router
    m_Router5.in_valid[2]( out_valid[9][4] );
    m_Router5.in_ready[2]( out_ready[9][4] );
    // 3: West router
    m_Router5.in_flit[3](  out_flit[4][1] ); // from router 4, east router
    m_Router5.in_valid[3]( out_valid[4][1] );
    m_Router5.in_ready[3]( out_ready[4][1] );
    // 4: North router
    m_Router5.in_flit[4](  out_flit[1][2] ); // from router 1, south router
    m_Router5.in_valid[4]( out_valid[1][2] );
    m_Router5.in_ready[4]( out_ready[1][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router6.out_flit[i]( out_flit[6][i] );
        m_Router6.out_valid[i]( out_valid[6][i] );
        m_Router6.out_ready[i]( out_ready[6][i] );
    }
    // 0: local core
    m_Router6.in_flit[0](  flit_tx[6]  );
    m_Router6.in_valid[0]( valid_tx[6] );
    m_Router6.in_ready[0]( ready_tx[6] );
    // 1: East router
    m_Router6.in_flit[1](  out_flit[7][3] ); // from router 7, west router
    m_Router6.in_valid[1]( out_valid[7][3] );
    m_Router6.in_ready[1]( out_ready[7][3] );
    // 2: South router 
    m_Router6.in_flit[2](  out_flit[10][4] ); // from router 10, north router
    m_Router6.in_valid[2]( out_valid[10][4] );
    m_Router6.in_ready[2]( out_ready[10][4] );
    // 3: West router
    m_Router6.in_flit[3](  out_flit[5][1] ); // from router 5, east router
    m_Router6.in_valid[3]( out_valid[5][1] );
    m_Router6.in_ready[3]( out_ready[5][1] );
    // 4: North router
    m_Router6.in_flit[4](  out_flit[2][2] ); // from router 2, south router
    m_Router6.in_valid[4]( out_valid[2][2] );
    m_Router6.in_ready[4]( out_ready[2][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router7.out_flit[i]( out_flit[7][i] );
        m_Router7.out_valid[i]( out_valid[7][i] );
        m_Router7.out_ready[i]( out_ready[7][i] );
    }
    // 0: local core
    m_Router7.in_flit[0](  flit_tx[7]  );
    m_Router7.in_valid[0]( valid_tx[7] );
    m_Router7.in_ready[0]( ready_tx[7] );
    // 1: East router
    m_Router7.in_flit[1](   nonuse_flit ); 
    m_Router7.in_valid[1]( nonuse_valid );
    m_Router7.in_ready[1]( nonuse_ready[7] );
    // 2: South router 
    m_Router7.in_flit[2](  out_flit[11][4] ); // from router 11, north router
    m_Router7.in_valid[2]( out_valid[11][4] );
    m_Router7.in_ready[2]( out_ready[11][4] );
    // 3: West router
    m_Router7.in_flit[3](  out_flit[6][1] ); // from router 6, east router
    m_Router7.in_valid[3]( out_valid[6][1] );
    m_Router7.in_ready[3]( out_ready[6][1] );
    // 4: North router
    m_Router7.in_flit[4](  out_flit[3][2] ); // from router 3, south router
    m_Router7.in_valid[4]( out_valid[3][2] );
    m_Router7.in_ready[4]( out_ready[3][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router8.out_flit[i]( out_flit[8][i] );
        m_Router8.out_valid[i]( out_valid[8][i] );
        m_Router8.out_ready[i]( out_ready[8][i] );
    }
    // 0: local core
    m_Router8.in_flit[0](  flit_tx[8]  );
    m_Router8.in_valid[0]( valid_tx[8] );
    m_Router8.in_ready[0]( ready_tx[8] );
    // 1: East router
    m_Router8.in_flit[1](  out_flit[9][3] ); // from router 9, west router
    m_Router8.in_valid[1]( out_valid[9][3] );
    m_Router8.in_ready[1]( out_ready[9][3] );
    // 2: South router 
    m_Router8.in_flit[2](  out_flit[12][4] ); // from router 12, north router
    m_Router8.in_valid[2]( out_valid[12][4] );
    m_Router8.in_ready[2]( out_ready[12][4] );
    // 3: West router
    m_Router8.in_flit[3](   nonuse_flit ); 
    m_Router8.in_valid[3]( nonuse_valid );
    m_Router8.in_ready[3]( nonuse_ready[8] );
    // 4: North router
    m_Router8.in_flit[4](  out_flit[4][2] ); // from router 4, south router
    m_Router8.in_valid[4]( out_valid[4][2] );
    m_Router8.in_ready[4]( out_ready[4][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router9.out_flit[i]( out_flit[9][i] );
        m_Router9.out_valid[i]( out_valid[9][i] );
        m_Router9.out_ready[i]( out_ready[9][i] );
    }
    // 0: local core
    m_Router9.in_flit[0](  flit_tx[9]  );
    m_Router9.in_valid[0]( valid_tx[9] );
    m_Router9.in_ready[0]( ready_tx[9] );
    // 1: East router
    m_Router9.in_flit[1](  out_flit[10][3] ); // from router 10, west router
    m_Router9.in_valid[1]( out_valid[10][3] );
    m_Router9.in_ready[1]( out_ready[10][3] );
    // 2: South router 
    m_Router9.in_flit[2](  out_flit[13][4] ); // from router 13, north router
    m_Router9.in_valid[2]( out_valid[13][4] );
    m_Router9.in_ready[2]( out_ready[13][4] );
    // 3: West router
    m_Router9.in_flit[3](  out_flit[8][1] ); // from router 8, east router
    m_Router9.in_valid[3]( out_valid[8][1] );
    m_Router9.in_ready[3]( out_ready[8][1] );
    // 4: North router
    m_Router9.in_flit[4](  out_flit[5][2] ); // from router 5, south router
    m_Router9.in_valid[4]( out_valid[5][2] );
    m_Router9.in_ready[4]( out_ready[5][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router10.out_flit[i]( out_flit[10][i] );
        m_Router10.out_valid[i]( out_valid[10][i] );
        m_Router10.out_ready[i]( out_ready[10][i] );
    }
    // 0: local core
    m_Router10.in_flit[0](  flit_tx[10]  );
    m_Router10.in_valid[0]( valid_tx[10] );
    m_Router10.in_ready[0]( ready_tx[10] );
    // 1: East router
    m_Router10.in_flit[1](  out_flit[11][3] ); // from router 11, west router
    m_Router10.in_valid[1]( out_valid[11][3] );
    m_Router10.in_ready[1]( out_ready[11][3] );
    // 2: South router 
    m_Router10.in_flit[2](  out_flit[14][4] ); // from router 14, north router
    m_Router10.in_valid[2]( out_valid[14][4] );
    m_Router10.in_ready[2]( out_ready[14][4] );
    // 3: West router
    m_Router10.in_flit[3](  out_flit[9][1] ); // from router 9, east router
    m_Router10.in_valid[3]( out_valid[9][1] );
    m_Router10.in_ready[3]( out_ready[9][1] );
    // 4: North router
    m_Router10.in_flit[4](  out_flit[6][2] ); // from router 6, south router
    m_Router10.in_valid[4]( out_valid[6][2] );
    m_Router10.in_ready[4]( out_ready[6][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router11.out_flit[i]( out_flit[11][i] );
        m_Router11.out_valid[i]( out_valid[11][i] );
        m_Router11.out_ready[i]( out_ready[11][i] );
    }
    // 0: local core
    m_Router11.in_flit[0](  flit_tx[11]  );
    m_Router11.in_valid[0]( valid_tx[11] );
    m_Router11.in_ready[0]( ready_tx[11] );
    // 1: East router
    m_Router11.in_flit[1](   nonuse_flit ); 
    m_Router11.in_valid[1]( nonuse_valid );
    m_Router11.in_ready[1]( nonuse_ready[9] );
    // 2: South router 
    m_Router11.in_flit[2](  out_flit[15][4] ); // from router 15, north router
    m_Router11.in_valid[2]( out_valid[15][4] );
    m_Router11.in_ready[2]( out_ready[15][4] );
    // 3: West router
    m_Router11.in_flit[3](  out_flit[10][1] ); // from router 10, east router
    m_Router11.in_valid[3]( out_valid[10][1] );
    m_Router11.in_ready[3]( out_ready[10][1] );
    // 4: North router
    m_Router11.in_flit[4](  out_flit[7][2] ); // from router 7, south router
    m_Router11.in_valid[4]( out_valid[7][2] );
    m_Router11.in_ready[4]( out_ready[7][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router12.out_flit[i]( out_flit[12][i] );
        m_Router12.out_valid[i]( out_valid[12][i] );
        m_Router12.out_ready[i]( out_ready[12][i] );
    }
    // 0: local core
    m_Router12.in_flit[0](  flit_tx[12]  );
    m_Router12.in_valid[0]( valid_tx[12] );
    m_Router12.in_ready[0]( ready_tx[12] );
    // 1: East router
    m_Router12.in_flit[1](  out_flit[13][3] ); // from router 13, west router
    m_Router12.in_valid[1]( out_valid[13][3] );
    m_Router12.in_ready[1]( out_ready[13][3] );
    // 2: South router 
    m_Router12.in_flit[2](   nonuse_flit ); 
    m_Router12.in_valid[2]( nonuse_valid );
    m_Router12.in_ready[2]( nonuse_ready[10] );
    // 3: West router
    m_Router12.in_flit[3](   nonuse_flit ); 
    m_Router12.in_valid[3]( nonuse_valid );
    m_Router12.in_ready[3]( nonuse_ready[11] );
    // 4: North router
    m_Router12.in_flit[4](  out_flit[8][2] ); // from router 8, south router
    m_Router12.in_valid[4]( out_valid[8][2] );
    m_Router12.in_ready[4]( out_ready[8][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router13.out_flit[i]( out_flit[13][i] );
        m_Router13.out_valid[i]( out_valid[13][i] );
        m_Router13.out_ready[i]( out_ready[13][i] );
    }
    // 0: local core
    m_Router13.in_flit[0](  flit_tx[13]  );
    m_Router13.in_valid[0]( valid_tx[13] );
    m_Router13.in_ready[0]( ready_tx[13] );
    // 1: East router
    m_Router13.in_flit[1](  out_flit[14][3] ); // from router 14, west router
    m_Router13.in_valid[1]( out_valid[14][3] );
    m_Router13.in_ready[1]( out_ready[14][3] );
    // 2: South router 
    m_Router13.in_flit[2](   nonuse_flit );
    m_Router13.in_valid[2]( nonuse_valid );
    m_Router13.in_ready[2]( nonuse_ready[12] );
    // 3: West router
    m_Router13.in_flit[3](  out_flit[12][1] ); // from router 12, east router
    m_Router13.in_valid[3]( out_valid[12][1] );
    m_Router13.in_ready[3]( out_ready[12][1] );
    // 4: North router
    m_Router13.in_flit[4](  out_flit[9][2] ); // from router 9, south router
    m_Router13.in_valid[4]( out_valid[9][2] );
    m_Router13.in_ready[4]( out_ready[9][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router14.out_flit[i]( out_flit[14][i] );
        m_Router14.out_valid[i]( out_valid[14][i] );
        m_Router14.out_ready[i]( out_ready[14][i] );
    }
    // 0: local core
    m_Router14.in_flit[0](  flit_tx[14]  );
    m_Router14.in_valid[0]( valid_tx[14] );
    m_Router14.in_ready[0]( ready_tx[14] );
    // 1: East router
    m_Router14.in_flit[1](  out_flit[15][3] ); // from router 15, west router
    m_Router14.in_valid[1]( out_valid[15][3] );
    m_Router14.in_ready[1]( out_ready[15][3] );
    // 2: South router 
    m_Router14.in_flit[2](   nonuse_flit ); 
    m_Router14.in_valid[2]( nonuse_valid );
    m_Router14.in_ready[2]( nonuse_ready[13] );
    // 3: West router
    m_Router14.in_flit[3](  out_flit[13][1] ); // from router 13, east router
    m_Router14.in_valid[3]( out_valid[13][1] );
    m_Router14.in_ready[3]( out_ready[13][1] );
    // 4: North router
    m_Router14.in_flit[4](  out_flit[10][2] ); // from router 10, south router
    m_Router14.in_valid[4]( out_valid[10][2] );
    m_Router14.in_ready[4]( out_ready[10][2] );

    // output channel
    for(int i = 0; i < 5; i++){
        m_Router15.out_flit[i]( out_flit[15][i] );
        m_Router15.out_valid[i]( out_valid[15][i] );
        m_Router15.out_ready[i]( out_ready[15][i] );
    }
    // 0: local core
    m_Router15.in_flit[0](  flit_tx[15]  );
    m_Router15.in_valid[0]( valid_tx[15] );
    m_Router15.in_ready[0]( ready_tx[15] );
    // 1: East router
    m_Router15.in_flit[1](   nonuse_flit ); 
    m_Router15.in_valid[1]( nonuse_valid );
    m_Router15.in_ready[1]( nonuse_ready[14] );
    // 2: South router 
    m_Router15.in_flit[2](   nonuse_flit ); 
    m_Router15.in_valid[2]( nonuse_valid );
    m_Router15.in_ready[2]( nonuse_ready[15] );
    // 3: West router
    m_Router15.in_flit[3](  out_flit[14][1] ); // from router 14, east router
    m_Router15.in_valid[3]( out_valid[14][1] );
    m_Router15.in_ready[3]( out_ready[14][1] );
    // 4: North router
    m_Router15.in_flit[4](  out_flit[11][2] ); // from router 11, south router
    m_Router15.in_valid[4]( out_valid[11][2] );
    m_Router15.in_ready[4]( out_ready[11][2] );


    sc_start(10000, SC_NS);
    return 0;
}