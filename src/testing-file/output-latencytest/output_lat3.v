`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, i, j, k, l);
    input Clk, Rst, Start;
    input signed [15:0] a; 
    input signed [15:0] b; 
    input signed [15:0] c; 
    input signed [15:0] d; 
    input signed [15:0] e; 
    input signed [15:0] f; 
    input signed [15:0] g; 
    input signed [15:0] h; 
    output reg Done;
    output reg signed [15:0] i; 
    output reg signed [15:0] j; 
    output reg signed [15:0] k; 
    output reg signed [15:0] l; 
    reg [0:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3;

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
                    i <= a * b;
                    j <= c * d;
                    k <= e * f;
                    l <= g * h;
                    State <= S1;
                end
                S1: begin
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
