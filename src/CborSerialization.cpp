/*
 * CborSerialization.cpp
 *
 *  Created on: 15.10.2018
 *      Author: jonathan
 */

#include <CborSerialization.h>

CborSerialization::CborSerialization() {
	reset();
}

CborSerialization::~CborSerialization() {

}

int CborSerialization::initFromString(unsigned char *cborIn, int len) {

	cout << "init CborSerialization from string '" << cborIn << "'" << endl;

	struct cbor_load_result result;

	this->map = cbor_load(cborIn, len, &result);

	if (result.error.code != CBOR_ERR_NONE) {
		printf("There was an error (%d) while reading the input near byte %zu (read %zu bytes in total): ",
				result.error.code, result.error.position, result.read);

		return result.error.code;
	} else {
		cout << "check if we got a map with one entry..." << endl;
		// check that input CBOR is a map containing one pair
		if (   cbor_typeof(map) != CBOR_TYPE_MAP
				|| !cbor_isa_map(map)
				|| cbor_map_size(map) != 1) {

			return -1;
		}
	}
	return 0;
}

void CborSerialization::reset() {
	if (this->map != nullptr) {
		delete(this->map);
	}
	this->map = nullptr;
}

bool CborSerialization::isNgSpiceRelated() {
	cbor_pair *mainMap = cbor_map_handle(map);
	return (string((char*)cbor_string_handle(mainMap->key)) == "ngSpiceMap");
}



bool CborSerialization::addVectorInfo(cbor_item_t* targetMap, string nameToAdd, pvector_info itemToAdd) {

	bool success = true;

	// Preallocate the map structure
	cbor_item_t* vectorInfoToAdd = cbor_new_definite_map(7);

	// for debugging, first element should always be a string "####classname####"
	// which is compared against the expected type to prohibit possible misinterpretations
	cbor_pair classname = {
			.key   = cbor_move( cbor_build_string("####classname####")          ),
			.value = cbor_move( cbor_build_string("pvector_info") ) };
	success &= cbor_map_add(vectorInfoToAdd, classname);


	cbor_pair v_name = {
			.key   = cbor_move( cbor_build_string("v_name")          ),
			.value = cbor_move( cbor_build_string(itemToAdd->v_name) ) };
	success &= cbor_map_add(vectorInfoToAdd, v_name);

	cbor_pair v_type = {
			.key   = cbor_move( cbor_build_string("v_type")          ) };
	if (itemToAdd->v_type < 0) {
		v_type.value = cbor_move( cbor_build_negint32(itemToAdd->v_type) );
	} else {
		v_type.value = cbor_move( cbor_build_uint32(itemToAdd->v_type) );
	}
	success &= cbor_map_add(vectorInfoToAdd, v_type);

	cbor_pair v_flags = {
			.key   = cbor_move( cbor_build_string("v_flags")          ) };
	if (itemToAdd->v_flags < 0) {
		v_flags.value = cbor_move( cbor_build_negint32(itemToAdd->v_flags) );
	} else {
		v_flags.value = cbor_move( cbor_build_uint32(itemToAdd->v_flags) );
	}
	success &= cbor_map_add(vectorInfoToAdd, v_flags);

	cbor_item_t* realArr = cbor_new_definite_array(itemToAdd->v_length);
	cbor_item_t* imagArr = NULL;

	if (itemToAdd->v_flags & VF_REAL) {
		// real data

		// minimal overhead, but reliefs from distinguishing between maps
		// which contain real or imaginary data
		imagArr = cbor_new_definite_array(0);

		for (int i=0; i<itemToAdd->v_length; ++i) {
			success &= cbor_array_set(realArr, i, cbor_move( cbor_build_float8(itemToAdd->v_realdata[i]) ) );
		}
	} else if (itemToAdd->v_flags & VF_COMPLEX) {
		//cout << "complex data for vector '" << itemToAdd->v_name << "'" << endl;

		imagArr = cbor_new_definite_array(itemToAdd->v_length);

		// build CBOR arrays
		for (int i=0; i<itemToAdd->v_length; ++i) {
			success &= cbor_array_set(realArr, i, cbor_build_float8(itemToAdd->v_compdata[i].cx_real) );
			success &= cbor_array_set(imagArr, i, cbor_build_float8(itemToAdd->v_compdata[i].cx_imag) );
		}
	}

	// add vector of imaginary component
	cbor_pair v_realdata = {
			.key   = cbor_move( cbor_build_string("v_realdata") ),
			.value = cbor_move( realArr ) };
	success &= cbor_map_add(vectorInfoToAdd, v_realdata);

	// add vector of imaginary component
	cbor_pair v_compdata = {
			.key   = cbor_move( cbor_build_string("v_imagdata") ),
			.value = cbor_move( imagArr ) };
	success &= cbor_map_add(vectorInfoToAdd, v_compdata);

	// number of timestamps in this vector
	if (itemToAdd->v_length < 0) {
		cout << "ERROR: v_length was negative for vector " << itemToAdd->v_name << ": " << itemToAdd->v_length << endl;
	}
	cbor_pair v_length = {
			.key   = cbor_move( cbor_build_string("v_length")       ),
			.value = cbor_move( cbor_build_uint32(itemToAdd->v_length) ) };
	success &= cbor_map_add(vectorInfoToAdd, v_length);

	// put into root map
	cbor_pair itemInRootMap = {
			.key   = cbor_move( cbor_build_string(nameToAdd.c_str()) ),
			.value = cbor_move( vectorInfoToAdd ) };
	success &= cbor_map_add(targetMap, itemInRootMap );

	if (!success) {
		cout << "ERROR in serialization of pvector_info named '" << itemToAdd->v_name
				<< "' into CBOR named " << nameToAdd << endl;
	}

	return success;
}



bool CborSerialization::writeToFile(pvector_info vec, string vecName, string filename) {
	bool success = true;

	/* Preallocate the map structure */
	cbor_item_t* root = cbor_new_definite_map(1);

	// add vector to root map with name "vector"
	success &= addVectorInfo(root, vecName, vec);

	/* Output: `length` bytes of data in the `buffer` */
	unsigned char * buffer;
	size_t buffer_size,
	length = cbor_serialize_alloc(root, &buffer, &buffer_size);
	//cout << " needed " << buffer_size << " for serialization" << endl;
	//cout << " resulting file has " << length << " bytes" << endl;


	ofstream file (filename, ios::out|ios::binary);
	if (file.is_open()) {
		file.write((char*)buffer, length);
		file.close();
		//cout << "successfully written :)" << endl;
	} else {
		success = false;
		cout << "Unable to write file " << filename << endl;
	}

	free(buffer);

	cbor_decref(&root);

	return success;
}
