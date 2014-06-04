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

#include "ProphetV2.h"


Define_Module(ProphetV2);

void ProphetV2::initialize(int stage)
{
	BaseNetwLayer::initialize(stage);
	if (stage==0){
		PEncMax = par("PEncMax").doubleValue();
		I_TYP = par("I_TYP").doubleValue();
		Beta = par("Beta").doubleValue();
		GAMMA = par("GAMMA").doubleValue();
		secondsInTimeUnit = par("secondsInTimeUnit");
		int fwd = par("fwdStrategy");
		fwdStrategy = static_cast<t_prophet_forward>(fwd);
		int q = par("qStrategy");
		qStrategy = static_cast<t_prophet_queuing>(q);
		/*
		 * L3Address will be initialized by BaseNetwLayer::initialize(1);
		 */
//		myNetwAddr = LAddress::L3Type( getId() );
		lastAgeUpdate = 0;
		preds = std::map<LAddress::L3Type,double>();
		lastEncouterTime = std::map<LAddress::L3Type,double>();
		bundles = std::list<WaveShortMessage*>();
		mapsForBundles = std::multimap<LAddress::L3Type,WaveShortMessage*>();
		int tmp = getThisNic()->getId();
//		listener = new NeighborhoodManager(this,tmp);
		listener = new NeighborhoodManager(this, tmp);
	}
	else if (stage==1){
		listener->setSrcAdresse(myNetwAddr);
		simulation.getSystemModule()->subscribe("Connect", listener);
		simulation.getSystemModule()->subscribe("Disconnect", listener);
	}
}
void ProphetV2::updateDeliveryPredsFor(const LAddress::L3Type BAdress)
{
	double PEnc,lastEncTime, predsForB;
	/*
	 * before calculating predsForB, we must age preds.
	 */
	ageDeliveryPreds();
	double encTime = simTime().dbl();
	map_it it= preds.find(BAdress);
	map_it it2= lastEncouterTime.find(BAdress);
	if (it==preds.end()){
		/*
		 * if iterator is equal map.end(), it means that there is no entry for BAdress in preds
		 * so lastEncTime equal 0
		 */
		predsForB = 0;
		lastEncTime = 0;
		PEnc = PEncMax;

	}else {
		/*
		 * if iterator is not equal map.end(), it means that there is an entry for BAdress in preds
		 */
		predsForB = it->second;
		if (it2==lastEncouterTime.end()){
			lastEncTime = 0;
		}else {
			lastEncTime = it2->second;
		}
		;
		if (simTime().dbl()-lastEncTime<I_TYP){
			/*
			 * if the node has been encountered recently then don't use PEncMax
			 */
			PEnc = PEncMax * (( simTime().dbl()-lastEncTime)/I_TYP);
		}else {
			/*
			 * if the node has been encountered long ago then use PEncMax
			 */
			PEnc = PEncMax;
		}
	}
	predsForB = predsForB + (1-predsForB)*PEnc;
	preds.insert(std::pair<LAddress::L3Type,double>(BAdress,predsForB));
	lastEncouterTime.insert(std::pair<LAddress::L3Type,double>(BAdress,encTime));

}


void ProphetV2::updateTransitivePreds(const LAddress::L3Type BAdress, std::map<LAddress::L3Type,double> *Bpreds)
{

	double predsForB,predsForC,BpredsForC;
	LAddress::L3Type CAdress;
	map_it tmp_it;
	/*
	 * before calculating transitive predictions, we must age preds.
	 */
	ageDeliveryPreds();
	for (map_it it=Bpreds->begin(); it!=Bpreds->end();it++){
		if (it->first==myNetwAddr)
			continue;

		CAdress = it->first;
		tmp_it= preds.find(CAdress);
		predsForC=tmp_it->second;

		tmp_it= Bpreds->find(CAdress);
		BpredsForC=tmp_it->second;

		tmp_it= preds.find(BAdress);
		predsForB=tmp_it->second;

		if (predsForC<(predsForB*BpredsForC*Beta)){
			predsForC = predsForB*BpredsForC*Beta;
		}
		preds.insert(std::pair<LAddress::L3Type,double>(CAdress,predsForC));
	}
}

void ProphetV2::ageDeliveryPreds()
{
	double timeDiff = (simTime().dbl()-lastAgeUpdate)/secondsInTimeUnit;
	if (timeDiff==0){
		return;
	}else {
		double mult = std::pow(GAMMA, timeDiff);
		for (map_it it=preds.begin();it!=preds.end();it++){
			it->second = it->second * mult;
		}
		lastAgeUpdate = simTime().dbl();
	}
}

void ProphetV2::update()
{
}



bool ProphetV2::canITransmit()
{
//	if (listener->)
	return listener->isAnyNeighbor();
	//	cIListener *listener = new cListener();

	//

	//	while (cc->getParentModule()!=NULL){
	//		cc = cc->getParentModule();
	//	}
	//	ConnectionManager *cM = NULL;
	//	cM = (ConnectionManager *)(cc);
	//	if (nic!=NULL){
	//		cM->getGateList(nic->getId());
	//	}

}
cModule* ProphetV2::getThisNic(){

	cModule *cc = this->getParentModule();
	cModule *nic = NULL;
	if (cc!=NULL){
			nic = cc->getSubmodule("nic");
	}
	return nic;
}



