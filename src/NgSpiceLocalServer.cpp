/*
 * NgSpiceLocalServer.cpp
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#include <NgSpiceLocalServer.h>

NgSpiceLocalServer::NgSpiceLocalServer() {
	mySocket = 0;

	Reset();
}

NgSpiceLocalServer::~NgSpiceLocalServer() {

}

// setup socket and start listening / handling requests
int NgSpiceLocalServer::startService() {

	int status = 0;
	buffer = (unsigned char*)malloc(BUF);

	const int socketDomain = AF_INET;   // IPv4 protocol
	const int socketType = SOCK_STREAM; // reliable TCP connection
	const int socketProtocol = 0;       // default: stream => TCP

	// create socket
	const int create_socket = socket(socketDomain, socketType, socketProtocol);
	if (create_socket <0 ) {
		cout << "Error: socket could not be created." << endl;
		return create_socket; // return error code of socket
	}

	// allow reusage of address => allows to restart server on same port faster
	const int y = 1;
	setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y));


	// listening address
	struct sockaddr_in address;

	// init struct to zeros
	memset(&address, 0, sizeof(address));

	// 0.0.0.0:15000
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons (15000);

	// bind to socket
	if ( (status=bind(create_socket, (struct sockaddr *) &address, sizeof (address))) == 0 ) {

		printf ("binding of socket was successful\n");

		// wait for incoming requests from one client
		status=listen(create_socket, 1);
		if (status<0) {
			cout << "Error: could not start listening: port already in use?" << endl;
			return status;
		}

		unsigned int addrlen = sizeof (struct sockaddr_in);

		// server's duty is to serve requests for its whole life
		bool keepAlive = true;
		while (keepAlive) {

			cout << "accepting connections from client..." << endl;

			// wait for connection from client
			mySocket = accept(create_socket, (struct sockaddr *) &address, &addrlen);

			if (mySocket < 0) {
				cout << "ERROR: client failed to get accepted" << endl;
			} else {
				cout << "A new client (" << inet_ntoa (address.sin_addr) << ") is connected" << endl;

				// handle messages from this particular client
				while(true) {

					pair<unsigned char*, int> commandAndLen = receiveFromSocket(mySocket);

					string cborReturnValue = handle_command_CBOR(commandAndLen.first, commandAndLen.second);

					cout << "returning '" << cborReturnValue << "' to client" << endl;

					sendToSocket(mySocket, cborReturnValue);
				}
			}
		}

		// close socket at the end
		int closeRet = close(create_socket);
		if (closeRet < 0) {
			cout << "Error: could not close socket: " << closeRet << endl;
		}

	} else {
		cout << "Error: could not bind to socket." << endl;
	}

	return status;

}

pair<unsigned char*, int> NgSpiceLocalServer::receiveFromSocket(SOCKET sock) {

	msgSize = recv (sock, buffer, BUF-1, 0);
	if( msgSize > 0 && msgSize<=BUF) {
		buffer[msgSize] = '\0';

		return pair<unsigned char*, int> {buffer, (int)msgSize};
	} else {
		cout << "ERROR: received " << msgSize << " bytes..." << endl;
	}
	return pair<unsigned char*, int> {nullptr, 0};
}

int NgSpiceLocalServer::sendToSocket(SOCKET sock, string cborToSend) {


	return 0;
}

string NgSpiceLocalServer::handle_command_CBOR(unsigned char *cborIn, int len) {

	pair<string, void*> des = deserializeCBOR(cborIn, len);

	void* ret = handleCommand(des.first, des.second);

	return serializeCBOR(pair<string, void*> {"ret"+des.first, ret});
}


void* NgSpiceLocalServer::handleCommand(string command, void* data) {

	if (command == "Reset") {
		Reset();
	} else if (command == "Init") {
		int *ret = new int();
		*ret = Init();
		return ret;
	} // ...

	return nullptr;
}

string NgSpiceLocalServer::serializeCBOR(pair<string, void*> data) {

	// Preallocate the map structure
	cbor_item_t* vectorInfoToAdd = cbor_new_definite_map(7);

	if (data.first == "Init") {
		int retVal = *( (int*)data.second);

		// serialize into {"retInit", retVal}
		cbor_pair retInit = {
				.key   = cbor_move( cbor_build_string("retInit")          ) };
		if (retVal < 0) {
			retInit.value = cbor_move( cbor_build_negint32(retVal) );
		} else {
			retInit.value = cbor_move( cbor_build_uint32(retVal) );
		}
		cbor_map_add(vectorInfoToAdd, retInit);
	}

	/* Preallocate the map structure */
	cbor_item_t* root = cbor_new_definite_map(1);
	cbor_pair itemInRootMap = {
			.key   = cbor_move( cbor_build_string("ngSpiceMap") ),
			.value = cbor_move( vectorInfoToAdd ) };
	cbor_map_add(root, itemInRootMap );

	/* Output: `length` bytes of data in the `buffer` */
	unsigned char * buffer;
	size_t buffer_size,
	length = cbor_serialize_alloc(root, &buffer, &buffer_size);

	stringstream retBuf;
	for (size_t i=0; i<length; ++i) {
		retBuf << buffer[i];
	}

	return retBuf.str();
}

pair<string, void*> NgSpiceLocalServer::deserializeCBOR(unsigned char *cborIn, int len) {
	string ret = "";

	ser->initFromString(cborIn, len);

	cout << "successfully initialized from string" << endl;

	// check whether it is a valid ngspice message
	if (ser->isNgSpiceRelated()) {
		cout << "got a ngSpice-related message :)" << endl;

		// 2) apply appropriate deserialization




	} else {
		cout << "not ngspice-related" << endl;
	}

	ser->reset();

	return pair<string, void*> {ret, nullptr};
}



