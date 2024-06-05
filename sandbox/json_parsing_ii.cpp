// https://dev.to/uponthesky/c-making-a-simple-json-parser-from-scratch-250g
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
// Function to check if brackets are balanced
bool validate_structure(std::string & file_buffer)
{
    // Declare a stack to hold the previous brackets.
    std::stack<char> temp;
    for (int i = 0; i < file_buffer.length(); i++) {
        if (temp.empty()) {

            // If the stack is empty
            // just push the current bracket
            temp.push(file_buffer[i]);
        }
        else if ((temp.top() == '(' && file_buffer[i] == ')')
                 || (temp.top() == '{' && file_buffer[i] == '}')
                 || (temp.top() == '[' && file_buffer[i] == ']')) {

            // If we found any complete pair of bracket
            // then pop
            temp.pop();
        }
        else {
            temp.push(file_buffer[i]);
        }
    }
    if (temp.empty()) {

        // If stack is empty return true
        return true;
    }
    return false;
}

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
    //std::cout << file_buffer << std::endl;

    //if (validate_structure(file_buffer) == true) {
    //    std::cout << "Valid JSON structure" << std::endl;
    //} else {
    //    std::cerr << "Invalid JSON" << std::endl;
    //}
    return 0;
}
