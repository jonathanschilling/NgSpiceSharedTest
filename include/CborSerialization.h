/*
 * CborSerialization.h
 *
 * using https://github.com/PJK/libcbor
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#ifndef CBORSERIALIZATION_H_
#define CBORSERIALIZATION_H_

// libcbor binary serialization
#include <cbor.h>

#include <ngspice/sharedspice.h>

// STL
#include <string>
#include <memory>
#include <fstream>
#include <iostream>

using namespace std;

/* Dvec flags from <ngspice/dvec.h> */
enum dvec_flags {
  VF_REAL      = (1 << 0), /* The data is real. */
  VF_COMPLEX   = (1 << 1), /* The data is complex. */
  VF_ACCUM     = (1 << 2), /* writedata should save this vector. */
  VF_PLOT      = (1 << 3), /* writedata should incrementally plot it. */
  VF_PRINT     = (1 << 4), /* writedata should print this vector. */
  VF_MINGIVEN  = (1 << 5), /* The v_minsignal value is valid. */
  VF_MAXGIVEN  = (1 << 6), /* The v_maxsignal value is valid. */
  VF_PERMANENT = (1 << 7)  /* Don't garbage collect this vector. */
};

class CborSerialization {
public:
	CborSerialization();
	virtual ~CborSerialization();

	int initFromString(unsigned char *cborIn, int len);
	void reset();

	bool isNgSpiceRelated();

	bool addVectorInfo(cbor_item_t* targetMap, string nameToAdd, pvector_info itemToAdd);

	bool writeToFile(pvector_info vec, string vecName, string filename);

private:

	cbor_item_t * map;
};

#endif /* CBORSERIALIZATION_H_ */
