#include "DataLogger.h"
#include <iostream>
#include <fstream>
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/monster_generated.h"



void DataLogger::logData()
{

	std::cout << "Logging data..." << std::endl;

	// create a master binary file (for now), this will then call various serialise functions that all add data to the binary file
	std::string fileName =  std::string(BINARIES_PATH) + "test_data.bin"; 
	std::ofstream testFile(fileName, std::ios::binary);

	testFile.close();
}

// this function will create a JSON file from the binary file using flatbuffers to create a human readable test file
void DataLogger::createJSON(std::string filePath)
{
	std::cout << "Creating JSON file..." << std::endl;

	// I've put down a general strategy for converting from binary to json that I will build upon in the next pull request, the below is example code from https://flatbuffers.dev/flatbuffers_guide_use_cpp.html
	// 1. Read the binary file
	std::ifstream infile;
	infile.open("monsterdata_test.mon", std::ios::binary | std::ios::in);
	infile.seekg(0, std::ios::end);
	int length = infile.tellg();
	infile.seekg(0, std::ios::beg);
	char* data = new char[length];
	infile.read(data, length);
	infile.close();

	// 2. Parse the binary file using some kind of FBS schema

	// auto json = flatbuffers::GetRoot(data);

	// 3. convert to string

	// 4. write to to a JSON file


}