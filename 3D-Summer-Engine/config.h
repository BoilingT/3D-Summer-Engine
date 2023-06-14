#pragma once
#include "fileHandler.h" 
#include <string>
#include <vector>
#include <map>

class Config
{
	FileHandler file;
	std::string filepath;
	std::string contents;
public:
	std::map<std::string, std::string> values;

	Config(std::string _filepath);
	void updateValues();
	bool isFile();

private:
	// Fill the map with values from a config
	void parseContents(std::string contents);
	std::vector<std::string> split(std::string str, std::string delimiter);
	std::string concatenateString(std::string s, std::string delimiter);
	int countChar(std::string str, char c);
};

