//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
/*
 * Arslane HAMZA CHERIF 10/02/2014
 */

#ifndef PROPHETV2_H_
#define PROPHETV2_H_
/**
 * iterator for std::map<LAddress::L3Type, double>
 */
#include <cassert>

#include <cobject.h>
#include "BaseNetwLayer.h"
#include "ProphetV2.h"
#include <map>
#include <queue>
#include <iterator>
#include <math.h>
#include "WaveShortMessage_m.h"
#include "NetwPkt_m.h"
#include "cmessage.h"
#include "NetwControlInfo.h"
#include "ArpInterface.h"
#include "SimpleAddress.h"

typedef std::map<LAddress::L3Type, double>::iterator map_it;

class ProphetV2: public BaseNetwLayer {
private:
	/** delivery predictability initialization constant*/
	double PEncMax;
	/** typical interconnection time in seconds*/
	double I_TYP;
	/** delivery predictability transitivity scaling constant default value */
	double Beta;
	/** delivery predictability aging constant */
	double GAMMA;

	/**
	 * Number of seconds in time unit
	 * How many seconds one time unit is when calculating aging of
	 * delivery predictions. Should be tweaked for the scenario.*/
	int secondsInTimeUnit;

	/** delivery predictabilities */
	std::map<LAddress::L3Type, double> preds;
	//std::map<DTNHost, double> preds;

	/** last encouter timestamp (sim)time */
	std::map<LAddress::L3Type, double> lastEncouterTime;
	//std::map<DTNHost, double> lastEncouterTime;

	/** last delivery predictability update (sim)time */
	double lastAgeUpdate;

	/** Fifo structure for WMS Storage*/
	std::queue<WaveShortMessage*> Bundles;

	/** Function for calculating P(A,B)*/
	void updateDeliveryPredsFor(const LAddress::L3Type BAdress);

	/** Function for calculating P(A,C) with P(A,B) & P(B,C)*/
	void updateTransitivePreds(const LAddress::L3Type BAdress,
			std::map<LAddress::L3Type, double> *Bpreds);

	/** Function for aging P(A,B)*/
	void ageDeliveryPreds();

	/** Function for updating & exchanging probabilities*/
	void update();

	/** Function to verify if the transmission will not fail*/
	void canITransmit();

	/** @brief Handle messages from upper layer */
	virtual void handleUpperMsg(cMessage* msg);

	/** @brief Handle messages from lower layer */
	virtual void handleLowerMsg(cMessage* msg);

	/** @brief Handle self messages */
	virtual void handleSelfMsg(cMessage* msg);

//	/** @brief Handle control messages from lower layer */
//	virtual void handleLowerControl(cMessage* msg);
//
//	/** @brief Handle control messages from lower layer */
//	virtual void handleUpperControl(cMessage* msg);

	/** @brief decapsulate higher layer message from NetwPkt */
	virtual cMessage* decapsMsg(NetwPkt*);

	/** @brief Encapsulate higher layer packet into an NetwPkt*/
	virtual NetwPkt* encapsMsg(cPacket*);

//	virtual cObject *const setDownControlInfo(cMessage *const pMsg, const LAddress::L2Type& pDestAddr);
//
//	virtual cObject *const setUpControlInfo(cMessage *const pMsg, const LAddress::L3Type& pSrcAddr);
public:
	virtual void initialize(int stage);
	virtual void finish();
protected:
	/**
	 * Enumeration for Queuing strategy defined by RFC 6693
	 */
	enum t_prophet_queuing {
		QUEUING_FIFO=1,
		QUEUING_MOFO,
		QUEUING_MOPR,
		QUEUING_LinearMOPR,
		QUEUING_SHLI,
		QUEUING_LEPR,
	};

	/**
	 * Enumeration for Forwarding strategy defined by RFC 6693
	 */
	enum t_prophet_forward {
		FWD_GRTR=1,
		FWD_GTMX,
		FWD_GTHR,
		FWD_GRTRplus,
		FWD_GTMXplus,
		FWD_GRTRsort,
		FWD_GRTRmax,
	};
};

#endif /* PROPHETV2_H_ */
