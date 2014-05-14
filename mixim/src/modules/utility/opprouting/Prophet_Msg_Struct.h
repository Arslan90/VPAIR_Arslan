/*
 * Prophet_Msg_Struct.h
 *
 *  Created on: Apr 22, 2014
 *      Author: arslan
 */

#ifndef PROPHET_MSG_STRUCT_H_
#define PROPHET_MSG_STRUCT_H_

#include "Prophet_Msg_Enum.h"
#include <iostream>
#include <bitset>
#include "TlvData.h"

using namespace std;


typedef struct prophet_header {
	unsigned char result_field;
	unsigned char codes_field;
	int transaction_ID;
};

typedef struct prophet_tlv {
	unsigned char tlv_Type;
	std::bitset<8> tlv_Flags;
	std::list<Tlv_Data> tlv_Data;
};


#endif /* PROPHET_MSG_STRUCT_H_ */
