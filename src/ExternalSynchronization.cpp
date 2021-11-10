/*
 * ExternalSynchronization.cpp
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#include <ExternalSynchronization.h>

ExternalSynchronization::ExternalSynchronization() {
	// TODO Auto-generated constructor stub

}

ExternalSynchronization::~ExternalSynchronization() {
	// TODO Auto-generated destructor stub
}

// so how does it actually work?
double ExternalSynchronization::synchronize(double ckttime, double olddelta, int redostep, int loc) {
	cout << "synchronize: ckttime=" << ckttime << ", olddelta=" << olddelta << ", redostep=" << redostep << ", loc=" << loc << endl;

	// return value is used as timestep in the connected simulation
	double cktdelta = 1.0e-6;
	return cktdelta;
}