void NgSpiceLocalServer::Reset() {
	if (ngSpice != nullptr) {
		delete(ngSpice);
	}
	if (ser != nullptr) {
		delete(ser);
	}

	registeredForIOHandler = false;
	registeredForOnlineDataHandler = false;
	registeredForSimulationStatusHandler = false;
	registeredForExternalVoltageSource = false;
	registeredForExternalCurrentSource = false;
	registeredForExternalSynchronization = false;
}

int NgSpiceLocalServer::Init() {
	ngSpice = new NgSpice();
	ser = new CborSerialization();

	// setup all interactive functionality
	if (registeredForIOHandler              ) ngSpice->setIOHandler(this);
	if (registeredForOnlineDataHandler      ) ngSpice->setOnlineDataHandler(this);
	if (registeredForSimulationStatusHandler) ngSpice->setSimulationStatusHandler(this);
	if (registeredForExternalVoltageSource  ) ngSpice->setExternalVoltageSource(this);
	if (registeredForExternalCurrentSource  ) ngSpice->setExternalCurrentSource(this);
	if (registeredForExternalSynchronization) ngSpice->setExternalSynchronization(this);

	return 0;
}


void NgSpiceLocalServer::regUnregHandler(std::string handler, bool newState) {
	cout << "change status of " << handler << " to " << newState;
	if (handler == "IOHandler") {
		registeredForIOHandler = newState;
	} else if (handler == "OnlineDataHandler") {
		registeredForOnlineDataHandler = newState;
	} else if (handler == "SimulationStatusHandler") {
		registeredForSimulationStatusHandler = newState;
	} else if (handler == "ExternalVoltageSource") {
		registeredForExternalVoltageSource = newState;
	} else if (handler == "ExternalCurrentSource") {
		registeredForExternalCurrentSource = newState;
	} else if (handler == "ExternalSynchronization") {
		registeredForExternalSynchronization = newState;
	}
}




// IOHandler
void NgSpiceLocalServer::handleStdOut(std::string outMessage) {
	if (!registeredForIOHandler) {
		cout << "ERROR: why is handleStdOut called if registeredForIOHandler is false?" << endl;
		return;
	}

	cout << "stdout: '" << outMessage << "'" << endl;

	string request = serializeCBOR(pair<string, void*> {"handleStdOut", &outMessage});

	sendToSocket(mySocket, request);
}

void NgSpiceLocalServer::handleStdErr(std::string errMessage) {
	if (!registeredForIOHandler) {
		cout << "ERROR: why is handleStdErr called if registeredForIOHandler is false?" << endl;
		return;
	}

	cerr << "stderr: '" << errMessage << "'" << endl;
}

// OnlineDataHandler
void NgSpiceLocalServer::initDataStructures(pvecinfoall newVecInfoAll) {
	if (!registeredForOnlineDataHandler) {
		cout << "ERROR: why is initDataStructures called if registeredForOnlineDataHandler is false?" << endl;
		return;
	}

	this->OnlineDataHandler::initDataStructures(newVecInfoAll);

}

void NgSpiceLocalServer::addDataPoints(pvecvaluesall newVecValuesAll, int len) {
	if (!registeredForOnlineDataHandler) {
		cout << "ERROR: why is addDataPoints called if registeredForOnlineDataHandler is false?" << endl;
		return;
	}

	this->OnlineDataHandler::addDataPoints(newVecValuesAll, len);
	//cout << "add data points" << endl;

}

// SimulationStatusHandler
void NgSpiceLocalServer::handleSimulationStatus(std::string message) {
	if (!registeredForSimulationStatusHandler) {
		cout << "ERROR: why is handleSimulationStatus called if registeredForSimulationStatusHandler is false?" << endl;
		return;
	}

	cout << "simulation status: " << message << endl;

}

// ExternalVoltageSource
double NgSpiceLocalServer::getVoltage(std::string nodename, double time) {
	if (!registeredForExternalVoltageSource) {
		cout << "ERROR: why is getVoltage called if registeredForExternalVoltageSource is false?" << endl;
		return NAN;
	}

	cout << "requested voltage at " << time;

	pair<string, double> voltageRequest = pair<string, double> { nodename, time};

	string request = serializeCBOR(pair<string, void*> {"getVoltage", &voltageRequest});
	sendToSocket(mySocket, request);
	pair<unsigned char*, int> responseAndLen = receiveFromSocket(mySocket);

	double retVal = *((double*)deserializeCBOR(responseAndLen.first, responseAndLen.second).second);
	cout << " is " << retVal << endl;

	return retVal;
}

// ExternalCurrentSource
double NgSpiceLocalServer::getCurrent(std::string nodename, double time) {
	if (!registeredForExternalCurrentSource) {
		cout << "ERROR: why is getCurrent called if registeredForExternalCurrentSource is false?" << endl;
		return NAN;
	}

	cout << "requested current at " << time << endl;
	return 0.0;
}

// ExternalSynchronization
double NgSpiceLocalServer::synchronize(double ckttime, double olddelta, int redostep, int loc) {
	if (!registeredForExternalSynchronization) {
		cout << "ERROR: why is synchronize called if registeredForExternalSynchronization is false?" << endl;
		return NAN;
	}

	return this->ExternalSynchronization::synchronize(ckttime, olddelta, redostep, loc);
}

