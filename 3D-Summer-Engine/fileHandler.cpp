#include "fileHandler.h"

std::fstream fileHandler::openFile(std::string fileName) {
	std::fstream openedFile(fileName);
	return openedFile;
}

bool fileHandler::createFile(std::string fileName) {
	std::ofstream outFile(fileName);
	outFile.close();
	return true;
}

void fileHandler::closeFile() {
	file.close();
}

bool fileHandler::removeFile(const char * fileName) {
	if (file.is_open())
	{
		fileHandler::closeFile();
	}
	try
	{
		std::remove(fileName);
		return true;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool fileHandler::readFile(std::string fileName, std::string & content) {
	try
	{
		content = "";
		//Variable for containing a single line of the file
		std::string line;

		std::fstream file(fileName);

		//Reading the file
		while (std::getline(file, line))
		{
			content += line + "\n";
		}

		file.close();
		return true;

	}
	catch (const std::exception&)
	{
		return false;
	}
}

bool fileHandler::writeFile(std::string fileName, std::string content) {
	//Open and create the file
	std::fstream fileToBeWrittenTo = fileHandler::openFile(fileName);
	if (fileToBeWrittenTo.is_open()) {
		//Write to the file
		fileToBeWrittenTo << content;
		fileToBeWrittenTo.close();
		return true;
	}
	else
	{
		fileHandler::createFile(fileName);
		fileToBeWrittenTo = std::fstream(fileName);

		fileToBeWrittenTo << content;
		fileToBeWrittenTo.close();
		return true;
	}

	return false;
}

bool fileHandler::createDir(const char* path) {
	return _mkdir(path) ? true : false;
}