@echo off
rem Compile C++ code using g++
@REM g++ main.cpp Block.cpp CDFGraph.cpp Conditional.cpp ControlGraph.cpp Edge.cpp IOV.cpp Parser.cpp State.cpp Vertex.cpp -o hlsyn
@REM rem Check if compilation was successful
@REM if %errorlevel% neq 0 (
@REM     echo Compilation failed.
@REM     exit /b %errorlevel%
@REM )

rem Set the source code directory (replace "src" with your actual subfolder name)
set DIR_1=C:\Users\brand\OneDrive\Documents\Spring_2023_2024\Computer-Aided Logic Design\Assignment\Homework3\cald-assignment-3\src\lib
set DIR_2=C:\Users\brand\OneDrive\Documents\Spring_2023_2024\Computer-Aided Logic Design\Assignment\Homework3\cald-assignment-3\src

rem Compile all .cpp files within the source directory (excluding main.cpp)
for /f "tokens=*" %%a in ("%DIR_1%\*.cpp") do (
  if "%%a" neq "%DIR_1%\main.cpp" (
    g++ "%%a" -o hlsyn
    if %errorlevel% neq 0 (
      echo Compilation failed for %%a. Aborting.
      exit /b 1
    )
  )
)

rem Compile main.cpp separately
g++ "%DIR_2%\main.cpp" -o hlsyn

if %errorlevel% neq 0 (
  echo Compilation failed for main.cpp. Aborting.
  exit /b 1
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