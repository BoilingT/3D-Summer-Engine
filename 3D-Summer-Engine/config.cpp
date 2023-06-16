#include "config.h"

Config::Config(std::string _filepath) {
	filepath = _filepath;
	parseContents(contents);
}

//Clear and load the map with the contents of the file
void Config::updateValues()
{
	file.readFile(filepath, contents);
	parseContents(contents);
}

bool Config::isFile()
{
	return file.isFile(filepath);
}

//Load the map with keys and values from the contents of a file
void Config::parseContents(std::string contents)
{
	//Read the file and store the content in a list
	//Iterate through each character in the list
	//When a space is found cut everything up to that position and save it as a key and delete it from the list
	//When a linebreak is found (\n) cut everything up to that position and save it as a value and delete it from the list
	//Go to the next iteration until there is nothing left

	//Divide the file into lines
	std::string newLineDelimiter = "\n";
	std::vector<std::string> lines = split(contents, newLineDelimiter);
	//Divide parts between equal sign (=)
	std::string delimiter = " ";
	for (std::string line : lines) {
		if (line.empty() || line == " " || line.length() <= 0) {
			continue;
		}
		std::vector<std::string> parts = split(line, delimiter);
		for (unsigned int i = 0; i < parts.size(); i++) {
			std::string part = parts[i];
			if (part.empty() || part == " " || part.length() <= 0) {
				parts.erase(parts.begin() + i);
				i--;
			}
		}

		std::string key = parts[0];
		std::string value = parts[1];

		values[key] = value;
		//std::cout << key << " = " << value << std::endl;
	}

}

//Split a string in parts where the specifik delimiter is found
std::vector<std::string> Config::split(std::string str, std::string delimiter) {
	std::vector<std::string> tokens;

	int lastPos = 0;
	int newPos = 0;
	std::string token;

	while ((newPos = str.find(delimiter, lastPos)) != std::string::npos)
	{
		token = str.substr(lastPos, newPos - lastPos);
		lastPos = newPos + 1;
		tokens.push_back(token);
	}
	token = str.substr(lastPos, newPos - lastPos);
	tokens.push_back(token);

	return tokens;
}

//Unused
std::string Config::concatenateString(std::string s, std::string delimiter) {
	std::vector<std::string> tokens = split(s, ".");
	std::string result = "";

	for (std::string token : tokens)
	{
		result.append(token);
	}
	return result;
}

int Config::countChar(std::string str, char c) {
	int amount = 0;
	for (char x : str) {
		if (x == c) amount++;
	}
	return amount;
}