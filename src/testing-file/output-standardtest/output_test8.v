`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, x0, x1, x2, x3, y0, c0, five, ten, d1, d2, e, f, g, h);
    input Clk, Rst, Start;
    input signed [31:0] x0; 
    input signed [31:0] x1; 
    input signed [31:0] x2; 
    input signed [31:0] x3; 
    input signed [31:0] y0; 
    input signed [31:0] c0; 
    input signed [31:0] five; 
    input signed [31:0] ten; 
    output reg Done;
    output reg signed [31:0] d1; 
    output reg signed [31:0] d2; 
    output reg signed [31:0] e; 
    output reg signed [31:0] f; 
    output reg signed [31:0] g; 
    output reg signed [31:0] h; 
    reg signed [31:0] t1; 
    reg signed [31:0] t2; 
    reg signed [31:0] t3; 
    reg signed [31:0] vd1; 
    reg signed [31:0] ve; 
    reg signed [31:0] vf; 
    reg signed [31:0] vg; 
    reg [3:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7, S6 = 8, S7 = 9, S8 = 10, S9 = 11, S10 = 12, S11 = 13, S12 = 14, S13 = 15, S14 = 16;

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
                    t2 <= x1 + x2;
                    State <= S1;
                end
                S1: begin
                    t3 <= x3 + c0;
                    State <= S2;
                end
                S2: begin
                    t1 <= x0 + x1;
                    ve <= t2 * t3;
                    State <= S3;
                end
                S3: begin
                    vg <= x0 - ten;
                    State <= S4;
                end
                S4: begin
                    g <= x0 - ten;
                    State <= S5;
                end
                S5: begin
                    State <= S6;
                end
                S6: begin
                    State <= S7;
                end
                S7: begin
                    State <= S8;
                end
                S8: begin
                    State <= S9;
                end
                S9: begin
                    State <= S10;
                end
                S10: begin
                    State <= S11;
                end
                S11: begin
                    vd1 <= t1 * c0;
                    State <= S12;
                end
                S12: begin
                    vf <= ve * y0;
                    State <= S13;
                end
                S13: begin
                    d1 <= t1 * c0;
                    d2 <= vd1 * five;
                    e <= t2 * t3;
                    f <= ve * y0;
                    State <= S14;
                end
                S14: begin
                    h <= vf + vg;
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
