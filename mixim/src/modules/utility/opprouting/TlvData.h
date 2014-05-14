/*
 * TlvData.h
 *
 *  Created on: Apr 23, 2014
 *      Author: arslan
 */

#ifndef TLVDATA_H_
#define TLVDATA_H_

#include <bitset>
#include "simtime.h"
#include <omnetpp.h>
#include <map>
#include "SimpleAddress.h"

using namespace std;

class Tlv_Data {

};
class Tlv_Hello : public Tlv_Data {

public :
	Tlv_Hello();
	virtual ~Tlv_Hello();

private :
	unsigned int timer;
};

class Tlv_Error : public Tlv_Data {
public :
	Tlv_Error();
	virtual ~Tlv_Error();
};

class Tlv_RIBD : public Tlv_Data {
public :
	Tlv_RIBD();
	virtual ~Tlv_RIBD();
};

class Tlv_RIB : public Tlv_Data {
public :
	Tlv_RIB();
	virtual ~Tlv_RIB();

private :
	std::map<LAddress::L3Type, double > transmited_Preds;
};

class Tlv_Bndl_OfferResp : public Tlv_Data {
public :
	Tlv_Bndl_OfferResp();
	virtual ~Tlv_Bndl_OfferResp();

private :
	std::bitset<8> bndl_Flags;
	int senderAddress_var;
    int recipientAddress_var;
	int serial_var;
	simtime_t timestamp_var;
};

class Tlv_Bundle : public Tlv_Data {
public :
	Tlv_Bundle();
	virtual ~Tlv_Bundle();
};


#endif /* TLVDATA_H_ */
