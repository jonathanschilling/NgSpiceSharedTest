//============================================================================
// Name        : NgSpiceSharedTest.cpp
// Author      : Jonathan Schilling (jonathan.schilling@ipp.mpg.de)
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <NgSpiceLocalServer.h>

int main(int argc, char** argv) {

	NgSpiceLocalServer* server = new NgSpiceLocalServer();

	// TODO: handle commandline arguments and setup server accordingly

	return server->startService();
}


