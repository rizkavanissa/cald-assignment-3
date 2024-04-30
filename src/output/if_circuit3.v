`timescale 1ns / 1ps

module HLSM (Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, one, two, four, avg);
    input Clk, Rst, Start;
    input signed [31:0] a; 
    input signed [31:0] b; 
    input signed [31:0] c; 
    input signed [31:0] d; 
    input signed [31:0] e; 
    input signed [31:0] f; 
    input signed [31:0] g; 
    input signed [31:0] h; 
    input signed [31:0] sa; 
    input signed [31:0] one; 
    input signed [31:0] two; 
    input signed [31:0] four; 
    output reg Done;
    output reg signed [31:0] avg; 
    reg signed [31:0] t1; 
    reg signed [31:0] t2; 
    reg signed [31:0] t3; 
    reg signed [31:0] t4; 
    reg signed [31:0] t5; 
    reg signed [31:0] t6; 
    reg signed [31:0] t7; 
    reg signed [31:0] t7div2; 
    reg signed [31:0] t7div4; 
    reg signed csa1; 
    reg signed csa2; 
    reg signed csa3; 
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
                    t1 <= a + b;
                    if (sa > one) begin
                        csa1 <= 1;
                    end
                    else begin
                        csa1 <= 0;
                    end
                    State <= S1;
                end
                S1: begin
                    t2 <= t1 + c;
                    if (sa > two) begin
                        csa2 <= 1;
                    end
                    else begin
                        csa2 <= 0;
                    end
                    State <= S2;
                end
                S2: begin
                    t3 <= t2 + d;
                    if (sa > four) begin
                        csa3 <= 1;
                    end
                    else begin
                        csa3 <= 0;
                    end
                    State <= S3;
                end
                S3: begin
                    t4 <= t3 + e;
                    if (csa1) begin
                        t7div2 <= t7 >> one;
                        if (csa2) begin
                            t7div4 <= t7div2 >> one;
                        end
                        else begin
                            t7div4 <= t7 >> one;
                            if (csa3) begin
                                avg <= t7div4 >> one;
                            end
                            end
                        end
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
