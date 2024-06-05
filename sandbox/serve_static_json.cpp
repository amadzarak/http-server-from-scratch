// Parsing simple json file
#include <iostream>
#include <fstream>

using namespace std;
int main(int argc, const char * argv[]) {
    char line[20];
    std::ifstream myfile;
    myfile.open("posts.json");
   // myfile.seekg(0, ios::end);
   // int file_size = myfile.tellg();
    if (myfile.is_open()) {
        while (myfile.getline(line, 200)) {
            std::cout << line << std::endl;
        }
        myfile.close();
    }
    return 0;
}
