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

    // Read header
    TarHeader header;
    if (!file.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        std::cerr << "Failed to read header" << std::endl;
        return 1;
    }

    // Get file size from header (stored in octal)
    size_t fileSize = octalToDecimal(std::string(header.size, sizeof(header.size)));

    // Calculate number of 512-byte chunks needed
    size_t numChunks = (fileSize + 511) / 512;  // Round up division

    // Create buffer for file contents
    std::vector<char> buffer(numChunks * 512);

    // Read all chunks
    if (!file.read(buffer.data(), numChunks * 512)) {
        std::cerr << "Failed to read file contents" << std::endl;
        return 1;
    }

    // Write only the actual file contents (up to fileSize)
    std::cout.write(buffer.data(), fileSize);

    return 0;
}