/*
 * SimulationStatusHandler.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef SIMULATIONSTATUSHANDLER_H_
#define SIMULATIONSTATUSHANDLER_H_

#include <string>
#include <iostream>

using namespace std;

class SimulationStatusHandler {
public:
	SimulationStatusHandler();
	virtual ~SimulationStatusHandler();

	virtual void handleSimulationStatus(std::string message);
};

#endif /* SIMULATIONSTATUSHANDLER_H_ */
