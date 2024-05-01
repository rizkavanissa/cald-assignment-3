`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, avg);
    input Clk, Rst, Start;
    input [31:0] a; 
    input [31:0] b; 
    input [31:0] c; 
    input [31:0] d; 
    input [31:0] e; 
    input [31:0] f; 
    input [31:0] g; 
    input [31:0] h; 
    input [31:0] sa; 
    output reg Done;
    output reg [31:0] avg; 
    reg [31:0] t1; 
    reg [31:0] t2; 
    reg [31:0] t3; 
    reg [31:0] t4; 
    reg [31:0] t5; 
    reg [31:0] t6; 
    reg [31:0] t7; 
    reg [31:0] t7div2; 
    reg [31:0] t7div4; 
    reg [3:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7, S6 = 8, S7 = 9, S8 = 10, S9 = 11, S10 = 12;

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
                    t7div2 <= t7 >> sa;
                    State <= S8;
                end
                S8: begin
                    t7div4 <= t7div2 >> sa;
                    State <= S9;
                end
                S9: begin
                    avg <= t7div4 >> sa;
                    State <= S10;
                end
                S10: begin
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
