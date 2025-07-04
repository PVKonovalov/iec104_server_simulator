#ifndef UTILS_H
#define UTILS_H
#include <string>

#include "cs104_slave.h"


void sigint_handler(int signalId);

unsigned int init_rand();

std::string CP56Time2aToString(CP56Time2a time);

void print_usage();

#endif
