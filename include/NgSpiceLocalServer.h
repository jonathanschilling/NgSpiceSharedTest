/*
 * NgSpiceLocalServer.h
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef NGSPICELOCALSERVER_H_
#define NGSPICELOCALSERVER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <atomic>
#include <utility>
#include <thread>
#include <sstream>

// socket includes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ngspice interface and serialization
#include "NgSpice.h"
#include "CborSerialization.h"

// 1MB buffer size
#define BUF (1024*1024)

// On a linux platform, sockets are identified by int file descriptors.
typedef int SOCKET;

class NgSpiceLocalServer:
		// this one does it all:
		public IOHandler,
		public OnlineDataHandler,
		public SimulationStatusHandler,
		public ExternalVoltageSource,
		public ExternalCurrentSource,
		public ExternalSynchronization {

public:
	NgSpiceLocalServer();
	virtual ~NgSpiceLocalServer();

	SOCKET mySocket;
	unsigned char *buffer;
	ssize_t msgSize;

	int startService();
	pair<unsigned char*, int> receiveFromSocket(SOCKET sock);
	int sendToSocket(SOCKET sock, string cborToSend);

	CborSerialization* ser;

	string handle_command_CBOR(unsigned char *cborIn, int len);
	string serializeCBOR(pair<string, void*> data);
	pair<string, void*> deserializeCBOR(unsigned char *cborIn, int len);
	void* handleCommand(string command, void* data);

	void Reset();
	int Init();

	void regUnregHandler(std::string handler, bool newState);

	// IOHandler
	void handleStdOut(std::string outMessage) override;
	void handleStdErr(std::string errMessage) override;

	// OnlineDataHandler
	void initDataStructures(pvecinfoall newVecInfoAll) override;
	void addDataPoints(pvecvaluesall newVecValuesAll, int len) override;

	// SimulationStatusHandler
	void handleSimulationStatus(std::string message) override;

	// ExternalVoltageSource
	double getVoltage(std::string nodename, double time) override;

	// ExternalCurrentSource
	double getCurrent(std::string nodename, double time) override;

	// ExternalSynchronization
	double synchronize(double ckttime, double olddelta, int redostep, int loc) override;

protected:

	NgSpice* ngSpice = NULL;

	bool registeredForIOHandler              ;
	bool registeredForOnlineDataHandler      ;
	bool registeredForSimulationStatusHandler;
	bool registeredForExternalVoltageSource  ;
	bool registeredForExternalCurrentSource  ;
	bool registeredForExternalSynchronization;

};

#endif /* NGSPICELOCALSERVER_H_ */
