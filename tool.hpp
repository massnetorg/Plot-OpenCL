//
// Created by Blink on 2020/10/1.
//

#ifndef PLOT_OPENCL_TOOL_HPP
#define PLOT_OPENCL_TOOL_HPP


#include <OpenCL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

/** convert the kernel file into a string */
int convertToString(const char *filename, std::string& s);

/**Getting platforms and choose an available one.*/
int getPlatform(cl_platform_id &platform);

/**Step 2:Query the platform and choose the first GPU device if has one.*/
cl_device_id *getCl_device_id(cl_platform_id &platform);


#endif //PLOT_OPENCL_TOOL_HPP
