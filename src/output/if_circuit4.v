`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, zero, one, t, z, x);
    input Clk, Rst, Start;
    input signed [31:0] a; 
    input signed [31:0] b; 
    input signed [31:0] c; 
    input signed [31:0] zero; 
    input signed [31:0] one; 
    input signed t; 
    output reg Done;
    output reg signed [31:0] z; 
    output reg signed [31:0] x; 
    reg signed [31:0] e; 
    reg signed [31:0] g; 
    reg signed [31:0] d; 
    reg signed [31:0] f; 
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
                    State <= S1;
                end
                S1: begin
                    if (t) begin
                        d <= a - one;
                        f <= a + c;
                        x <= f - d;
                        end
                    State <= S2;
                end
                S2: begin
                    State <= S3;
                end
                S3: begin
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
