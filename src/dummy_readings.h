#pragma once
#include "cs104_slave.h"

#ifndef DUMMY_READINGS_H
#define DUMMY_READINGS_H


InformationObject makeReading(float baseVal, float deviation, TypeID type, int ioa);

float floatVal(float baseVal, float deviation);

float intVal(float baseVal, float deviation);

#endif
