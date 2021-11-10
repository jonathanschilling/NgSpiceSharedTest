/*
 * NgSpice.cpp
 *
 *  Object-oriented interface to ngspice
 *
 * Loosely based on ngspice.cpp (from the KiCad package)
 *  by Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *  and  Maciej Suminski <maciej.suminski@cern.ch>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * https://www.gnu.org/licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 *  Created on: 2018-10-14
 *      Author: Jonathan Schilling (jonathan.schilling@ipp.mpg.de)
 */

#include "NgSpice.h"

NgSpice::NgSpice() {

}

NgSpice::~NgSpice() {

	// TODO: exit spice background thread if it is still running

	if (ng_dll && !g_module_close(ng_dll)) {
		printf("g_module_close error: %s\n", g_module_error());
	}
}

bool NgSpice::Init(void) {
	bool ret = ensureInitialized();
	if (!ret) {
		cout << "Init ERROR: NgSpice could not be initialized!" << endl;
	}
	return ret;
}

int NgSpice::Command(const string& command) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		return (*m_ngSpice_Command)( (char*)command.c_str() );
	} else {
		cout << "Command ERROR: NgSpice could not be initialized!" << endl;
		return -1;
	}
}

bool NgSpice::isRunning(void) {
	if (ensureInitialized()) {
		return (*m_ngSpice_Running)();
	} else {
		cout << "isRunning ERROR: NgSpice could not be initialized!" << endl;
		return -1;
	}
}

pvector_info NgSpice::Get_Vec_Info(const string& vectorName) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		return (*m_ngGet_Vec_Info)( (char*)vectorName.c_str() );
	} else {
		cout << "Get_Vec_Info ERROR: NgSpice could not be initialized!" << endl;
		return NULL;
	}
}

int NgSpice::Circ(vector<string> circuitLines) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		vector<char*> cstrings;
		cstrings.reserve(circuitLines.size());

		for(size_t i=0; i<circuitLines.size(); ++i) {
			cstrings.push_back(const_cast<char*>(circuitLines[i].c_str()));
		}

		// ngspice wants to have a \0 at the end...
		cstrings.push_back(0);

		if(!cstrings.empty()) {
			return (*m_ngSpice_Circ)( &cstrings[0] );
		} else {
			cout << "Circ ERROR: empty circuit!" << endl;
			return -1;
		}
	} else {
		cout << "Circ ERROR: NgSpice could not be initialized!" << endl;
		return -1;
	}
}


// direct call to ngSpice_CurPlot
string NgSpice::CurPlot(void) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		return string( (*m_ngSpice_CurPlot)() );
	} else {
		cout << "CurPlot ERROR: NgSpice could not be initialized!" << endl;
		return "";
	}
}

// direct call to ngSpice_AllPlots
vector<string> NgSpice::AllPlots(void) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		int numPlots = 0;
		char** allPlots = (*m_ngSpice_AllPlots)();
		char** currentPlot = allPlots;

		// count plots
		while ( (*currentPlot) != 0 ) {
			numPlots++;
			currentPlot++;
		}

		if (numPlots > 0) {
			// reserve space in return vector
			vector<string> ret;
			ret.reserve(numPlots);

			// fill return vector
			currentPlot = allPlots;
			while ( (*currentPlot) != 0 ) {
				ret.push_back( *(currentPlot++) );
			}

			return ret;
		} else {
			cout << "AllPlots ERROR: no plots in current circuit!" << endl;
			return vector<string>();
		}
	} else {
		cout << "AllPlots ERROR: NgSpice could not be initialized!" << endl;
		return vector<string>();
	}
}

// direct call to ngSpice_AllVecs
vector<string> NgSpice::AllVecs(const string& plot) {

	// ngspice works correctly only with C locale
	LOCALE_IO c_locale;

	if (ensureInitialized()) {
		int numVecs = 0;
		char** allVecs = (*m_ngSpice_AllVecs)( (char*)plot.c_str() );
		char** currentVec = allVecs;

		// count vectors in requested plot
		while ( (*currentVec) != 0 ) {
			numVecs++;
			currentVec++;
		}

		if (numVecs > 0) {
			// reserve space in return vector
			vector<string> ret;
			ret.reserve(numVecs);

			// fill return vector
			currentVec = allVecs;
			while ( (*currentVec) != 0 ) {
				ret.push_back( *(currentVec++) );
			}

			return ret;
		} else {
			cout << "AllVecs ERROR: no vectors in plot '" << plot << "'" << endl;
			return vector<string>();
		}
	} else {
		cout << "AllVecs ERROR: NgSpice could not be initialized!" << endl;
		return vector<string>();
	}
}

