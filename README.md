# ROB307
Software developments for the project course ROB307 @ENSTA

The goal of the course ROB307 - MPSOC was to design and test on a Xilinx Zedboard card a complete heterogeneous multi-processor architecture for a delivery UAV.

This repository encapsulates the five core functionnalities - Control, Perception, Learning, Navigation and Image Processing - and functions - Matrix Multiplication, Median Filter, K-Means, ICP and Windowed Convolution - to be implemented on the MPSOC.

For each functionnality, we provide a Vivado HLS compatible implementation of the function and a corresponding test bench file. One can also find a *Time.cpp file, which can be used to estimate the execution time of the program.

