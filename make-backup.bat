@echo off
rem Compile C++ code using g++
g++ main.cpp Block.cpp CDFGraph.cpp Conditional.cpp ControlGraph.cpp Edge.cpp IOV.cpp Parser.cpp State.cpp Vertex.cpp -o hlsyn
rem Check if compilation was successful
if %errorlevel% neq 0 (
    echo Compilation failed.
    exit /b %errorlevel%
)
echo Compilation successful.

@REM rem Run the compiled program with arguments
@REM dpgen 474a_circuit1.txt acircuit1.v
@REM dpgen 474a_circuit2.txt acircuit2.v
@REM dpgen 474a_circuit3.txt acircuit3.v
@REM dpgen 474a_circuit4.txt acircuit4.v
@REM dpgen error1.txt error1.v
@REM dpgen error2.txt error2.v
@REM dpgen error3.txt error3.v
@REM dpgen error4.txt error4.v
@REM dpgen mixedcircuit1.txt mixedcircuit1.v
@REM dpgen mixedcircuit2.txt mixedcircuit2.v
@REM dpgen mixedcircuit3.txt mixedcircuit3.v
@REM dpgen ucircuit1.txt ucircuit1.v
@REM dpgen ucircuit2.txt ucircuit2.v
@REM dpgen ucircuit3.txt ucircuit3.v