// direct call to ngSpice_SetBkpt
bool NgSpice::SetBkpt(const double& timestamp) {
	if (ensureInitialized()) {
		// TODO: check whether background thread is running and halt it by "bg_halt" in that case
		return (*m_ngSpice_SetBkpt)(timestamp);
	} else {
		cout << "setBkpt ERROR: NgSpice could not be initialized!" << endl;
		return false;
	}
}

/////////////////////////
// convenience methods //
/////////////////////////

vector<Complex> NgSpice::GetPlot( const string& aName, int aMaxLen ) {

	vector<Complex> data;
	pvector_info vi = Get_Vec_Info( aName );

	if( vi ) {

		int length = aMaxLen < 0 ? vi->v_length : std::min( aMaxLen, vi->v_length );
		data.reserve( length );

		if( vi->v_realdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( Complex( vi->v_realdata[i], 0.0 ) );
			}
		} else if( vi->v_compdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( Complex( vi->v_compdata[i].cx_real, vi->v_compdata[i].cx_imag ) );
			}
		}
	}

	return data;
}


vector<double> NgSpice::GetRealPlot( const string& aName, int aMaxLen ) {

	vector<double> data;
	pvector_info vi = Get_Vec_Info( aName );

	if( vi ) {
		int length = aMaxLen < 0 ? vi->v_length : std::min( aMaxLen, vi->v_length );
		data.reserve( length );

		if( vi->v_realdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( vi->v_realdata[i] );
			}
		} else if( vi->v_compdata ) {
			for( int i = 0; i < length; i++ ) {
				assert( vi->v_compdata[i].cx_imag == 0.0 );
				data.push_back( vi->v_compdata[i].cx_real );
			}
		}
	}

	return data;
}


vector<double> NgSpice::GetImagPlot( const string& aName, int aMaxLen ) {

	vector<double> data;
	pvector_info vi = Get_Vec_Info( aName );

	if( vi ) {
		int length = aMaxLen < 0 ? vi->v_length : std::min( aMaxLen, vi->v_length );
		data.reserve( length );

		if( vi->v_compdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( vi->v_compdata[i].cx_imag );
			}
		}
	}

	return data;
}


vector<double> NgSpice::GetMagPlot( const string& aName, int aMaxLen ) {

	vector<double> data;
	pvector_info vi = Get_Vec_Info( aName );

	if( vi ) {
		int length = aMaxLen < 0 ? vi->v_length : std::min( aMaxLen, vi->v_length );
		data.reserve( length );

		if( vi->v_realdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( vi->v_realdata[i] );
			}
		} else if( vi->v_compdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( hypot( vi->v_compdata[i].cx_real, vi->v_compdata[i].cx_imag ) );
			}
		}
	}

	return data;
}


vector<double> NgSpice::GetPhasePlot( const string& aName, int aMaxLen ) {

	vector<double> data;
	pvector_info vi = Get_Vec_Info( aName );

	if( vi ) {
		int length = aMaxLen < 0 ? vi->v_length : std::min( aMaxLen, vi->v_length );
		data.reserve( length );

		if( vi->v_realdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( 0.0 );      // well, that's life
			}
		} else if( vi->v_compdata ) {
			for( int i = 0; i < length; i++ ) {
				data.push_back( atan2( vi->v_compdata[i].cx_imag, vi->v_compdata[i].cx_real ) );
			}
		}
	}

	return data;
}

bool NgSpice::Run() {

	// bg_* commands execute in a separate thread
	return Command( "bg_run" );
}


bool NgSpice::Stop() {

	// bg_* commands execute in a separate thread
	return Command( "bg_halt" );
}





