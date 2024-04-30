`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, z, x);
    input Clk, Rst, Start;
    input signed [15:0] a; 
    input signed [15:0] b; 
    output reg Done;
    output reg signed [7:0] z; 
    output reg signed [15:0] x; 
    reg signed [15:0] d; 
    reg signed [15:0] e; 
    reg signed [15:0] f; 
    reg signed [15:0] g; 
    reg [1:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5;

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
                    d <= a + b;
