#include <cstddef> // For std::byte
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

namespace utils {


	bool fileExists(const std::string& filename)
	{
		std::ifstream file(filename);
		return file.good();
	}



    // Function to read the contents of a binary file into a vector of std::byte
    std::vector<std::byte> readBinaryFile(const std::string& fileName) {
        // Open the file in binary mode and position the file pointer at the end to determine the file size
        std::ifstream infile(fileName, std::ios::binary | std::ios::ate);
        if (!infile) {
            // If the file could not be opened, print an error message to standard error and return an empty vector
            std::cerr << "File does not exist or cannot be opened: " << fileName << std::endl;
            return {}; // Return an empty vector to indicate failure
        }

        // Determine the file size by getting the current position of the file pointer (at the end of the file)
        auto length = infile.tellg();
		// if the length is 0 return to avoid undefined behaviour
        if (length == 0) {
            return {};
        }
        // Move the file pointer back to the start of the file to begin reading from the beginning
        infile.seekg(0, std::ios::beg);

        // Create a vector of std::byte large enough to hold the entire file's contents
        std::vector<std::byte> data(length);
        // Read the file's contents into the vector. Since std::ifstream::read expects a char*,
        // we need to cast our std::byte* (obtained from data.data()) to char*.
        // This is safe for binary data and allows us to read directly into the std::byte vector.
        infile.read(reinterpret_cast<char*>(data.data()), length);
        // Close the file (done automatically by the ifstream destructor when it goes out of scope, but shown here for clarity)
        infile.close();

        // Return the vector containing the file's contents
        return data;
    }

}