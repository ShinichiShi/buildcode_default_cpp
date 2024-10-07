#include <iostream>
#include <fstream>
#include <cstring>  
using namespace std;
int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <tar file>" << std::endl;
        return 1;
    }

    std::ifstream tarFile(argv[1], std::ios::binary);
    if (!tarFile) {
        cerr << "Error: Could not open the file " << argv[1] << std::endl;
        return 1;
    }

    char filename[101]; 
    memset(filename, 0, sizeof(filename)); 

    tarFile.read(filename, 100); 
    if (!tarFile) {
        cerr << "Error: Could not read the tar header" << std::endl;
        return 1;
    }

    cout << filename << endl;

    tarFile.close();

    return 0;
}
