# Enable support for a specific C++ standard in CMake

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Define the target application executable and the list of CPP sources

add_executable(dpgen main.cpp Vertex.cpp State.cpp Parser.cpp IOV.cpp Edge.cpp ControlGraph.cpp Conditional.cpp CDFGraph.cpp Block.cpp)