cMessage* ProphetV2::decapsMsg(NetwPkt *msg)
{
    cMessage *m = msg->decapsulate();
//    setUpControlInfo(m, msg->getSrcAddr());
    // delete the netw packet
//    delete msg;
    return m;
}


NetwPkt* ProphetV2::encapsMsg(cPacket *appPkt)
{
	LAddress::L2Type macAddr;
	LAddress::L3Type netwAddr;

	coreEV <<"in encaps...\n";

	NetwPkt *pkt = new NetwPkt(appPkt->getName(), appPkt->getKind());
	pkt->setBitLength(headerLength);

	cObject* cInfo = appPkt->removeControlInfo();

	if(cInfo == NULL){
	EV << "warning: Application layer did not specifiy a destination L3 address\n"
	   << "\tusing broadcast address instead\n";
	netwAddr = LAddress::L3BROADCAST;
	} else {
	coreEV <<"CInfo removed, netw addr="<< NetwControlInfo::getAddressFromControlInfo( cInfo ) << std::endl;
		netwAddr = NetwControlInfo::getAddressFromControlInfo( cInfo );
	delete cInfo;
	}

	pkt->setSrcAddr(myNetwAddr);
	pkt->setDestAddr(netwAddr);
	coreEV << " netw "<< myNetwAddr << " sending packet" <<std::endl;
	if(LAddress::isL3Broadcast( netwAddr )) {
		coreEV << "sendDown: nHop=L3BROADCAST -> message has to be broadcasted"
		   << " -> set destMac=L2BROADCAST\n";
		macAddr = LAddress::L2BROADCAST;
	}
	else{
		coreEV <<"sendDown: get the MAC address\n";
		macAddr = arp->getMacAddr(netwAddr);
	}

//	setDownControlInfo(pkt, macAddr);

	//encapsulate the application packet
	pkt->encapsulate(appPkt);
	coreEV <<" pkt encapsulated\n";
	return pkt;
}

void ProphetV2::handleLowerMsg(cMessage* msg)
{
    NetwPkt *m = static_cast<NetwPkt *>(msg);
    coreEV << " handling packet from " << m->getSrcAddr() << std::endl;
    sendUp(decapsMsg(m));
//    sendUp(msg);
}

void ProphetV2::handleUpperMsg(cMessage* msg)
{
	assert(dynamic_cast<WaveShortMessage*>(msg));
	WaveShortMessage* tmp = static_cast<WaveShortMessage*>(msg);
	std::multimap<LAddress::L3Type, WaveShortMessage*>::iterator it;
	/**
	 * part of code to verify if the msg exist currently in data structures that store bundles and metadata
	 */
	if (mapsForBundles.count(tmp->getRecipientAddress())==0){
		bundles.push_back(static_cast<WaveShortMessage*>(msg));
		mapsForBundles.insert(std::pair<LAddress::L3Type, WaveShortMessage*>(tmp->getRecipientAddress(),tmp));
	}
	if (mapsForBundles.count(tmp->getRecipientAddress())>0){
		bool exist = false;
		for (it = mapsForBundles.lower_bound(tmp->getRecipientAddress()); it != mapsForBundles.upper_bound(tmp->getRecipientAddress()); ++it){
				if (it->second == tmp){
					exist = true;
					break;
				}
		}
		if (!exist){
			bundles.push_back(static_cast<WaveShortMessage*>(msg));
			mapsForBundles.insert(std::pair<LAddress::L3Type, WaveShortMessage*>(tmp->getRecipientAddress(),tmp));
		}
	}



	if (canITransmit()){
		WaveShortMessage* copy = tmp->dup();
		sendDown(encapsMsg(copy));
	}
//    sendDown(msg);
}

void ProphetV2::handleSelfMsg(cMessage* msg)
{
//	getParentModule();
//	BaseConnectionManager::
}

//void ProphetV2::handleLowerControl(cMessage* msg)
//{
//
//}
//
//void ProphetV2::handleUpperControl(cMessage* msg)
//{
//
//}

//cObject *const ProphetV2::setDownControlInfo(cMessage *const pMsg, const LAddress::L2Type& pDestAddr)
//{
//	return BaseNetwLayer::setDownControlInfo(pMsg, pDestAddr);
//}
//
//cObject *const ProphetV2::setUpControlInfo(cMessage *const pMsg, const LAddress::L3Type& pSrcAddr)
//{
//	return BaseNetwLayer::setUpControlInfo(pMsg, pSrcAddr);
//}

void ProphetV2::finish()
{

}

void ProphetV2::resumeConnection()
{
	if (!bundles.empty()){
	std::list<WaveShortMessage*>::iterator it;
		for (it = bundles.begin();it != bundles.end();++it){
			handleUpperMsg(*it);
		}
	}

}

ProphetV2::~ProphetV2()
{
}


