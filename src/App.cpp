#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

struct TarHeader {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
};

// Convert octal string to decimal
long long octalToDecimal(const std::string& octal) {
    if (octal.empty()) return 0;
    try {
        std::string trimmed;
        for (char c : octal) {
            if (c != '\0') trimmed += c;
        }
        return std::stoll(trimmed, nullptr, 8);
    } catch (...) {
        return 0;
    }
}

// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\0");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\0");
    return str.substr(first, last - first + 1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <tar-file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    TarHeader header;
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        std::cerr << "Failed to read header" << std::endl;
        return 1;
    }

    // Print all fields
    std::cout << trim(std::string(header.name, sizeof(header.name))) << std::endl;
    std::cout << octalToDecimal(std::string(header.mode, sizeof(header.mode))) << std::endl;
    std::cout << octalToDecimal(std::string(header.uid, sizeof(header.uid))) << std::endl;
    std::cout << octalToDecimal(std::string(header.gid, sizeof(header.gid))) << std::endl;
    std::cout << octalToDecimal(std::string(header.size, sizeof(header.size))) << std::endl;
    std::cout << octalToDecimal(std::string(header.mtime, sizeof(header.mtime))) << std::endl;
    
    // For checksum, trim the trailing 2 bytes as specified
    std::string checksum(header.checksum, sizeof(header.checksum) - 2);
    std::cout << octalToDecimal(checksum) << std::endl;
    
    std::cout << trim(std::string(header.typeflag, sizeof(header.typeflag))) << std::endl;
    std::cout << trim(std::string(header.linkname, sizeof(header.linkname))) << std::endl;
    std::cout << trim(std::string(header.magic, sizeof(header.magic))) << std::endl;
    std::cout << trim(std::string(header.version, sizeof(header.version))) << std::endl;
    std::cout << trim(std::string(header.uname, sizeof(header.uname))) << std::endl;
    std::cout << trim(std::string(header.gname, sizeof(header.gname))) << std::endl;
    std::cout << octalToDecimal(std::string(header.devmajor, sizeof(header.devmajor))) << std::endl;
    std::cout << octalToDecimal(std::string(header.devminor, sizeof(header.devminor))) << std::endl;
    std::cout << trim(std::string(header.prefix, sizeof(header.prefix))) << std::endl;

    return 0;
}