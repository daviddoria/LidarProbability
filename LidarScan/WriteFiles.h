#ifndef WRITEFILES_H
#define WRITEFILES_H

#include <string.h>
#include <fstream>
#include <iostream>

#include "LidarPoint.h"
#include "LidarStrip.h"
#include "LidarScan.h"
#include "LidarScanner.h"
#include <geom_Geometry3.h>

using namespace std;

void WriteParaviewFile(const char* sOutputFileName, LidarScanner &Scanner);
void WriteRawPoints(const char* sOutputFileName, LidarScan &Scan);


#endif

