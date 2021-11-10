/*
 * ExternalCurrentSource.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef EXTERNALCURRENTSOURCE_H_
#define EXTERNALCURRENTSOURCE_H_

#include <string>
#include <iostream>

using namespace std;

class ExternalCurrentSource {
public:
	ExternalCurrentSource();
	virtual ~ExternalCurrentSource();

	virtual double getCurrent(std::string nodename, double time);
};

#endif /* EXTERNALCURRENTSOURCE_H_ */