// load dll, init function pointers, init spice
bool NgSpice::ensureInitialized() {

	// re-initialize if error occured
	if( errorOccured ) {
		isInitialized = false;
	}

	// immediately return if there is nothing to do
	if (isInitialized) {
		return true;
	}

	// ngspice works correctly only with C locale (?)
	LOCALE_IO c_locale;

	// check if module loading via glib-2.0 is supported
	gboolean moduleLoadingSupported = g_module_supported();
	if (!moduleLoadingSupported) {
		cout << "ERROR: GModule loading not supported on this platform!" <<endl;
		return false;
	}

	// open dll
	// TODO: search further than linux system library dir
	std::string dllFilename = "libngspice";
	ng_dll = g_module_open(dllFilename.c_str(), G_MODULE_BIND_LAZY);
	if (!ng_dll) {
		printf("g_module_open error: %s\n", g_module_error() );
		return false;
	}
	//cout << "found shared library: " << g_module_name(ng_dll) << endl;

	// look for symbols and init function pointers
	if (       !g_module_symbol(ng_dll, "ngSpice_Init",      (funptr_t*) &m_ngSpice_Init      )
			|| !g_module_symbol(ng_dll, "ngSpice_Init_Sync", (funptr_t*) &m_ngSpice_Init_Sync )
			|| !g_module_symbol(ng_dll, "ngSpice_Command",   (funptr_t*) &m_ngSpice_Command   )
			|| !g_module_symbol(ng_dll, "ngGet_Vec_Info",    (funptr_t*) &m_ngGet_Vec_Info    )
			|| !g_module_symbol(ng_dll, "ngSpice_Circ",      (funptr_t*) &m_ngSpice_Circ      )
			|| !g_module_symbol(ng_dll, "ngSpice_CurPlot",   (funptr_t*) &m_ngSpice_CurPlot   )
			|| !g_module_symbol(ng_dll, "ngSpice_AllPlots",  (funptr_t*) &m_ngSpice_AllPlots  )
			|| !g_module_symbol(ng_dll, "ngSpice_AllVecs",   (funptr_t*) &m_ngSpice_AllVecs   )
			|| !g_module_symbol(ng_dll, "ngSpice_running",   (funptr_t*) &m_ngSpice_Running   ) // maybe rename to ngSpice_Running ...
			|| !g_module_symbol(ng_dll, "ngSpice_SetBkpt",   (funptr_t*) &m_ngSpice_SetBkpt   ) ) {

		printf("g_module_symbol error: %s\n", g_module_error() );
		if (!g_module_close(ng_dll)) {
			printf("g_module_close error: %s\n", g_module_error() );
		}
		return false;
	}

	// check whether initalization of function pointers was successful
	bool allFound = true;
	if (m_ngSpice_Init      == NULL) { allFound=false; printf("ERROR: symbol ngSpice_Init was not found in %s\n",      dllFilename.c_str()); }
	if (m_ngSpice_Init_Sync == NULL) { allFound=false; printf("ERROR: symbol ngSpice_Init_Sync was not found in %s\n", dllFilename.c_str()); }
	if (m_ngSpice_Command   == NULL) { allFound=false; printf("ERROR: symbol ngSpice_Command was not found in %s\n",   dllFilename.c_str()); }
	if (m_ngGet_Vec_Info    == NULL) { allFound=false; printf("ERROR: symbol ngGet_Vec_Info was not found in %s\n",    dllFilename.c_str()); }
	if (m_ngSpice_Circ      == NULL) { allFound=false; printf("ERROR: symbol ngSpice_Circ was not found in %s\n",      dllFilename.c_str()); }
	if (m_ngSpice_CurPlot   == NULL) { allFound=false; printf("ERROR: symbol ngSpice_CurPlot was not found in %s\n",   dllFilename.c_str()); }
	if (m_ngSpice_AllPlots  == NULL) { allFound=false; printf("ERROR: symbol ngSpice_AllPlots was not found in %s\n",  dllFilename.c_str()); }
	if (m_ngSpice_AllVecs   == NULL) { allFound=false; printf("ERROR: symbol ngSpice_AllVecs was not found in %s\n",   dllFilename.c_str()); }
	if (m_ngSpice_Running   == NULL) { allFound=false; printf("ERROR: symbol ngSpice_running was not found in %s\n",   dllFilename.c_str()); }
	if (m_ngSpice_SetBkpt   == NULL) { allFound=false; printf("ERROR: symbol ngSpice_SetBkpt was not found in %s\n",   dllFilename.c_str()); }
	if (!allFound) {
		if (!g_module_close(ng_dll)) {
			printf("g_module_close error: %s\n", g_module_error() );
		}
		return false;
	}

	// init shared library and setup callback pointers
	m_ngSpice_Init( &cb_SendChar, cb_SendStat, &cb_ControlledExit, &cb_SendData, &cb_SendInitData, &cb_BGThreadRunning, this );
	m_ngSpice_Init_Sync( &cb_GetVSRCData, &cb_GetISRCData, &cb_GetSyncData, &id, this );

	// if we finally got up to here, consider the class as initialized
	isInitialized = true;
	errorOccured = false;

	return true;
}

void NgSpice::controlledExit(int status, bool immediate, bool exit_upon_quit) {
	cout << "ERROR: controlled exit, status=" << status << ", immediate=" << immediate << ", exit_upon_quit=" << exit_upon_quit << endl;
	errorOccured = true;
}

void NgSpice::bgThreadRunning(bool isRunning) {
	cout << "bgThreadRunning notification: isRunning=" << isRunning << endl;

	// TODO: handle notification about running background thread
}


