`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, z, x);
    input Clk, Rst, Start;
    input signed [31:0] a; 
    input signed [31:0] b; 
    input signed [31:0] c; 
    output reg Done;
    output reg signed [31:0] z; 
    output reg signed [31:0] x; 
    reg signed [31:0] d; 
    reg signed [31:0] e; 
    reg signed [31:0] f; 
    reg signed [31:0] g; 
    reg signed [31:0] h; 
    reg dLTe; 
    reg dEQe; 
    reg [2:0] State;
    parameter Wait = 0, Final = 1, S0 = 2, S1 = 3, S2 = 4, S3 = 5, S4 = 6, S5 = 7;

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
                    e <= a + c;
                    State <= S2;
                end
                S2: begin
                    f <= a - b;
                    if (d < e) begin
                        dLTe <= 1;
                    end
                    else begin
                        dLTe <= 0;
                    end
                    State <= S3;
                end
                S3: begin
                    if (d == e) begin
                        dEQe <= 1;
                    end
                    else begin
                        dEQe <= 0;
                    end
                    g <= (dLTe)?d:e;
                    State <= S4;
                end
                S4: begin
                    h <= (dEQe)?g:f;
                    State <= S5;
                end
                S5: begin
                    x <= g << dLTe;
                    z <= h >> dEQe;
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
