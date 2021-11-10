/*
 * ExternalVoltageSource.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef EXTERNALVOLTAGESOURCE_H_
#define EXTERNALVOLTAGESOURCE_H_

#include <string>

using namespace std;

class ExternalVoltageSource {
public:
	ExternalVoltageSource();
	virtual ~ExternalVoltageSource();

	virtual double getVoltage(std::string nodename, double time);
};

#endif /* EXTERNALVOLTAGESOURCE_H_ */
