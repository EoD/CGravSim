#ifndef _INC_GRAVITY_ERROR
#define _INC_GRAVITY_ERROR

#include "gDefines.h"
#include <bitset>

namespace error {
	using namespace std;
	extern bitset<ERROR_MAX> errors;
	const bitset<ERROR_MAX> unknown	( ERROR_TO_BITSET(ERROR_UNKNOWN) );
	const bitset<ERROR_MAX> file_in	( ERROR_TO_BITSET(ERROR_FILE_IN) );
	const bitset<ERROR_MAX> file_out( ERROR_TO_BITSET(ERROR_FILE_OUT) );
	const bitset<ERROR_MAX> calc	( ERROR_TO_BITSET(ERROR_CALC) );
}

#endif
