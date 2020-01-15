#pragma once
#ifndef _DOF_H_
#define _DOF_H_
#include <iostream> 
#include <stdlib.h> 
#include <time.h>  
#include <vector>
#include <math.h>
#include "Vector3.h"
#include "mathlib.h"
#define NUMDOF 20
#define RANDOMMAX 10000
#define pi 3.1415926
#define	Debug_MCP 1
#define	Debug_PCP 1
#define	Debug_DIP 1
using namespace std;				



const float max_angle[NUMDOF] = {   20.0 / 180*0,	75.0 / 180*0,	95.0 / 180*0,	75.0 / 180*0,//thumb don't change
									20.0 / 180*0,	75.0 / 180* Debug_MCP,	95.0 / 180* Debug_PCP,	75.0 / 180 * Debug_DIP,
									20.0 / 180*0,	75.0 / 180* Debug_MCP,	95.0 / 180 * Debug_PCP,	75.0 / 180 * Debug_DIP,
									20.0 / 180*0,	75.0 / 180* Debug_MCP,	95.0 / 180 * Debug_PCP,	75.0 / 180 * Debug_DIP,
									20.0 / 180*0,	75.0 / 180* Debug_MCP,	95.0 / 180 * Debug_PCP,	75.0 / 180 * Debug_DIP };
const float min_angle[NUMDOF] = {   -20.0 / 180 * 0,	-45.0 / 180*0,	0,	0 ,				//thumb don't change
									-20.0 / 180 * 0,	-45.0 / 180* Debug_MCP,	0,	0,
									-20.0 / 180 * 0,	-45.0 / 180* Debug_MCP,	0,	0  ,
									-20.0 / 180 * 0,	-45.0 / 180* Debug_MCP,	0,	0,
									-20.0 / 180 * 0,	-45.0 / 180* Debug_MCP,	0,	0 };
class DOF {
public:
	static vector<float> randDOF() {
		vector<float>df;
		srand((unsigned)time(NULL));
		for (int i = 0; i < NUMDOF; i++) {
			df.push_back(-1 * (rand() % RANDOMMAX *1.0 / RANDOMMAX*(max_angle[i] - min_angle[i]) + min_angle[i])*pi);
			//df.push_back(-1 * ((max_angle[i] - min_angle[i]) + min_angle[i])*pi);
		}
		return df;
	}
};
#endif