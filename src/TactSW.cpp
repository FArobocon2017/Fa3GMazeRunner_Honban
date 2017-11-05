#include <iostream>
#include <fstream>
#include "../include/TactSW.h"

using namespace std;

void TactSW::getTactSwStsAll(int *tswSts)
{
	ifstream ifs0("/dev/rtswitch0");
	ifs0 >> tswSts[0];
	ifstream ifs1("/dev/rtswitch1");
	ifs1 >> tswSts[1];
	ifstream ifs2("/dev/rtswitch2");
	ifs2 >> tswSts[2];

	/*
	// Debug
	cout 	<< tswSts[0] << " " 
		<< tswSts[1] << " "  
		<< tswSts[2] << endl;
	//*/
}


int TactSW::getTactSwSts0()
{
	int tswSts = 0;
	ifstream ifs("/dev/rtswitch0");
	ifs >> tswSts;

	return tswSts;
}

int TactSW::getTactSwSts1()
{
	int tswSts = 0;
	ifstream ifs("/dev/rtswitch1");
	ifs >> tswSts;

	return tswSts;
}

int TactSW::getTactSwSts2()
{
	int tswSts = 0;
	ifstream ifs("/dev/rtswitch2");
	ifs >> tswSts;

	return tswSts;
}
