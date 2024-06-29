// In utils.h or a similar header file

#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace utils {

    inline bool fileExists(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    inline std::vector<std::byte> readBinaryFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file) {
            auto size = file.tellg();
            std::vector<std::byte> buffer(size);
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(buffer.data()), size);
            return buffer;
        }
        return {};
    }

} // namespace utils
