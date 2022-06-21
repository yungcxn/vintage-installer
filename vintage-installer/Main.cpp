
#define CURL_STATICLIB
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "curl/curl.h"
#include "json.hpp"

#ifdef _DEBUG
#pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment (lib, "curl/libcurl_a.lib")
#endif

#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "Crypt32.lib")
#pragma comment (lib, "advapi32.lib")


#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <string>
#include <vector>

using namespace std;
using namespace experimental::filesystem;
using namespace nlohmann;



string jsonurl = "https://raw.githubusercontent.com/yungcxn/vintage-client/master/vintage/vintage.json";
string versionurl = "https://raw.githubusercontent.com/yungcxn/vintage-client/master/vintage/vintage.jar";
string liburl = "https://raw.githubusercontent.com/yungcxn/vintage-client/master/lib/lib.txt";

string berinicon = "https://raw.githubusercontent.com/yungcxn/vintage-client/master/berinicon.txt";






static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
	return written;
}

void downloadFile(const char* url, const char* fname) {
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(fname, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
}


std::vector<std::string> splitString(std::string str, char splitter) {
	std::vector<std::string> result;
	std::string current = "";
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == splitter) {
			if (current != "") {
				result.push_back(current);
				current = "";
			}
			continue;
		}
		current += str[i];
	}
	if (current.size() != 0)
		result.push_back(current);
	return result;
}

int main() {
	curl_global_init(CURL_GLOBAL_ALL);

	string pathtomc;
	char* buf = nullptr;
	size_t sz = 0;
	if (_dupenv_s(&buf, &sz, "APPDATA") == 0 && buf != nullptr)
	{
		pathtomc = buf;
		pathtomc += "\\.minecraft";
		free(buf);

	}

	cout << "installing latest vintage version..." << endl;
	cout << "by cxn" << endl << endl;


	cout << "locating mc folder... " << pathtomc << endl;

	string pathtovintage = pathtomc + "\\vintage";
	create_directory(pathtovintage);
	string pathtolib = pathtovintage + "\\lib";
	create_directory(pathtolib);

	string pathtovintageversion = pathtomc + "\\versions\\vintage";
	create_directory(pathtovintageversion);

	
	downloadFile(jsonurl.c_str(), (pathtovintageversion + "\\vintage.json").c_str());
	downloadFile(liburl.c_str(), "lib.txt");
	downloadFile(versionurl.c_str(), (pathtovintageversion + "\\vintage.jar").c_str());

	string linex;
	ifstream myfile("lib.txt");
	
	vector<string> libs;

	if (myfile.is_open())
	{

		while (getline(myfile, linex)) {
			libs.push_back(linex);
		}
		myfile.close();
	}
	for (int i = 0; i < libs.size(); i++) {
		vector<string> words = splitString(libs.at(i), '/');
		string name = words.at(words.size() - 1);
		downloadFile(libs.at(i).c_str(), (pathtolib + "\\" + name).c_str());
	}
	
	curl_global_cleanup();





	cout << endl << "download successful! configuring launcher profile..." << endl;


	string pathtolauncherprof = pathtomc + "\\launcher_profiles.json";
	ifstream lpf (pathtolauncherprof);
	json j;
	lpf >> j;

	boolean found = false;
	for (auto it : j["profiles"]) {
		if (it["lastVersionId"] == "vintage") {
			found = true;
			cout << "launcher profile found... ending " << endl;
		}
	}

	if (!found) {
		j["profiles"]["1d5c2b3d200f5c1a8432b67f1be2f56f"] = {
			{"created", "2021-01-01T00:00:00.000Z"},
			{"icon" , "Carved_Pumpkin"},
			{"javaArgs" , "-Xmx4G -Xms4G -Xmn3072m -XX:+DisableExplicitGC -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:+UseNUMA -XX:+CMSParallelRemarkEnabled -XX:MaxGCPauseMillis=30 -XX:GCPauseIntervalMillis=150 -XX:+UseAdaptiveGCBoundary -XX:-UseGCOverheadLimit -XX:+UseBiasedLocking -XX:SurvivorRatio=8 -XX:TargetSurvivorRatio=90 -XX:MaxTenuringThreshold=15 -Dfml.ignorePatchDiscrepancies=true -Dfml.ignoreInvalidMinecraftCertificates\u037dtrue -XX:+UseFastAccessorMethods -XX:+UseCompressedOops -XX:+OptimizeStringConcat -XX:+AggressiveOpts -XX:ReservedCodeCacheSize=2048M -XX:+UseCodeCacheFlushing -XX:SoftRefLRUPolicyMSPerMB=20000 -XX:ParallelGCThreads=10"},
			{"lastUsed" , "2020-01-01T00:00:00.000Z"},
			{"lastVersionId", "vintage"},
			{"name" , "vintage"},
			{"type" , "custom" } };
	}
	
	cout << "downloading berins face for launcher profile... " << endl;
	downloadFile(berinicon.c_str(), "berinicon.txt");
	string content;
	ifstream b("berinicon.txt");
	string line;

	if (b.is_open())
	{

		while (getline(b, line)) {
			content += line;
		}
		b.close();
	}
	

	j["profiles"]["1d5c2b3d200f5c1a8432b67f1be2f56f"]["icon"] = content;
	cout << "updated berins face for launcher profile... " << endl;

	ofstream o(pathtolauncherprof);
	o << std::setw(4) << j << std::endl;
	cout << "overwrote launcher profile! " << endl;
	cout << "done!" << endl;
	system("pause");

	
	

	return 0;
}
