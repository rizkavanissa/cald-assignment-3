# Authors and NetID: 	
# 1. Brandon John Lim Yung Chen - brandonjchen
# 2. Moh Erwin Septianto - mohseptianto
# 3. Muhammad Nafis Aimanurrohman - muhnafis
# 4. Pieter Timothy - pietertimothy
# 5. Rizka Vanissa Alifia - rizkavanissa

# Date: 1 May 2024
# Assignment Number: 3

# A simple example CMake Setup for Compiling C Programs. This setup utilizes a single
# subdirectory (src) in which all CPP source (.cpp) and header (.h) files within that 
# directory are included within the project.

# CMake projects are forward compatible, but may not be backwards compatible. This
# commands defines the version of CMake used for the current setup.

cmake_minimum_required(VERSION 3.10)

# Define the project name. 

project(hlysn)

# Add a subdirectory to the project. The CMakeLists.txt file in that subdirectory
# will be used for further setting up the project.

add_subdirectory(src)
