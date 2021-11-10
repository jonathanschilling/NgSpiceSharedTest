/*
 * NgSpice.h
 *
 *	Object-oriented interface to ngspice
 *
 *	Loosely based on ngspice.h (from the KiCad package)
 *	by Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *	and  Maciej Suminski <maciej.suminski@cern.ch>.
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

#ifndef NGSPICE_H_
#define NGSPICE_H_


// module loading support by glib-2.0
#include <glib-2.0/gmodule.h>

// ngspice shared library interface
#include <ngspice/sharedspice.h>

// C++ can handle complex numbers :-)
#include <complex>
typedef std::complex<double> Complex;

// for LOCALE_IO class from KiCad: always use "C" locale
#include "common.h"

#include "IOHandler.h"               // for SendChar
#include "SimulationStatusHandler.h" // for SendStat
#include "OnlineDataHandler.h"       // for SendInitData, SendData
#include "ExternalVoltageSource.h"   // for GetVSRCData
#include "ExternalCurrentSource.h"   // for GetISRCData
#include "ExternalSynchronization.h" // for GetSyncData

// STL includes
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// object-oriented interface to ngspice shared library
class NgSpice {

public:
	NgSpice();
	virtual ~NgSpice();

	///////////////////////////
	//  public control API   //
	///////////////////////////

	// direct call to ensureInitialized for setting up environment and getting a clean stdout startup
	bool Init(void);

	// direct call to ngSpice_Command
	int Command(const string& command);

	// direct call to ngSpice_running
	bool isRunning(void);

	// direct call to ngGet_Vec_Info
	pvector_info Get_Vec_Info(const string& vectorName);

	// direct call to ngSpice_Circ
	int Circ(vector<string> circuitLines);

	// direct call to ngSpice_CurPlot
	string CurPlot(void);

	// direct call to ngSpice_AllPlots
	vector<string> AllPlots(void);

	// direct call to ngSpice_AllVecs
	vector<string> AllVecs(const string& plot);

	// direct call to ngSpice_SetBkpt
	bool SetBkpt(const double& timestamp);


	///////////////////////////
	//  convenience methods  //
	///////////////////////////

	vector<Complex> GetPlot     ( const string& aName, int aMaxLen );
	vector<double>  GetRealPlot ( const string& aName, int aMaxLen );
	vector<double>  GetImagPlot ( const string& aName, int aMaxLen );
	vector<double>  GetMagPlot  ( const string& aName, int aMaxLen );
	vector<double>  GetPhasePlot( const string& aName, int aMaxLen );

	bool Run(void);
	bool Stop(void);











	//////////////////
	//   Handlers   //
	//////////////////

	// IO handling
	bool _hasIOHandler = false;
	void setIOHandler(IOHandler* newIOHandler) {
		this->ioHandler = newIOHandler;
		_hasIOHandler = (this->ioHandler != NULL);
	}
	IOHandler* getIOHandler() { return this->ioHandler; }
	bool       hasIOHandler() { return this->_hasIOHandler; }

	// handling of simulation status messages
	bool _hasSimulationStatusHandler = false;
	void setSimulationStatusHandler(SimulationStatusHandler* newSimStatHandler) {
		this->simulationStatusHandler = newSimStatHandler;
		_hasSimulationStatusHandler = (this->simulationStatusHandler != NULL);
	}
	SimulationStatusHandler* getSimulationStatusHandler() { return this->simulationStatusHandler; }
	bool                     hasSimulationStatusHandler() { return this->_hasSimulationStatusHandler; }

	// handling of online results from the simulation
	bool _hasOnlineDataHandler = false;
	void setOnlineDataHandler(OnlineDataHandler* newOnlineDataHandler) {
		this->onlineDataHandler = newOnlineDataHandler;
		_hasOnlineDataHandler = (this->onlineDataHandler != NULL);
	}
	OnlineDataHandler* getOnlineDataHandler() { return this->onlineDataHandler; }
	bool               hasOnlineDataHandler() { return this->_hasOnlineDataHandler; }

	// external voltage sources
	bool _hasExternalVoltageSource = false;
	void setExternalVoltageSource(ExternalVoltageSource* newVoltageSource) {
		this->externalVoltageSource = newVoltageSource;
		_hasExternalVoltageSource = (this->externalVoltageSource != NULL);
	}
	ExternalVoltageSource* getExternalVoltageSource() { return this->externalVoltageSource; }
	bool                   hasExternalVoltageSource() { return this->_hasExternalVoltageSource; }

	// external current sources
	bool _hasExternalCurrentSource = false;
	void setExternalCurrentSource(ExternalCurrentSource* newCurrentSource) {
		this->externalCurrentSource = newCurrentSource;
		_hasExternalCurrentSource = (this->externalCurrentSource != NULL);
	}
	ExternalCurrentSource* getExternalCurrentSource() { return this->externalCurrentSource; }
	bool                   hasExternalCurrentSource() { return this->_hasExternalCurrentSource; }

	// external synchronization
	bool _hasExternalSynchronization = false;
	void setExternalSynchronization(ExternalSynchronization* newSync) {
		this->externalSynchronization = newSync;
		_hasExternalSynchronization = (this->externalSynchronization != NULL);
	}
	ExternalSynchronization* getExternalSynchronization() { return this->externalSynchronization; }
	bool                     hasExternalSynchronization() { return this->_hasExternalSynchronization; }





	// ngspice-internal complex type to std::complex type
	static Complex n2stl_cmplx(const ngcomplex_t& in) {
		return Complex(in.cx_real, in.cx_real);
	}

	// ngspice-internal complex type to std::complex type
	static void n2stl_cmplx(const ngcomplex_t& in, Complex* ret) {
		(*ret).real(in.cx_real);
		(*ret).imag(in.cx_real);
	}

	// std::complex type to ngspice-internal complex type
	static ngcomplex_t stl2n_cmplx(const Complex& in) {
		ngcomplex_t ret = { .cx_real=in.real(), .cx_imag = in.imag() };
		return ret;
	}

	// std::complex type to ngspice-internal complex type
	static void stl2n_cmplx(const Complex& in, ngcomplex_t* ret) {
		(*ret).cx_real = in.real();
		(*ret).cx_imag = in.imag();
	}




private:

	void validate(void);

	// load dll, setup function pointers, init ngspice
	// returns true if initialized, false if any error occured
	bool ensureInitialized();

	// controlled exit request from the dll
	void controlledExit(int status, bool immediate, bool exit_upon_quit);

	// notification about active background thread
	void bgThreadRunning(bool isRunning);

	// ngspice shared lib
	GModule *ng_dll = NULL;
	bool isInitialized = false;
	bool errorOccured = false;
	int id = 0;

	// ngspice library functions
	typedef void         (*funptr_t);
	typedef void         (*ngSpice_Init)      ( SendChar*, SendStat*, ControlledExit*, SendData*, SendInitData*, BGThreadRunning*, void* );
	typedef int          (*ngSpice_Init_Sync) ( GetVSRCData*, GetISRCData*, GetSyncData*, int*, void* );
	typedef int          (*ngSpice_Command)   ( char*  );
	typedef pvector_info (*ngGet_Vec_Info)    ( char*  );
	typedef int          (*ngSpice_Circ)      ( char** );
	typedef char*        (*ngSpice_CurPlot)   ( void   );
	typedef char**       (*ngSpice_AllPlots)  ( void   );
	typedef char**       (*ngSpice_AllVecs)   ( char*  );
	typedef bool         (*ngSpice_Running)   ( void   );
	typedef bool         (*ngSpice_SetBkpt)   ( double );

	// handles to DLL functions in this module
	ngSpice_Init      m_ngSpice_Init      = NULL;
	ngSpice_Init_Sync m_ngSpice_Init_Sync = NULL;
	ngSpice_Command   m_ngSpice_Command   = NULL;
	ngGet_Vec_Info    m_ngGet_Vec_Info    = NULL; // maybe rename to ngSpice_Get_Vec_Info at some point...
	ngSpice_Circ      m_ngSpice_Circ      = NULL;
	ngSpice_CurPlot   m_ngSpice_CurPlot   = NULL;
	ngSpice_AllPlots  m_ngSpice_AllPlots  = NULL;
	ngSpice_AllVecs   m_ngSpice_AllVecs   = NULL;
	ngSpice_Running   m_ngSpice_Running   = NULL;
	ngSpice_SetBkpt   m_ngSpice_SetBkpt   = NULL;

	// callback functions
	// the last one probably should have ng_ident and loc exchanged ...
	static int cb_SendChar       ( char* what,                                                       int ng_ident,          void* userptr );
	static int cb_SendStat       ( char* what,                                                       int ng_ident,          void* userptr );
	static int cb_ControlledExit ( int status, bool immediate, bool exit_upon_quit,                  int ng_ident,          void* userptr );
	static int cb_SendData       ( pvecvaluesall curvecvaluesall, int len,                           int ng_ident,          void* userptr );
	static int cb_SendInitData   ( pvecinfoall   curvecinfoall,                                      int ng_ident,          void* userptr );
	static int cb_BGThreadRunning( bool is_running,                                                  int ng_ident,          void* userptr );
	static int cb_GetVSRCData    ( double* vval, double time, char* vname,                           int ng_ident,          void* userptr );
	static int cb_GetISRCData    ( double* ival, double time, char* iname,                           int ng_ident,          void* userptr );
	static int cb_GetSyncData    ( double ckttime, double* pcktdelta, double olddelta, int redostep, int ng_ident, int loc, void* userptr );

	IOHandler*               ioHandler               = NULL; // for SendChar
	SimulationStatusHandler* simulationStatusHandler = NULL; // for SendStat
	OnlineDataHandler*       onlineDataHandler       = NULL; // for SendInitData, SendData
	ExternalVoltageSource*   externalVoltageSource   = NULL; // for GetVSRCData
	ExternalCurrentSource*   externalCurrentSource   = NULL; // for GetISRCData
	ExternalSynchronization* externalSynchronization = NULL; // for GetSyncData
};

#endif /* NGSPICE_H_ */
