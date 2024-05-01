`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, z, x);
    input Clk, Rst, Start;
    input [15:0] a; 
    input [15:0] b; 
    input [15:0] c; 
    output reg Done;
    output reg [7:0] z; 
    output reg [15:0] x; 
    reg [7:0] d; 
    reg [7:0] e; 
    reg [7:0] f; 
    reg [7:0] g; 
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
                    f <= a * c;
                    State <= S1;
                end
                S1: begin
                    e <= a + c;
                    State <= S2;
                end
                S2: begin
                    if (d > e) begin
                        g <= 1;
                    end
                    else begin
                        g <= 0;
                    end
                    x <= f - d;
                    State <= S3;
                end
                S3: begin
                    z <= (g)?d:e;
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
