/*
 * IOHandler.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#include <string>
#include <iostream>

using namespace std;

class IOHandler {
public:
	IOHandler();
	virtual ~IOHandler();

	virtual void handleStdOut(std::string outMessage);
	virtual void handleStdErr(std::string errMessage);
};

#endif /* IOHANDLER_H_ */
