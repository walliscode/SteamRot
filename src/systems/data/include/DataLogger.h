

class DataLogger
{

public:
	DataLogger() = default;

	void logData(); // Log data to a binary file, this will call serialise functions
	void createJSON(std::string filePath); // Create a JSON file from the binary file using flatbuffers
};