///////////////////
//   CALLBACKS   //
///////////////////

int NgSpice::cb_SendChar(char* what, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasIOHandler() && sim->ioHandler != NULL) {
			// if available, send messages to appropriate channels
			if( strncasecmp(what, "stdout ", 7) == 0 ) {
				what += 7; // skip "stdout " part
				sim->ioHandler->handleStdOut(std::string(what));
			} else if ( strncasecmp(what, "stderr ", 7) == 0 ) {
				what += 7; // skip "stderr " part
				sim->ioHandler->handleStdErr(std::string(what));
			} else {
				// default is stdout ...
				sim->ioHandler->handleStdOut(std::string(what));
				// ... and a warning to the server admin
				cout << "cb_SendChar ERROR: target (stdout or strerr) was not specified by ngSpice for message '" << what << "'" << endl;
			}
		} else if (sim->hasIOHandler()) {
			cout << "cb_SendChar ERROR: IO handler not found!" << endl;
		}
	} else {
		cout << "cb_SendChar ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_SendStat(char* what, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasSimulationStatusHandler() && sim->simulationStatusHandler != NULL) {
			// if possible, handle simulation status
			sim->simulationStatusHandler->handleSimulationStatus(std::string(what));
		} else if (sim->hasSimulationStatusHandler()) {
			cout << "cb_SendStat ERROR: simulation status message handler not found!" << endl;
		}
	} else {
		cout << "cb_SendStat ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_ControlledExit (int status, bool immediate, bool exit_upon_quit, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>(userptr);
	if (sim != NULL) {
		// if possible, handle controlled exit
		sim->controlledExit(status, immediate, exit_upon_quit);
	} else {
		cout << "cb_ControlledExit ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_SendData(pvecvaluesall curvecvaluesall, int len, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>(userptr);
	if (sim != NULL) {
		if (sim->hasOnlineDataHandler() && sim->onlineDataHandler != NULL) {
			// if possible, handle new data points
			sim->onlineDataHandler->addDataPoints(curvecvaluesall, len);
		} else if (sim->hasOnlineDataHandler()) {
			cout << "cb_SendData ERROR: data point handler not found!" << endl;
		}
	} else {
		cout << "cb_SendData ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_SendInitData(pvecinfoall curvecinfoall, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasOnlineDataHandler() && sim->onlineDataHandler != NULL) {
			// if possible, handle data structure initilization
			sim->onlineDataHandler->initDataStructures(curvecinfoall);
		} else if (sim->hasOnlineDataHandler()) {
			cout << "cb_SendInitData ERROR: data initialization handler not found!" << endl;
		}
	} else {
		cout << "cb_SendInitData ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_BGThreadRunning(bool is_running, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		// if possible, handle notification about running background thread
		sim->bgThreadRunning(is_running);
	} else {
		cout << "cb_BGThreadRunning ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_GetVSRCData(double* vval, double time, char* vname, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasExternalVoltageSource() && sim->externalVoltageSource != NULL) {
			// if available, evaluate external voltage source
			(*vval) = sim->externalVoltageSource->getVoltage(std::string(vname), time);
		} else if (sim->hasExternalVoltageSource()) {
			cout << "cb_GetVSRCData ERROR: external voltage source not found!" << endl;
		}
	} else {
		cout << "cb_GetVSRCData ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_GetISRCData(double* ival, double time, char* iname, int ng_ident, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasExternalCurrentSource() && sim->externalCurrentSource != NULL) {
			// if available, evaluate external current source
			(*ival) = sim->externalCurrentSource->getCurrent(std::string(iname), time);
		} else if (sim->hasExternalCurrentSource()) {
			cout << "cb_GetISRCData ERROR: external current source not found!" << endl;
		}
	} else {
		cout << "cb_GetISRCData ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

int NgSpice::cb_GetSyncData(double ckttime, double* pcktdelta, double olddelta, int redostep, int ng_ident, int loc, void* userptr) {

	NgSpice* sim = reinterpret_cast<NgSpice*>( userptr );
	if (sim != NULL) {
		if (sim->hasExternalSynchronization() && sim->externalSynchronization != NULL) {
			// if available, evaluate external synchronization
			(*pcktdelta) = sim->externalSynchronization->synchronize(ckttime, olddelta, redostep, loc);
		} else if (sim->hasExternalSynchronization()) {
			cout << "cb_GetSyncData ERROR: external synchronization not found!" << endl;
		}
	} else {
		cout << "cb_GetSyncData ERROR: pointer to NgSpice object not available!" << endl;
	}

	return 0;
}

