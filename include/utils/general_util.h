#pragma once

#include <fstream>
#include <string>
#include <vector>


namespace utils {

// Check if a file exists by taking in its path as a string
inline bool fileExists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

// Read a binary file and return a vector of bytes
inline std::vector<std::byte> readBinaryFile(const std::string &filename) {
  std::ifstream file(filename,
                     std::ios::binary |
                         std::ios::ate); // std::ios::ate sets the position at
                                         // the end of the file
  if (file) {
    auto size =
        file.tellg(); // returns the position of the current character in the
                      // input stream, becuase we have set the position at the
                      // end of the file, this will return the size of the file
    std::vector<std::byte> buffer(
        size); // create a vector of bytes with the size of the file
    file.seekg(0,
               std::ios::beg); // set the position at the beginning of the file
    file.read(reinterpret_cast<char *>(buffer.data()),
              size); // read the file into the buffer
    return buffer;
  }
  return {};
}

} // namespace utils
