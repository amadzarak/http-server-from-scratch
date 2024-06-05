// Learning how to make system calls.
#include <cstddef>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, const char * argv[]) {

    struct addrinfo * result;
    int error;

    error = getaddrinfo("192.168.1.7",
                NULL,
                NULL,
                &result);

    // source: https://pubs.opengroup.org/onlinepubs/009695399/functions/getaddrinfo.html
    // A successfull completion of getaddrinfo() is indicated by a return type of 0
    // Error values include: [EAI_AGAIN], [EAI_BADFLAGS], [EAI_FAIL], [EAI_FAMILY], [EAI_MEMORY], [EAI_NONAME]
    // [EAI_SERVICE], [EAI_SOCKTYPE], [EAI_SYSTEM], [EAI_OVERFLOW]

    if (error != 0) {
        if (error == EAI_SYSTEM) {
            std::cerr << "Error in getaddrinfo()" << error << std::endl;
        } else {
            std::cerr << "Error in getaddrinfo()" << error <<std::endl;
        }
    }

    std::cout << error << std::endl;

    return 0;
}
