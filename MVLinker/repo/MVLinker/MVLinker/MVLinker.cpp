#include "plugin.h"
//>>>
#include "filesystem"
//<<<

using namespace plugin;
//>>>
using namespace std;

const string MODNAME = "MVLinker";

const string NAMESUFFIX = " _" + MODNAME + "_";

const string VEHICLESFOLDER = "vehicles/";
const string WEAPONSFOLDER = "weapons/";

HMODULE mvlmodule;
string mvlpath;

bool checkDirectory(string directorypath) {
	return filesystem::exists(directorypath) && filesystem::is_directory(directorypath);
}

void deleteLinks(string foldername) {
	string folderpath = mvlpath + foldername;
	if (checkDirectory(folderpath)) {
		for (filesystem::directory_entry directoryentry : filesystem::directory_iterator(folderpath)) {
			if (!filesystem::is_directory(directoryentry)) {
				string entryname = directoryentry.path().stem().string();
				int entrylength = entryname.size();
				if (entrylength > 10 && entryname.substr(entrylength - 11, entrylength) == NAMESUFFIX) {
					filesystem::remove(directoryentry);
				}
			}
		}
	}
}

void cleanUp() {
	deleteLinks(VEHICLESFOLDER);
	deleteLinks(WEAPONSFOLDER);
}

string pluginpath;

void createLinks(string foldername) {
	string folderpath = pluginpath + foldername;
	if (checkDirectory(folderpath)) {
		for (filesystem::directory_entry directoryentry : filesystem::directory_iterator(folderpath)) {
			if (!filesystem::is_directory(directoryentry)) {
				filesystem::path entrypath = directoryentry.path();
				filesystem::create_hard_link(entrypath, mvlpath + foldername + entrypath.stem().string() + NAMESUFFIX + entrypath.extension().string());
			}
		}
	}
}
//<<<

class MVLinker {
public:
    MVLinker() {
        // Initialise your plugin here
        
//>>>
		Events::attachRwPluginsEvent.after += [] {
			mvlmodule = GetModuleHandle("vehmod_retail.dll");
			if (!mvlmodule) {
				mvlmodule = GetModuleHandle("vehmod_steam.dll");
			}
			if (mvlmodule) {
				char pathchar[255];
				GetModuleFileName(mvlmodule, pathchar, sizeof(pathchar));
				mvlpath = filesystem::path(pathchar).parent_path().string() + '/';

				cleanUp();
			}
		};

		Events::initGameEvent.before += [] {
			if (mvlmodule) {
				pluginpath = PLUGIN_PATH("") + filesystem::path(PLUGIN_FILENAME).stem().string() + '/';

				createLinks(VEHICLESFOLDER);
				createLinks(WEAPONSFOLDER);
			}
			};

		Events::initScriptsEvent.before += [] {
			if (mvlmodule) {
				cleanUp();
			}
		};
//<<<
    }
//>>>
//<<<
} mVLinker;

//>>>
//<<<