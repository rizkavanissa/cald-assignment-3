@echo off
rem Compile C++ code using g++
g++ main.cpp converter.cpp operation.cpp cdfg.cpp iov.cpp -o hlsyn
rem Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
echo Compilation successful.

@REM rem Run the compiled program with arguments
@REM hlsyn error1.c 4 output_err1.v
@REM hlsyn error2.c 4 output_err2.v
@REM hlsyn error3.c 4 output_err3.v

@REM hlsyn test_if1.c 4 output_if1.v
@REM hlsyn test_if2.c 4 output_if2.v
@REM hlsyn test_if3.c 8 output_if3.v
@REM hlsyn test_if4.c 4 output_if4.v

rem Latency Test
@REM hlsyn hls_lat_test1.c 4 output_lat1.v
@REM hlsyn hls_lat_test2.c 9 output_lat2.v
@REM hlsyn hls_lat_test3.c 2 output_lat3.v
@REM hlsyn hls_lat_test4.c 4 output_lat4.v
@REM hlsyn hls_lat_test5.c 6 output_lat5.v

rem Standard Test
@REM hlsyn hls_test1.c 4 output_test1.v
@REM hlsyn hls_test2.c 6 output_test2.v
@REM hlsyn hls_test3.c 10 output_test3.v
@REM hlsyn hls_test5.c 11 output_test5.v
@REM hlsyn hls_test6.c 34 output_test6.v
@REM hlsyn hls_test7.c 8 output_test7.v
@REM hlsyn hls_test8.c 15 output_test8.v