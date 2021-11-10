/*
 * OnlineDataHandler.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef ONLINEDATAHANDLER_H_
#define ONLINEDATAHANDLER_H_

#include "ngspice/sharedspice.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class OnlineDataHandler {
public:
	OnlineDataHandler();
	virtual ~OnlineDataHandler();

	virtual void initDataStructures(pvecinfoall newVecInfoAll);          // to be called from within SendInitData
	virtual void addDataPoints(pvecvaluesall newVecValuesAll, int len);  // to be called from within SendData

protected:

	// reset variables to "empty" state
	void reset(void) {
		plotName = plotTitle = plotDate = plotType = "";
		veccount = 0;
		vecNames.clear();
		isReal.clear();

		isInitialized = false;
	}

	bool isInitialized;

	string plotName;
	string plotTitle;
	string plotDate;
	string plotType;

	int veccount;
	vector<string> vecNames;
	vector<bool> isReal;

};

#endif /* ONLINEDATAHANDLER_H_ */
