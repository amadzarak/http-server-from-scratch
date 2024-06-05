// Sandboxing the first part of implementation: Parsing the "REQUEST LINE"

#include <iostream>
#include <string.h>

struct http_request_line {
    const char * method;
    const char * route;
    const char * http_version;
};

std::vector<int> get_single_space_locations(const char * line) {
    std::vector<int> spaceLocations;
    for (int i = 0; i < strlen(line); ++i)
        if (line[i] == ' ')
            spaceLocations.push_back(i);

    //for (int i = 0; i < spaceLocations.size(); ++i) {
    //    std::cout << spaceLocations[i] << std::endl;
    //}

    return spaceLocations;
}

char* substring(const char* src, int start_index, int end_index) {

    int length = end_index - start_index;

    if (length < 0)
        return NULL;

    char* dest = new char[length + 1]; // use new[] not malloc

    std::copy(src + start_index, src + end_index, dest); // prefer to memcopy
    dest[length] = '\0';

    return dest;
}

int parse_route(const char * line) {
    std::vector<int> spaceLocations = get_single_space_locations(line);
    const char * method = substring(line, 0, spaceLocations[0]);
    const char * route = substring(line, spaceLocations[0] + 1, spaceLocations[1]);
    const char * http_version = substring(line, spaceLocations[1] + 1, strlen(line));
    std::cout << method << std::endl;
    std::cout << route << std::endl;
    std::cout << http_version << std::endl;
    return 0;
}

http_request_line parse_http_request_line(const char * line) {
    std::vector<int> spaceLocations = get_single_space_locations(line);
    const char * method = substring(line, 0, spaceLocations[0]);
    const char * route = substring(line, spaceLocations[0] + 1, spaceLocations[1]);
    const char * http_version = substring(line, spaceLocations[1] + 1, strlen(line));

    return (http_request_line){.method = method,
                               .route = route,
                               .http_version = http_version};
}

int parse_path_and_query(const char * line) {
    // line = [METHOD PATH QUERY]
    //               ^     ^   ^
    const char * start_of_path = strchr(line, ' ') + 1;
    const char * start_of_query = strchr(start_of_path, '?');
    const char * end_of_query = strchr(start_of_query, ' ');

    // Allocate memory

    char path[start_of_query - start_of_path];
    char query[end_of_query - start_of_query];

    // Copy string to memory

    strncpy(path, start_of_path, start_of_query - start_of_path);
    strncpy(query, start_of_query, end_of_query - start_of_query);

    // NULL terminators
    path[sizeof(path)] = 0;
    query[sizeof(query)] = 0;

    std::cout << query << std::endl;
    std::cout << path << std::endl;

    return 0;
};



int parse_request_line(const char * line) {
    int i = 0;
    while (isspace(line[i]) == false) {
        ++i;
    }

    const char * method = substring(line, 0, i);
    std::cout << method << std::endl;

    return 0;
}

int find_crlf_pos(const char * line) {
    /*
      0x0a (ASCII newline)
      0x0d (ASCII carriage return)
      CRLF (0x0d0a)
    */
    for (int i = 0; i + 3 <= strlen(line); ++i) {
        const char * crlf = substring(line, i, i + 3);
        if (strcmp(crlf, "\n\r") == true) {
            std::cout << crlf << std::endl;
            return 0;
        }
    }
    return -1;
}



// Parser tests
int main(int argc, const char * argv[]) {
    struct http_request_line request_line_parsed;
    const char * request_line = "GET / HTTP/1.1\n\r";
    request_line_parsed = parse_http_request_line(request_line);

    std::cout << request_line_parsed.method << std::endl;
    std::cout << request_line_parsed.route << std::endl;
    std::cout << request_line_parsed.http_version << std::endl;
    return 0;
}
