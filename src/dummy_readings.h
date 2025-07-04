#pragma once
#include "cs104_slave.h"

#ifndef DUMMY_READINGS_H
#define DUMMY_READINGS_H



InformationObject make_reading(int val, int deviation, TypeID type, int ioa);

#endif
