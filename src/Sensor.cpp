#include <iostream>
#include <fstream>
#include "../include/Sensor.h"

using namespace std;

void Sensor::getSensorSts(int *senSts)
{
	ifstream ifs("/dev/rtlightsensor0");
	ifs >> senSts[0] >> senSts[1] >> senSts[2] >> senSts[3];
	
	/*
	// Debug
	cout 	<< senSts[0] << " " 
		<< senSts[1] << " "  
		<< senSts[2] << " " 
		<< senSts[3] << endl;
	//*/
}