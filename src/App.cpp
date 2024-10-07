#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstring>
#include <system_error>

namespace fs = std::filesystem;

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
            if (c != '\0' && c != ' ') trimmed += c;
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

// Extract regular file
bool extractFile(const fs::path& outputPath, std::ifstream& archive, size_t fileSize) {
    // Create parent directories if they don't exist
    fs::create_directories(outputPath.parent_path());

    // Open output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to create file: " << outputPath << std::endl;
        return false;
    }

    // Read and write file contents
    std::vector<char> buffer(fileSize);
    if (fileSize > 0) {
        if (!archive.read(buffer.data(), fileSize)) {
            std::cerr << "Failed to read file contents" << std::endl;
            return false;
        }
        outFile.write(buffer.data(), fileSize);
    }

    // Skip padding bytes
    size_t paddingSize = (512 - (fileSize % 512)) % 512;
    archive.seekg(paddingSize, std::ios::cur);

    return true;
}

// Handle hard links and symbolic links
bool handleLink(const fs::path& outputPath, const std::string& linkname, char typeflag) {
    try {
        if (typeflag == '2') { // Symbolic link
            fs::create_symlink(linkname, outputPath);
        } else if (typeflag == '1') { // Hard link
            fs::create_hard_link(linkname, outputPath);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create link: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5 || std::string(argv[1]) != "-xf" || std::string(argv[3]) != "-C") {
        std::cerr << "Usage: " << argv[0] << " -xf <tar-file> -C <output-dir>" << std::endl;
        return 1;
    }

    // Open tar archive
    std::ifstream archive(argv[2], std::ios::binary);
    if (!archive) {
        std::cerr << "Failed to open archive: " << argv[2] << std::endl;
        return 1;
    }

    // Create output directory
    fs::path outputDir = argv[4];
    try {
        fs::create_directories(outputDir);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create output directory: " << e.what() << std::endl;
        return 1;
    }

    while (archive) {
        // Read header
        TarHeader header;
        if (!archive.read(reinterpret_cast<char*>(&header), sizeof(header))) {
            break;
        }

        // Check for end of archive
        if (isEmptyHeader(header)) {
            break;
        }

        // Get filename and size
        std::string filename = trim(header.name, sizeof(header.name));
        size_t fileSize = octalToDecimal(std::string(header.size, sizeof(header.size)));
        char typeflag = header.typeflag[0];
        std::string linkname = trim(header.linkname, sizeof(header.linkname));

        // Construct output path
        fs::path outputPath = outputDir / filename;

        // Check if it's a directory
        if (typeflag == '5') {
            try {
                fs::create_directories(outputPath);
            } catch (const std::exception& e) {
                std::cerr << "Failed to create directory: " << outputPath << std::endl;
                return 1;
            }
        } else if (typeflag == '0' || typeflag == '\0') {
            // Extract regular file
            if (!extractFile(outputPath, archive, fileSize)) {
                return 1;
            }
        } else if (typeflag == '2' || typeflag == '1') {
            // Handle links
            if (!handleLink(outputPath, linkname, typeflag)) {
                return 1;
            }
        }
    }

    return 0;
}
