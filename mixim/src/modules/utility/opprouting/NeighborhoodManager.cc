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

#include "NeighborhoodManager.h"
#include "ProphetV2.h"

NeighborhoodManager::NeighborhoodManager() {
	// TODO Auto-generated constructor stub
	hostModule = NULL;
	nicSrc = 0;
	srcAdresse = 0;
	anyNeighbor = false;
	neighbors = std::map<int,LAddress::L3Type>();
}

NeighborhoodManager::NeighborhoodManager(cModule* host, int nic)
{
	hostModule = host;
	nicSrc = nic;
	srcAdresse = 0;
	anyNeighbor = false;
	neighbors = std::map<int,LAddress::L3Type>();
}


NeighborhoodManager::NeighborhoodManager(cModule* host, int nic, LAddress::L3Type currentAdresse)
{
	hostModule = host;
	nicSrc = nic;
	srcAdresse = currentAdresse;
	anyNeighbor = false;
	neighbors = std::map<int,LAddress::L3Type>();
}

void NeighborhoodManager::receiveSignal(cComponent *src, simsignal_t id, const char *s)
{
	const char *signalName = cComponent::getSignalName(id);
	const char *delimiters = ":";
	char *signalValue;
	char *token[2];
	strcpy(signalValue,s);

	token[0]=strtok(signalValue, delimiters);
	token[1] = strtok (NULL, delimiters);

	int nic0 = atoi(token[0]);
	int nic1 = atoi(token[1]);
	/*
	 * Verify if we are concerned by this signal
	 */

	if (nicSrc == nic0){
		if (strcmp(signalName,"Connect")){
			if (neighbors.size()==0){
				anyNeighbor = true;
			}
			neighbors.insert(std::pair<int,LAddress::L3Type>(nic1,LAddress::L3NULL));
			ProphetV2* module = check_and_cast<ProphetV2*>(hostModule);
			module->resumeConnection();
		}else if (strcmp(signalName,"Disconnect")){
			neighbors.erase(nic1);
			if (neighbors.size()==0){
				anyNeighbor = false;
			}
		}
	}
	else if (nicSrc==nic1){
		if (strcmp(signalName,"Connect")){
			if (neighbors.size()==0){
				anyNeighbor = true;
			}
			neighbors.insert(std::pair<int,LAddress::L3Type>(nic0,LAddress::L3NULL));
			ProphetV2* module = check_and_cast<ProphetV2*>(hostModule);
			module->resumeConnection();
		}else if (strcmp(signalName,"Disconnect")){
			neighbors.erase(nic0);
			if (neighbors.size()==0){
				anyNeighbor = false;
			}
		}
	}

}

LAddress::L3Type NeighborhoodManager::getSrcAdresse() const
{
    return srcAdresse;
}

cModule *NeighborhoodManager::getHostModule() const
{
    return hostModule;
}

bool NeighborhoodManager::isAnyNeighbor() const
{
    return anyNeighbor;
}

void NeighborhoodManager::setSrcAdresse(LAddress::L3Type srcAdresse)
{
    this->srcAdresse = srcAdresse;
}

NeighborhoodManager::~NeighborhoodManager() {
	// TODO Auto-generated destructor stub
}

