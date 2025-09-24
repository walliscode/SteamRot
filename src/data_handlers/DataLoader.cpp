/////////////////////////////////////////////////
/// @file
/// @brief Implementation of abstract DataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataLoader.h"
#include <fstream>
namespace steamrot {

////////////////////////////////////////////////////////////
char *DataLoader::LoadBinaryData(const std::filesystem::path &file_path) const {

  // open file in binary mode
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];
  infile.read(data, length);
  infile.close();
  return data;
}
} // namespace steamrot
