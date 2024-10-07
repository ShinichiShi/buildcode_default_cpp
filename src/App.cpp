#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

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
size_t octalToDecimal(const std::string& octal) {
    if (octal.empty()) return 0;
    try {
        std::string trimmed;
        for (char c : octal) {
            if (c != '\0') trimmed += c;
        }
        return std::stoull(trimmed, nullptr, 8);
    } catch (...) {
        return 0;
    }
}

// Trim null bytes and whitespace from string
std::string trim(const char* str, size_t size) {
    std::string result;
    for (size_t i = 0; i < size && str[i] != '\0'; ++i) {
        result += str[i];
    }
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

// Check if a header is empty (all null bytes)
bool isEmptyHeader(const TarHeader& header) {
    const char* ptr = reinterpret_cast<const char*>(&header);
    for (size_t i = 0; i < sizeof(TarHeader); ++i) {
        if (ptr[i] != '\0') return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "-tf") {
        std::cerr << "Usage: " << argv[0] << " -tf <tar-file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[2], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[2] << std::endl;
        return 1;
    }

    while (file) {
        // Read header
        TarHeader header;
        if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            break;
        }

        // Check if we've reached the end (two consecutive empty headers)
        if (isEmptyHeader(header)) {
            break;
        }

        // Get the filename
        std::string filename = trim(header.name, sizeof(header.name));
        if (!filename.empty()) {
            std::cout << filename << std::endl;
        }

        // Get file size and skip to next header
        size_t fileSize = octalToDecimal(std::string(header.size, sizeof(header.size)));
        size_t skipSize = ((fileSize + 511) / 512) * 512;  // Round up to next 512 boundary
        file.seekg(skipSize, std::ios::cur);
    }

    return 0;
}