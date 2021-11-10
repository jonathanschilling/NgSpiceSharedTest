/*
 * IOHandler.cpp
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#include <IOHandler.h>

IOHandler::IOHandler() {

}

IOHandler::~IOHandler() {
	// TODO Auto-generated destructor stub
}

void IOHandler::handleStdOut(std::string outMessage) {
	std::cout << outMessage;
}

void IOHandler::handleStdErr(std::string errMessage) {
	std::cerr << errMessage;
}
