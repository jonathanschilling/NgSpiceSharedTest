/*
 * ExternalSynchronization.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef EXTERNALSYNCHRONIZATION_H_
#define EXTERNALSYNCHRONIZATION_H_

#include <iostream>

using namespace std;

class ExternalSynchronization {
public:
	ExternalSynchronization();
	virtual ~ExternalSynchronization();

	virtual double synchronize(double ckttime, double olddelta, int redostep, int loc);
};

#endif /* EXTERNALSYNCHRONIZATION_H_ */
