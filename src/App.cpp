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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <tar-file>" << std::endl;
        return 1;
    }

    // Open file in binary mode
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Read the header
    TarHeader header;
    if (!file.read(reinterpret_cast<char*>(&header), 512)) {
        std::cerr << "Failed to read header" << std::endl;
        return 1;
    }

    // Get the file size from the header
    size_t fileSize = octalToDecimal(std::string(header.size, sizeof(header.size)));
    if (fileSize == 0) {
        return 0;  // Empty file
    }

    // Create a buffer exactly the size of the file
    std::vector<char> buffer(fileSize);

    // Read exactly fileSize bytes
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Failed to read file contents" << std::endl;
        return 1;
    }

    // Write the exact file contents without any null padding
    std::cout.write(buffer.data(), fileSize);

    return 0;
}