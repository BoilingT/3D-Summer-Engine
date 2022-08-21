#pragma once
#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <direct.h>
#include <list>


class fileHandler
{
public:
	bool writeFile(std::string fileName, std::string content);
	bool readFile(std::string fileName, std::string & content);
	bool removeFile(const char * fileName);
	bool createDir(const char* path);


private:
	std::fstream file;

	std::fstream openFile(std::string fileName);
	bool createFile(std::string fileName);
	void closeFile();

};

