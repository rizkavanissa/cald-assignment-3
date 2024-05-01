clang++ -I includes/ lib/*.cpp main.cpp -o hlsyn --std=c++17

./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test1.c" 4 output/circuit1.v
./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test2.c" 6 output/circuit2.v
./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test3.c" 10 output/circuit3.v
./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test5.c" 11 output/circuit5.v
./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test6.c" 34 output/circuit6.v
./hlsyn "../assignment3_testfiles_new/standard_tests/hls_test7.c" 8 output/circuit7.v

./hlsyn "../assignment3_testfiles_new/if tests/test_if1.c" 4 output/if_circuit1.v
./hlsyn "../assignment3_testfiles_new/if tests/test_if2.c" 4 output/if_circuit2.v
./hlsyn "../assignment3_testfiles_new/if tests/test_if3.c" 8 output/if_circuit3.v
./hlsyn "../assignment3_testfiles_new/if tests/test_if4.c" 4 output/if_circuit4.v

./hlsyn "../assignment3_testfiles_new/latency tests/hls_lat_test1.c" 4 output/latency_circuit1.v
./hlsyn "../assignment3_testfiles_new/latency tests/hls_lat_test2.c" 9 output/latency_circuit2.v
./hlsyn "../assignment3_testfiles_new/latency tests/hls_lat_test3.c" 2 output/latency_circuit3.v
./hlsyn "../assignment3_testfiles_new/latency tests/hls_lat_test4.c" 4 output/latency_circuit4.v
./hlsyn "../assignment3_testfiles_new/latency tests/hls_lat_test5.c" 6 output/latency_circuit5.v

./hlsyn "../assignment3_testfiles_new/error tests/error1.c" 4 output/error_circuit1.v
./hlsyn "../assignment3_testfiles_new/error tests/error2.c" 4 output/error_circuit2.v
./hlsyn "../assignment3_testfiles_new/error tests/error3.c" 4 output/error_circuit3.v
