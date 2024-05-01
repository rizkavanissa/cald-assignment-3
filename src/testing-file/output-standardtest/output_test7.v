`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, u, x, y, dx, a, three, u1, x1, y1, c);
    input Clk, Rst, Start;
    input signed [31:0] u; 
    input signed [31:0] x; 
    input signed [31:0] y; 
    input signed [31:0] dx; 
    input signed [31:0] a; 
    input signed [31:0] three; 
    output reg Done;
    output reg signed [31:0] u1; 
    output reg signed [31:0] x1; 
    output reg signed [31:0] y1; 
    output reg signed [31:0] c; 
    reg signed [31:0] t1; 
    reg signed [31:0] t2; 
    reg signed [31:0] t3; 
    reg signed [31:0] t4; 
    reg signed [31:0] t5; 
    reg signed [31:0] t6; 
    reg signed [31:0] t7; 
    reg signed [31:0] vx1; 
    reg [2:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7, S6 = 8, S7 = 9;

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
                    vx1 <= x + dx;
                    t1 <= three * x;
                    State <= S1;
                end
                S1: begin
                    x1 <= x + dx;
                    if (vx1 < a) begin
                        c <= 1;
                    end
                    else begin
                        c <= 0;
                    end
                    State <= S2;
                end
                S2: begin
                    t2 <= u * dx;
                    State <= S3;
                end
                S3: begin
                    t5 <= three * y;
                    State <= S4;
                end
                S4: begin
                    t3 <= t1 * t2;
                    State <= S5;
                end
                S5: begin
                    t6 <= t5 * dx;
                    t7 <= u * dx;
                    State <= S6;
                end
                S6: begin
                    t4 <= u - t3;
                    State <= S7;
                end
                S7: begin
                    u1 <= t4 - t6;
                    y1 <= y + t7;
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
