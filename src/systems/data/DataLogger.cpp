#include "DataLogger.h"
#include <iostream>
#include <fstream>



void DataLogger::logData()
{

	std::cout << "Logging data..." << std::endl;

	std::string fileName =  std::string(BINARIES_PATH) + "test_data.bin";
	std::ofstream testFile(fileName, std::ios::binary);

	testFile.close();
}