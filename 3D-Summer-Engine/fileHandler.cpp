#include "fileHandler.h"

std::fstream FileHandler::openFile(std::string fileName)
{
	std::fstream openedFile(fileName);
	return openedFile;
}

bool FileHandler::createFile(std::string fileName)
{
	std::ofstream outFile(fileName);
	outFile.close();
	return true;
}

void FileHandler::closeFile()
{
	file.close();
}

bool FileHandler::removeFile(const char *fileName)
{
	if ( file.is_open() )
	{
		FileHandler::closeFile();
	}
	try
	{
		std::remove(fileName);
		return true;
	}
	catch ( const std::exception &e )
	{
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool FileHandler::readFile(std::string fileName, std::string &content)
{
	try
	{
		content = "";
		//Variable for containing a single line of the file
		std::string line;

		std::fstream file(fileName);

		//Reading the file
		while ( std::getline(file, line) )
		{
			content += line + "\n";
		}

		file.close();
		return true;

	}
	catch ( const std::exception & )
	{
		return false;
	}
}

bool FileHandler::writeFile(std::string fileName, std::string content)
{
//Open and create the file
	std::fstream fileToBeWrittenTo = FileHandler::openFile(fileName);
	if ( fileToBeWrittenTo.is_open() )
	{
//Write to the file
		fileToBeWrittenTo << content;
		fileToBeWrittenTo.close();
		return true;
	}
	else
	{
		FileHandler::createFile(fileName);
		fileToBeWrittenTo = std::fstream(fileName);

		fileToBeWrittenTo << content;
		fileToBeWrittenTo.close();
		return true;
	}

	return false;
}

bool FileHandler::createDir(const char *path)
{
	return _mkdir(path) ? true : false;
}

bool FileHandler::fileExists(std::string fileName)
{
	std::ifstream f(fileName.c_str());
	return f.good();
}
