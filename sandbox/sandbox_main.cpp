// This is very basic implementation I did without really knowing what is going on under the hood.
// I just mangled together code from various resources. This is part of my learning process.
// Resources I used:
// https://osasazamegbe.medium.com/showing-building-an-http-server-from-scratch-in-c-2da7c0db6cb7
// https://stackoverflow.com/questions/61936165/send-html-through-socket-in-c
// https://github.com/JeffreytheCoder/Simple-HTTP-Server
// https://www.youtube.com/watch?v=F3iIGUiW27Q
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
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
// We cannot use namespace std; We need to manually put "std::" before every method or class that uses it
//
const char *get_file_extension(const char *file_name) {
    const char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name) {
        return "";
    }
    return dot + 1;
}


const char *get_mime_type(const char *file_ext) {
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) {
        return "text/html";
    } else if (strcasecmp(file_ext, "txt") == 0) {
        return "text/plain";
    } else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) {
        return "image/jpeg";
    } else if (strcasecmp(file_ext, "png") == 0) {
        return "image/png";
    } else {
        return "application/octet-stream";
    }
}

void build_http_response(const char *file_name,
                        const char *file_ext,
                        char *response,
                        size_t *response_len) {
    // build HTTP header
    const char *mime_type = get_mime_type(file_ext);
    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(header, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "\r\n",
             mime_type);

    // if file not exist, response is 404 Not Found
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) {
        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n"
                 "\r\n"
                 "404 Not Found");
        *response_len = strlen(response);
        return;
    }

    // get file size for Content-Length
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    off_t file_size = file_stat.st_size;

    // copy header to response buffer
    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    // copy file to response buffer
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd,
                            response + *response_len,
                            BUFFER_SIZE - *response_len)) > 0) {
        *response_len += bytes_read;
    }
    free(header);
    close(file_fd);
}

char *url_decode(const char *src) {
    size_t src_len = strlen(src);
    char *decoded = (char *)malloc(src_len + 1);
    size_t decoded_len = 0;

    // decode %2x to hex
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        } else {
            decoded[decoded_len++] = src[i];
        }
    }

    // add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}


void *handle_client(void *arg) {
    std::cout << "Handle Client Request" << std::endl;
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // receive request data from client and store into buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        // check if request is GET
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0) {
            // extract filename from request and decode URL
            buffer[matches[1].rm_eo] = '\0';
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);
            std::cout << file_name << std::endl;
            // get file extension
            char file_ext[32];
            strcpy(file_ext, get_file_extension(file_name));

            // build HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            build_http_response(file_name, file_ext, response, &response_len);

            // send HTTP response to client
            send(client_fd, response, response_len, 0);

            free(response);
            free(file_name);
        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

int main(int argc, const char * argv[]) {
    std::cout << "Hello World\n";

    // SERVER
    // AF_INET we are going to use IPV4. If we wanted to use IPv6 we would do AF_INET6
    // Side note, AF_INET and PF_INET are the exact same thing. One is Address Family the other is Protocol Family
    // INADDR_ANY any address we are using on our server, we will be using this server. Basically the IP for the server

    //Server address
    struct sockaddr_in saddr = {
        .sin_family = AF_INET, // Address family. IPv4 in this case.
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };
    int option = 1;
    int saddrSize = sizeof(saddr);
    int socketServer = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM means TCP. If we want to use UDP change to SOCK_DGRAM

    setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
    if (socketServer == -1) {
        std::cerr << "Not able to create socket. Exiting..." << std::endl;
        return -1;
    }


    // CLIENT
    struct sockaddr_in caddr;
    socklen_t caddrSize = sizeof(caddr);
    int socketClient;


    // Binding out socket to the IP and PORT
    // bind() comes from <sys/socket.h>
    bind(socketServer, (struct sockaddr*)&saddr, sizeof(saddr));

    // Put server in listening mode so client can connect
    // SOMAXCONN is the maximum number of sockets we will allow. As current defined it is 128
    listen(socketServer, SOMAXCONN);

    std::stringstream ss;
    ss << PORT;
    std::cout << "[SERVER] Listening on port " << ss.str() << std::endl;

    // Create the loop that will be executing as we are waiting for clients to connect
    char buff[4096];
    int sizeInBytesOfReceivedData;

    while (true) {
        // Accept connections from clients
        socketClient = accept(socketServer,
                            (struct sockaddr*)&caddr,
                            (socklen_t*)&caddrSize);
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


        // Recieve data from client
        // recv() comes from <sys/socket.h>
        sizeInBytesOfReceivedData = recv(socketClient,
                                        buff,
                                        4096,
                                        0);

        if (sizeInBytesOfReceivedData == -1) {
            std::cerr << "Error Recieving Data";
            break;
        } else if (sizeInBytesOfReceivedData == 0) {
            std::cout << "Client Disconnected" << std::endl;
            break;
        }

        // Now send data back to the client
        send(socketClient,
            buff,
            sizeInBytesOfReceivedData + 1,
            0);

        int *client_fd = (int *)malloc(sizeof(int));

        std::cout << std::string(buff, 0, sizeInBytesOfReceivedData) << std::endl;
        std::cout << "Creating new thread" << std::endl;
        pthread_t thread_id;

        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        std::cout << &thread_id << std::endl;
        pthread_detach(thread_id);
    }
        close(socketClient);


    return 0;
}
