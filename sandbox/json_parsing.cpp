// https://dev.to/uponthesky/c-making-a-simple-json-parser-from-scratch-250g
#include <iostream>
#include <fstream>
#include <string>

void load_file(std::string & file_buffer) {
    std::ifstream file("posts.json");
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            file_buffer.append(line);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    std::string file_buffer;

    load_file(file_buffer);
    std::cout << file_buffer << std::endl;
    return 0;
}
