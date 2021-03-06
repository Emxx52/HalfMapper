#include "common.h"
#include "bsp.h"
#include "ConfigXML.h"

void parseEntities(const string &szStr, const string &id, const MapEntry &sMapEntry){
	stringstream ss(szStr);
	
	int status = 0;
	
	string origin, targetname,landmark, modelname;
	bool isLandMark=false,isChangeLevel=false,isTeleport=false;
	
	map <string,int> changelevels;
	map <string,VERTEX> ret;
	
	while(ss.good()){
		string str;
		getline(ss, str);
		if(status == 0){
			if(str == "{"){
				status = 1, isLandMark=false,isChangeLevel=false,isTeleport=false;
			}else{
				if(ss.good())
					cerr << "Missing stuff in entity: " << str << endl;
			}
		}else if(status == 1){
			if(str == "}"){
				status = 0;
				if(isLandMark){
					float x,y,z;					
					sscanf(origin.c_str(),"%f %f %f", &x,&y,&z);
					VERTEX v(x,y,z);
					v.fixHand();
					
					if (sMapEntry.m_szOffsetTargetName == targetname) {
						// Apply map offsets from the config, to fix landmark positions.
						v.x += sMapEntry.m_fOffsetX;
						v.y += sMapEntry.m_fOffsetY;
						v.z += sMapEntry.m_fOffsetZ;
					}
					
					ret[targetname] = v;
					
				}else if(isChangeLevel){
					if(landmark.size()>0)
						changelevels[landmark]=1;
				}
				if(isTeleport || isChangeLevel){
					dontRenderModel[id].push_back(modelname);
				}
			}else{
				if(str == "\"classname\" \"info_landmark\""){
					isLandMark=true;
				}
				if(str == "\"classname\" \"trigger_changelevel\""){
					isChangeLevel=true;
				}
				if(str == "\"classname\" \"trigger_teleport\"" || str == "\"classname\" \"func_pendulum\"" || str == "\"classname\" \"trigger_transition\"" 
				|| str == "\"classname\" \"trigger_hurt\"" || str == "\"classname\" \"func_train\"" || str == "\"classname\" \"func_door_rotating\""){
					isTeleport=true;
				}
				if(str.substr(0,8) == "\"origin\""){
					origin = str.substr(10);
					origin.erase(origin.size() - 1);
				}
				if(str.substr(0,7) == "\"model\""){
					modelname = str.substr(9);
					modelname.erase(modelname.size() - 1);
				}
				if(str.substr(0,12) == "\"targetname\""){
					targetname = str.substr(14);
					targetname.erase(targetname.size() - 1);
				}
				if(str.substr(0,10) == "\"landmark\""){
					landmark = str.substr(12);
					landmark.erase(landmark.size() - 1);
				}
			}
		}
	}
	for(map <string,VERTEX>::iterator it=ret.begin(); it!=ret.end(); it++){
		if(changelevels.count((*it).first) != 0){
			landmarks[(*it).first].push_back(make_pair((*it).second, id));
		}
	}

}
