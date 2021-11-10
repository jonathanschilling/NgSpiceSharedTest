/*
 * OnlineDataHandler.cpp
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#include <OnlineDataHandler.h>

OnlineDataHandler::OnlineDataHandler() {
	reset();
}

OnlineDataHandler::~OnlineDataHandler() {

}

// to be called from within SendInitData
void OnlineDataHandler::initDataStructures(pvecinfoall newVecInfoAll) {
	reset();

	if (newVecInfoAll != NULL) {
		this->plotName  = newVecInfoAll->name;
		this->plotTitle = newVecInfoAll->title;
		this->plotDate  = newVecInfoAll->date;
		this->plotType  = newVecInfoAll->type;

		this->veccount = newVecInfoAll->veccount; // "No. of Data Columns" in ngspice app
		if (veccount>0) {
			cout << "No. of Data Columns : " << veccount << endl;
			vecNames.resize(veccount);
			isReal.resize(veccount);

			for (int i=0; i<veccount; ++i) {
				pvecinfo newVecInfo = newVecInfoAll->vecs[i];
				if (newVecInfo != NULL) {
					if ( newVecInfo->number == i ) {

						vecNames[i] = newVecInfo->vecname;
						isReal[i] = newVecInfo->is_real;

						// TODO: copy scale and data vector...

					} else {
						cout << "initDataStructures: mismatch between number in vecinfo and position in array: "
								<< i << " expected, but got " << newVecInfo->number << endl;
					}
				} else {
					cout << "initDataStructures: empty pvecinfo encountered at position " << i << endl;
					// skip or break the initialization?
					continue;
				}
			}

		} else {
			cout << "initDataStructures: no nodes in circuit? veccout=" << veccount << " from SendInitData" << endl;
		}

		isInitialized = true;
	} else {
		cout << "initDataStructures: got empty pvecinfoall from SendInitData" << endl;
	}
}

// to be called from within SendData
void OnlineDataHandler::addDataPoints(pvecvaluesall newVecValuesAll, int len) {
	if (!isInitialized) {
		cout << "addDataPoints ERROR: data handler is not initialized! Has SendInitData been connected to this handler as well?" << endl;
		return;
	}

	//cout << "len=" << len << endl;

	if (newVecValuesAll != NULL) {

		int vecCount = newVecValuesAll->veccount;
		//cout << "vecCount=" << vecCount << endl;

		//int vecIndex = newVecValuesAll->vecindex;
		//cout << "vecindex " << vecIndex << endl;

		//int scaleIndex = -1;

		vector<string> names;
		names.clear();
		names.resize(vecCount);

		vector<double> realValues;
		realValues.clear();
		realValues.resize(vecCount);

		vector<double> imagValues;
		imagValues.clear();
		imagValues.resize(vecCount);

		pvecvalues* newVecValues = newVecValuesAll->vecsa;
		if (newVecValues != NULL) {
			for (int i=0; i<vecCount; ++i) {

				names[i] = (*newVecValues)->name;
				//cout << "name at " << i << " is " << names[i] << endl;

//				if ( (*newVecValues)->is_scale ) {
//					scaleIndex = i;
//					//cout << "found scale vector at " << scaleIndex << endl;
//				}

				realValues[i] = (*newVecValues)->creal;
				if ( (*newVecValues)->is_complex ) {
					imagValues[i] = (*newVecValues)->cimag;
				}

				newVecValues++;
			}
		} else {
			cout << "addDataPoints: got empty pvecvalues from SendData" << endl;
		}
	} else {
		cout << "addDataPoints: got empty pvecvaluesall from SendData" << endl;
	}
}












