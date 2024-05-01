`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, d, e, i);
    input Clk, Rst, Start;
    input signed [15:0] a; 
    input signed [15:0] b; 
    input signed [15:0] c; 
    input signed [15:0] d; 
    input signed [15:0] e; 
    output reg Done;
    output reg signed [15:0] i; 
    reg signed [15:0] f; 
    reg signed [15:0] g; 
    reg signed [15:0] h; 
    reg [3:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7, S6 = 8, S7 = 9, S8 = 10;

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
                    f <= a * b;
                    State <= S1;
                end
                S1: begin
                    State <= S2;
                end
                S2: begin
                    State <= S3;
                end
                S3: begin
                    g <= f * c;
                    State <= S4;
                end
                S4: begin
                    State <= S5;
                end
                S5: begin
                    h <= g * d;
                    State <= S6;
                end
                S6: begin
                    State <= S7;
                end
                S7: begin
                    i <= h * e;
                    State <= S8;
                end
                S8: begin
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
