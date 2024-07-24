`timescale 1ns/10ps
module Router #(parameter Router_ID = 0)(
    rst,
    clk,
    out_flit0,
    out_flit1,
    out_flit2,
    out_flit3,
    out_flit4,
    out_valid0,
    out_valid1,
    out_valid2,
    out_valid3,
    out_valid4,
    out_ready0,
    out_ready1,
    out_ready2,
    out_ready3,
    out_ready4,
    in_flit0,
    in_flit1,
    in_flit2,
    in_flit3,
    in_flit4,
    in_valid0,
    in_valid1,
    in_valid2,
    in_valid3,
    in_valid4,
    in_ready0,
    in_ready1,
    in_ready2,
    in_ready3,
    in_ready4
);
    input rst;
    input clk;
    input [33:0] in_flit0;
    input [33:0] in_flit1;
    input [33:0] in_flit2;
    input [33:0] in_flit3;
    input [33:0] in_flit4;
    input in_valid0;
    input in_valid1;
    input in_valid2;
    input in_valid3;
    input in_valid4;
    output reg in_ready0;
    output reg in_ready1;
    output reg in_ready2;
    output reg in_ready3;
    output reg in_ready4;
    output reg [33:0] out_flit0;
    output reg [33:0] out_flit1;
    output reg [33:0] out_flit2;
    output reg [33:0] out_flit3;
    output reg [33:0] out_flit4;
    output reg out_valid0;
    output reg out_valid1;
    output reg out_valid2;
    output reg out_valid3;
    output reg out_valid4;
    input out_ready0;
    input out_ready1;
    input out_ready2;
    input out_ready3;
    input out_ready4;
    

    // input buffer
    reg [33:0] flit_buffer[0:4][0:7];
    reg [3:0]  r_addr_buf[0:4];
    reg [3:0]  w_addr_buf[0:4];
    reg buffer_empty[0:4];
    reg buffer_full[0:4];
    reg [2:0] channel_select[0:4];
    reg [2:0] channel_select_tmp[0:4];
    reg out_busy[0:4];

    integer i, j;
    always @(posedge clk or posedge rst ) begin
        if (rst) begin
            for (i = 0; i < 5; i = i + 1) begin
                for(j = 0; j < 8; j = j + 1) begin
                    flit_buffer[i][j] <= 0;
                end
                w_addr_buf[i] <= 0;
            end
        end
        else begin

            if(in_ready0 && in_valid0) begin
                flit_buffer[0][w_addr_buf[0][2:0]] <= in_flit0;
                w_addr_buf[0] <= w_addr_buf[0] + 1;
            end
            if(in_ready1 && in_valid1) begin
                flit_buffer[1][w_addr_buf[1][2:0]] <= in_flit1;
                w_addr_buf[1] <= w_addr_buf[1] + 1;
            end
            if(in_ready2 && in_valid2) begin
                flit_buffer[2][w_addr_buf[2][2:0]] <= in_flit2;
                w_addr_buf[2] <= w_addr_buf[2] + 1;
            end
            if(in_ready3 && in_valid3) begin
                flit_buffer[3][w_addr_buf[3][2:0]] <= in_flit3;
                w_addr_buf[3] <= w_addr_buf[3] + 1;
            end
            if(in_ready4 && in_valid4) begin
                flit_buffer[4][w_addr_buf[4][2:0]] <= in_flit4;
                //$display("*  in_flit4 = %b   *",in_flit4);
                w_addr_buf[4] <= w_addr_buf[4] + 1;
            end
            
        end
    end
    
    
    // buffer empty / full
    always @(*) begin
        for(i=0; i<5; i=i+1) begin
            if(r_addr_buf[i][2:0] == w_addr_buf[i][2:0] && r_addr_buf[i][3] == w_addr_buf[i][3]) begin
                buffer_empty[i] = 1;
            end
            else begin
                buffer_empty[i] = 0;
            end
        end
    end
    always @(*) begin
        for(i=0; i<5; i=i+1) begin
            if(r_addr_buf[i][2:0] == w_addr_buf[i][2:0] && r_addr_buf[i][3] != w_addr_buf[i][3]) begin
                buffer_full[i] = 1;
            end
            else begin
                buffer_full[i] = 0;
            end
        end
    end
    // input ready
    always @(*) begin
        
        if(buffer_full[0]) begin
            in_ready0 = 0;
        end else begin
            in_ready0 = 1;
        end
        if(buffer_full[1]) begin
            in_ready1 = 0;
        end else begin
            in_ready1 = 1;
        end
        if(buffer_full[2]) begin
            in_ready2 = 0;
        end else begin
            in_ready2 = 1;
        end
        if(buffer_full[3]) begin
            in_ready3 = 0;
        end else begin
            in_ready3 = 1;
        end
        if(buffer_full[4]) begin
            in_ready4 = 0;
        end else begin
            in_ready4 = 1;
        end
        
    end
    
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            for (i = 0; i < 5; i = i + 1) begin
                r_addr_buf[i] <= 0;
                out_busy[i] <= 0;
            end
        end
        else begin
            if(out_valid0 && out_ready0) begin
                if (flit_buffer[channel_select[0]][r_addr_buf[channel_select[0]][2:0]][32]) begin
                    out_busy[0] <= 0;
                end else begin
                    out_busy[0] <= out_valid0;
                end
                r_addr_buf[channel_select[0]] <= r_addr_buf[channel_select[0]] + 1;
            end
            if(out_valid1 && out_ready1) begin
                if (flit_buffer[channel_select[1]][r_addr_buf[channel_select[1]][2:0]][32]) begin
                    out_busy[1] <= 0;
                end else begin
                    out_busy[1] <= out_valid1;
                end
                r_addr_buf[channel_select[1]] <= r_addr_buf[channel_select[1]] + 1;
            end
            if(out_valid2 && out_ready2) begin
                if (flit_buffer[channel_select[2]][r_addr_buf[channel_select[2]][2:0]][32]) begin
                    out_busy[2] <= 0;
                end else begin
                    out_busy[2] <= out_valid2;
                end
                r_addr_buf[channel_select[2]] <= r_addr_buf[channel_select[2]] + 1;
            end
            if(out_valid3 && out_ready3) begin
                if (flit_buffer[channel_select[3]][r_addr_buf[channel_select[3]][2:0]][32]) begin
                    out_busy[3] <= 0;
                end else begin
                    out_busy[3] <= out_valid3;
                end
                r_addr_buf[channel_select[3]] <= r_addr_buf[channel_select[3]] + 1;
            end
            if(out_valid4 && out_ready4) begin
                if (flit_buffer[channel_select[4]][r_addr_buf[channel_select[4]][2:0]][32]) begin
                    out_busy[4] <= 0;
                end else begin
                    out_busy[4] <= out_valid4;
                end
                r_addr_buf[channel_select[4]] <= r_addr_buf[channel_select[4]] + 1;
            end
        end
    end

    wire [33:0] flit_tmp[0:4];
    wire [3:0] flit_router_id[0:4];
    wire is_head_flit[0:4];
    reg found_flit;

    assign flit_tmp[0] = flit_buffer[0][r_addr_buf[0][2:0]];
    assign flit_tmp[1] = flit_buffer[1][r_addr_buf[1][2:0]];
    assign flit_tmp[2] = flit_buffer[2][r_addr_buf[2][2:0]];
    assign flit_tmp[3] = flit_buffer[3][r_addr_buf[3][2:0]];
    assign flit_tmp[4] = flit_buffer[4][r_addr_buf[4][2:0]];
    assign flit_router_id[0] = flit_tmp[0][27:24];
    assign flit_router_id[1] = flit_tmp[1][27:24];
    assign flit_router_id[2] = flit_tmp[2][27:24];
    assign flit_router_id[3] = flit_tmp[3][27:24];
    assign flit_router_id[4] = flit_tmp[4][27:24];
    assign is_head_flit[0] = (flit_tmp[0][33] == 1);
    assign is_head_flit[1] = (flit_tmp[1][33] == 1);
    assign is_head_flit[2] = (flit_tmp[2][33] == 1);
    assign is_head_flit[3] = (flit_tmp[3][33] == 1);
    assign is_head_flit[4] = (flit_tmp[4][33] == 1);
    // input to output select
    // 0: local output channel
    always @(*) begin
        if(out_busy[0] == 0) begin
            
            if(buffer_empty[0] == 0 && flit_router_id[0] == Router_ID[3:0] && is_head_flit[0]) begin
                channel_select[0] = 0;
                out_valid0 = 1;
            end
            else if(buffer_empty[1] == 0 && flit_router_id[1] == Router_ID[3:0] && is_head_flit[1]) begin
                channel_select[0] = 1;
                out_valid0 = 1;
            end
            else if(buffer_empty[2] == 0 && flit_router_id[2] == Router_ID[3:0] && is_head_flit[2]) begin
                channel_select[0] = 2;
                out_valid0 = 1;
            end
            else if(buffer_empty[3] == 0 && flit_router_id[3] == Router_ID[3:0] && is_head_flit[3] ) begin
                channel_select[0] = 3;
                out_valid0 = 1;
            end
            else if(buffer_empty[4] == 0 && flit_router_id[4] == Router_ID[3:0] && is_head_flit[4] ) begin
                channel_select[0] = 4;
                out_valid0 = 1;
            end
            else begin
                channel_select[0] = 5;
                out_valid0 = 0;
            end
        end
        else begin
            channel_select[0] = channel_select_tmp[0];
            out_valid0 = 1;
        end
    end 
    //1: East output channel
    always @(*) begin
        if(out_busy[1] == 0) begin
            if(buffer_empty[0] == 0 && flit_tmp[0][25:24] > Router_ID[1:0] && is_head_flit[0]) begin
                channel_select[1] = 0;
                out_valid1 = 1;
            end
            else if(buffer_empty[1] == 0 && flit_tmp[1][25:24] > Router_ID[1:0] && is_head_flit[1]) begin
                channel_select[1] = 1;
                out_valid1 = 1;
            end
            else if(buffer_empty[2] == 0 && flit_tmp[2][25:24] > Router_ID[1:0] && is_head_flit[2]) begin
                channel_select[1] = 2;
                out_valid1 = 1;
            end
            else if(buffer_empty[3] == 0 && flit_tmp[3][25:24] > Router_ID[1:0] && is_head_flit[3]) begin
                channel_select[1] = 3;
                out_valid1 = 1;
            end
            else if(buffer_empty[4] == 0 && flit_tmp[4][25:24] > Router_ID[1:0] && is_head_flit[4]) begin
                channel_select[1] = 4;
                out_valid1 = 1;
            end
            else begin
                channel_select[1] = 5;
                out_valid1 = 0;
            end
        end
        else begin
            channel_select[1] = channel_select_tmp[1];
            out_valid1 = 1;
        end
    end 
    //3: West output channel
    always @(*) begin
        if(out_busy[3] == 0) begin
            if(buffer_empty[0] == 0 && flit_tmp[0][25:24] < Router_ID[1:0] && is_head_flit[0]) begin
                channel_select[3] = 0;
                out_valid3 = 1;
            end
            else if(buffer_empty[1] == 0 && flit_tmp[1][25:24] < Router_ID[1:0] && is_head_flit[1]) begin
                channel_select[3] = 1;
                out_valid3 = 1;
            end
            else if(buffer_empty[2] == 0 && flit_tmp[2][25:24] < Router_ID[1:0] && is_head_flit[2]) begin
                channel_select[3] = 2;
                out_valid3 = 1;
            end
            else if(buffer_empty[3] == 0 && flit_tmp[3][25:24] < Router_ID[1:0] && is_head_flit[3]) begin
                channel_select[3] = 3;
                out_valid3 = 1;
            end
            else if(buffer_empty[4] == 0 && flit_tmp[4][25:24] < Router_ID[1:0] && is_head_flit[4]) begin
                channel_select[3] = 4;
                out_valid3 = 1;
            end
            else begin
                channel_select[3] = 5;
                out_valid3 = 0;
            end
        end
        else begin
            channel_select[3] = channel_select_tmp[3];
            out_valid3 = 1;
        end
    end 
    //2: South output channel
    always @(*) begin
        if(out_busy[2] == 0) begin
            if(buffer_empty[0] == 0 && (flit_tmp[0][27:24] >> 2) > (Router_ID >> 2) && is_head_flit[0] && channel_select[1] != 0 && channel_select[3] != 0) begin
                channel_select[2] = 0;
                out_valid2 = 1;
            end
            else if(buffer_empty[1] == 0 && (flit_tmp[1][27:24] >> 2) > (Router_ID >> 2) && is_head_flit[1] && channel_select[1] != 1 && channel_select[3] != 1) begin
                channel_select[2] = 1;
                out_valid2 = 1;
            end
            else if(buffer_empty[2] == 0 && (flit_tmp[2][27:24] >> 2) > (Router_ID >> 2) && is_head_flit[2] && channel_select[1] != 2 && channel_select[3] != 2) begin
                channel_select[2] = 2;
                out_valid2 = 1;
            end
            else if(buffer_empty[3] == 0 && (flit_tmp[3][27:24] >> 2) > (Router_ID >> 2) && is_head_flit[3] && channel_select[1] != 3 && channel_select[3] != 3) begin
                channel_select[2] = 3;
                out_valid2 = 1;
            end
            else if(buffer_empty[4] == 0 && (flit_tmp[4][27:24] >> 2) > (Router_ID >> 2) && is_head_flit[4] && channel_select[1] != 4 && channel_select[3] != 4) begin
                channel_select[2] = 4;
                out_valid2 = 1;
            end
            else begin
                channel_select[2] = 5;
                out_valid2 = 0;
            end
        end
        else begin
            channel_select[2] = channel_select_tmp[2];
            out_valid2 = 1;
        end
    end 
    //4: North output channel
    always @(*) begin
        if(out_busy[4] == 0) begin
            if(buffer_empty[0] == 0 && (flit_tmp[0][27:24] >> 2) < (Router_ID >> 2) && is_head_flit[0] && channel_select[1] != 0 && channel_select[3] != 0) begin
                channel_select[4] = 0;
                out_valid4 = 1;
            end
            else if(buffer_empty[1] == 0 &&  (flit_tmp[1][27:24] >> 2) < (Router_ID >> 2) && is_head_flit[1] && channel_select[1] != 1 && channel_select[3] != 1) begin
                channel_select[4] = 1;
                out_valid4 = 1;
            end
            else if(buffer_empty[2] == 0 &&  (flit_tmp[2][27:24] >> 2) < (Router_ID >> 2) && is_head_flit[2] && channel_select[1] != 2 && channel_select[3] != 2) begin
                channel_select[4] = 2;
                out_valid4 = 1;
            end
            else if(buffer_empty[3] == 0 &&  (flit_tmp[3][27:24] >> 2) < (Router_ID >> 2) && is_head_flit[3] && channel_select[1] != 3 && channel_select[3] != 3) begin
                channel_select[4] = 3;
                out_valid4 = 1;
            end
            else if(buffer_empty[4] == 0 &&  (flit_tmp[4][27:24] >> 2) < (Router_ID >> 2) && is_head_flit[4] && channel_select[1] != 4 && channel_select[3] != 4) begin
                channel_select[4] = 4;
                out_valid4 = 1;
            end
            
            else begin
                channel_select[4] = 5;
                out_valid4 = 0;
            end
        end
        else begin
            channel_select[4] = channel_select_tmp[4];
            out_valid4 = 1;
        end
    end

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            channel_select_tmp[0] <= 0;
            channel_select_tmp[1] <= 0;
            channel_select_tmp[2] <= 0;
            channel_select_tmp[3] <= 0;
            channel_select_tmp[4] <= 0;
        end
        else begin
            channel_select_tmp[0] <= channel_select[0];
            channel_select_tmp[1] <= channel_select[1];
            channel_select_tmp[2] <= channel_select[2];
            channel_select_tmp[3] <= channel_select[3];
            channel_select_tmp[4] <= channel_select[4];
        end
    end

    always @(*) begin
        if(out_busy[0]) begin
            out_flit0 = flit_buffer[channel_select_tmp[0]][r_addr_buf[channel_select_tmp[0]][2:0]];
        end
        else begin
            out_flit0 = flit_buffer[channel_select[0]][r_addr_buf[channel_select[0]][2:0]];
        end
        if(out_busy[1]) begin
            out_flit1 = flit_buffer[channel_select_tmp[1]][r_addr_buf[channel_select_tmp[1]][2:0]];
        end
        else begin
            out_flit1 = flit_buffer[channel_select[1]][r_addr_buf[channel_select[1]][2:0]];
        end
        if(out_busy[2]) begin
            out_flit2 = flit_buffer[channel_select_tmp[2]][r_addr_buf[channel_select_tmp[2]][2:0]];
        end
        else begin
            out_flit2 = flit_buffer[channel_select[2]][r_addr_buf[channel_select[2]][2:0]];
        end
        if(out_busy[3]) begin
            out_flit3 = flit_buffer[channel_select_tmp[3]][r_addr_buf[channel_select_tmp[3]][2:0]];
        end
        else begin
            out_flit3 = flit_buffer[channel_select[3]][r_addr_buf[channel_select[3]][2:0]];
        end
        if(out_busy[4]) begin
            out_flit4 = flit_buffer[channel_select_tmp[4]][r_addr_buf[channel_select_tmp[4]][2:0]];
        end
        else begin
            out_flit4 = flit_buffer[channel_select[4]][r_addr_buf[channel_select[4]][2:0]];
        end
        
    end
endmodule