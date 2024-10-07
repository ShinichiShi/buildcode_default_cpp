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

// Trim null bytes and whitespace
std::string trim(const char* str, size_t size) {
    std::string result;
    for (size_t i = 0; i < size && str[i] != '\0'; ++i) {
        result += str[i];
    }
    // Trim trailing spaces
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
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

    // Print filename
    std::cout << trim(header.name, sizeof(header.name)) << std::endl;

    // Print numeric fields
    std::cout << octalToDecimal(std::string(header.mode, sizeof(header.mode))) << std::endl;
    std::cout << octalToDecimal(std::string(header.uid, sizeof(header.uid))) << std::endl;
    std::cout << octalToDecimal(std::string(header.gid, sizeof(header.gid))) << std::endl;
    std::cout << octalToDecimal(std::string(header.size, sizeof(header.size))) << std::endl;
    std::cout << octalToDecimal(std::string(header.mtime, sizeof(header.mtime))) << std::endl;
    
    // Print checksum (keep leading zeros)
    std::string checksum(header.checksum, 6); // Use first 6 bytes only
    std::cout << checksum << std::endl;

    // Print typeflag
    std::cout << trim(header.typeflag, sizeof(header.typeflag)) << std::endl;

    // Print empty line for linkname
    std::cout << std::endl;

    // Print magic
    std::cout << trim(header.magic, sizeof(header.magic)) << std::endl;

    // Print empty line for version
    std::cout << std::endl;

    // Print username and groupname
    std::cout << trim(header.uname, sizeof(header.uname)) << std::endl;
    std::cout << trim(header.gname, sizeof(header.gname)) << std::endl;

    // Print two empty lines for devmajor and devminor
    std::cout << std::endl;
    std::cout << std::endl;

    // Print empty line for prefix
    std::cout << std::endl;

    return 0;
}