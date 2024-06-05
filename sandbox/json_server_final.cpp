// This file is part of my process for creating an HTTP server from
// scratch. This file is very simple and sends HTML to the client
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <sstream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 5001
#define BUFFER_SIZE 104857600

void build_http_response() {}
void * handle_client(void *arg) { return NULL;}

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
    // Resources:
    // https://www.gta.ufrj.br/ensino/eel878/sockets/htonsman.html -> htons(), htonl(), ntohs(), ntohl()
    //
    // Socket: an abstraction of a bidirectional network I/O queue.
    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    int option = 1;
    int saddrSize = sizeof(saddr);

    // Create the actual socket
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM means TCP. If we want to use UDP change to SOCK_DGRAM
    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
    if (socketServer == -1) {
        std::cerr << "Not able to create socket. Exiting..." << std::endl;
        return -1;
    }

    // CLIENT
    struct sockaddr_in caddr;
    socklen_t caddrSize = sizeof(caddr);
    int new_socket;

    //int bind(int sock, const struct sockaddr *addr, socklen_t addrlen);
    // int sock                         the socket,
    // const struct sockaddr * addr     the socket address,
    // socklen_t addrlen                the length of the socket address object.
    if (bind(socketServer, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
    };

    // int listen(int sockfd, int backlog);
    // Put server in listening mode so client can connect
    // SOMAXCONN is the maximum number of sockets we will allow. As current defined it is 128
    listen(socketServer, SOMAXCONN);

    std::ostringstream ss;
    ss << "\n*** Listening on ADDRESS: "
        << inet_ntoa(saddr.sin_addr)
        << " PORT: " << ntohs(saddr.sin_port)
        << " ***\n\n";
    std::cout << ss.str() << std::endl;

    char buffer[30720] = {0};
    int recievedBytes;
    while (true) {

        new_socket = accept(socketServer,
                            (struct sockaddr*)&caddr,
                            (socklen_t*)&caddrSize);
        if (new_socket < 0)
            {
                std::ostringstream ss;
                ss << "Server failed to accept incoming connection from ADDRESS: "
                   << inet_ntoa(saddr.sin_addr) << "; PORT: "
                   << ntohs(saddr.sin_port);
                std::cout << ss.str() << std::endl;
            }

        std::cout << "[SERVER] Client Connected Sucess" << std::endl;

        // We are now going to recieve some bytes
        // Try to find out "who" is the client
        char hostClient[NI_MAXHOST];
        // Sometimes ports are called by services. like DB connections have specific ports (e.g 5432 for Postgres)
        char portClient[NI_MAXSERV];
        // We don't know where it was in the memory before we create those variables so we need to clean
        // the memory. we will set all the memory to zero
        memset(hostClient, 0, NI_MAXHOST);
        memset(hostClient, 0, NI_MAXSERV);


        // FIGURE OUT WHO THE CLIENT ACTUALLY IS
        // We don't know if we are going to get the client, so we may or may not get the client name
        // This is how we figure out who our client is
        if (getnameinfo((sockaddr*) &caddr,
                        sizeof(caddr),
                        hostClient,
                        NI_MAXHOST,
                        portClient,
                        NI_MAXSERV,
                        0)) {
            std::cout << " -- " << hostClient << " connected to port " << portClient << std::endl;
        } else {
            inet_ntop(AF_INET,
                    &caddr.sin_addr,
                    hostClient,
                    NI_MAXHOST);
            std::cout << " -- " << hostClient << " is connected to the port " << ntohs(caddr.sin_port) << std::endl;
        }

        // read() is equivalent to recv() with a flags parameter of 0. Other values for the flags parameter change the behaviour of recv(). Similarly, write() is equivalent to send() with flags == 0.
        // recievedBytes = read(m_new_socket, buffer, BUFFER_SIZE);
        recievedBytes = recv(new_socket,
                            buffer,
                            4096,
                            0);

        if (recievedBytes < 0) {
            std::cerr << "Failed to read bytes from client socket connection";
            break;
        } else if (recievedBytes == 0) {
            std::cout << "Client Disconnected" << std::endl;
            break;
        }


        // So now that we have recieved bytes, lets response to the client
        // ssize_t write(int fd, const void *buf, size_t count);
        // Mac OS X docs: https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/write.2.html
        // ssize_t write(int fildes, const void *buf, size_t nbyte);
        // Windows does not have write(), so we use send() instead
        // send(socketClient,
        //    buff,
        //    sizeInBytesOfReceivedData + 1,
        //    0);

        // I guess when to use write() versus send()
        // int send(int sockfd, const void *msg, int len, int flags);

        //const char * msg = "GET / HTTP/1.0";
        //int len, bytes_sent;
        //len = strlen(msg);
        std::string file_buffer;
        load_file(file_buffer);
        //char arr[200]="HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
        char result[500];
        const char * header = "HTTP/1.1 200 OK\nContent-Type:application/json\n\n";
        std::strcpy(result, header);
        std::strcat(result, file_buffer.c_str());
        int send_res = send(new_socket,result, strlen(result),0);
        //bytes_sent = send(new_socket, msg, len, 0);
        close(new_socket);

        // So we closed the socket. My next to do is to use POSIX threads to incorporate multithreading.
    }

    return 0;
}
