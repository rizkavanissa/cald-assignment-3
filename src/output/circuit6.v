`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, i, j, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee, ff, gg, hh, ii, final);
    input Clk, Rst, Start;
    input [31:0] a; 
    input [31:0] b; 
    input [31:0] c; 
    input [31:0] d; 
    input [31:0] e; 
    input [31:0] f; 
    input [31:0] g; 
    input [31:0] h; 
    input [31:0] i; 
    input [31:0] j; 
    input [31:0] l; 
    input [31:0] m; 
    input [31:0] n; 
    input [31:0] o; 
    input [31:0] p; 
    input [31:0] q; 
    input [31:0] r; 
    input [31:0] s; 
    input [31:0] t; 
    input [31:0] u; 
    input [31:0] v; 
    input [31:0] w; 
    input [31:0] x; 
    input [31:0] y; 
    input [31:0] z; 
    input [31:0] aa; 
    input [31:0] bb; 
    input [31:0] cc; 
    input [31:0] dd; 
    input [31:0] ee; 
    input [31:0] ff; 
    input [31:0] gg; 
    input [31:0] hh; 
    input [31:0] ii; 
    output reg Done;
    output reg [31:0] final; 
    reg [31:0] t1; 
    reg [31:0] t2; 
    reg [31:0] t3; 
    reg [31:0] t4; 
    reg [31:0] t5; 
    reg [31:0] t6; 
    reg [31:0] t7; 
    reg [31:0] t8; 
    reg [31:0] t9; 
    reg [31:0] t10; 
    reg [31:0] t11; 
    reg [31:0] t12; 
    reg [31:0] t13; 
    reg [31:0] t14; 
    reg [31:0] t15; 
    reg [31:0] t16; 
    reg [31:0] t17; 
    reg [31:0] t18; 
    reg [31:0] t19; 
    reg [31:0] t20; 
    reg [31:0] t21; 
    reg [31:0] t22; 
    reg [31:0] t23; 
    reg [31:0] t24; 
    reg [31:0] t25; 
    reg [31:0] t26; 
    reg [31:0] t27; 
    reg [31:0] t28; 
    reg [31:0] t29; 
    reg [31:0] t30; 
    reg [31:0] t31; 
    reg [31:0] t32; 
    reg [31:0] t33; 
    reg [5:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7, S6 = 8, S7 = 9, S8 = 10, S9 = 11, S10 = 12, S11 = 13, S12 = 14, S13 = 15, S14 = 16, S15 = 17, S16 = 18, S17 = 19, S18 = 20, S19 = 21, S20 = 22, S21 = 23, S22 = 24, S23 = 25, S24 = 26, S25 = 27, S26 = 28, S27 = 29, S28 = 30, S29 = 31, S30 = 32, S31 = 33, S32 = 34, S33 = 35;

    always @(posedge Clk) begin
        if (Rst == 1) begin
            Done <= 0;
            State <= Wait;
        end
        else begin
            case(State)
                Wait: begin
                    Done <= 0;
                    if (Start == 1) begin
                        State <= S0;
                    end
                    else begin
                        State <= Wait;
                    end
                end
                S0: begin
                    t1 <= a + b;
                    State <= S1;
                end
                S1: begin
                    t2 <= t1 + c;
                    State <= S2;
                end
                S2: begin
                    t3 <= t2 + d;
                    State <= S3;
                end
                S3: begin
                    t4 <= t3 + e;
                    State <= S4;
                end
                S4: begin
                    t5 <= t4 + f;
                    State <= S5;
                end
                S5: begin
                    t6 <= t5 + g;
                    State <= S6;
                end
                S6: begin
                    t7 <= t6 + h;
                    State <= S7;
                end
                S7: begin
                    t8 <= t7 + i;
                    State <= S8;
                end
                S8: begin
                    t9 <= t8 + j;
                    State <= S9;
                end
                S9: begin
                    t10 <= t9 + l;
                    State <= S10;
                end
                S10: begin
                    t11 <= t10 + m;
                    State <= S11;
                end
                S11: begin
                    t12 <= t11 + n;
                    State <= S12;
                end
                S12: begin
                    t13 <= t12 + o;
                    State <= S13;
                end
                S13: begin
                    t14 <= t13 + p;
                    State <= S14;
                end
                S14: begin
                    t15 <= t14 + q;
                    State <= S15;
                end
                S15: begin
                    t16 <= t15 + r;
                    State <= S16;
                end
                S16: begin
                    t17 <= t16 + s;
                    State <= S17;
                end
                S17: begin
                    t18 <= t17 + t;
                    State <= S18;
                end
                S18: begin
                    t19 <= t18 + u;
                    State <= S19;
                end
                S19: begin
                    t20 <= t19 + v;
                    State <= S20;
                end
                S20: begin
                    t21 <= t20 + w;
                    State <= S21;
                end
                S21: begin
                    t22 <= t21 + x;
                    State <= S22;
                end
                S22: begin
                    t23 <= t22 + y;
                    State <= S23;
                end
                S23: begin
                    t24 <= t23 + z;
                    State <= S24;
                end
                S24: begin
                    t25 <= t24 + aa;
                    State <= S25;
                end
                S25: begin
                    t26 <= t25 + bb;
                    State <= S26;
                end
                S26: begin
                    t27 <= t26 + cc;
                    State <= S27;
                end
                S27: begin
                    t28 <= t27 + dd;
                    State <= S28;
                end
                S28: begin
                    t29 <= t28 + ee;
                    State <= S29;
                end
                S29: begin
                    t30 <= t29 + ff;
                    State <= S30;
                end
                S30: begin
                    t31 <= t30 + gg;
                    State <= S31;
                end
                S31: begin
                    t32 <= t31 + hh;
                    State <= S32;
                end
                S32: begin
                    t33 <= t32 + ii;
                    State <= S33;
                end
                S33: begin
                    final <= t33 + t1;
                    State <= Final;
                end
                Final: begin
                    Done <= 1;
                    State <= Wait;
                end
            endcase
        end
    end
endmodule
