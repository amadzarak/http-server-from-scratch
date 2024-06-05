// This is a reference of some of the structs defined in <netdb.h>
#include <iostream>

struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    size_t ao_addrlen;
    struct sockaddr * ai_addr;
    char * ai_cannonname;
    struct addrinfo * ai_next;
};

struct sockaddr {
    unsigned short sa_family;
    char sa_data[14];
};

// sockaddr_in -> "in" short for interet
struct in_addr {
  uint32_t s_addr;
};
struct sockaddr_in {
    short int sin_family;
    unsigned short int sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};



// For ipv6
struct in6_addr {
  unsigned char s6_addr[16];
};
struct sockaddr_in6 {
    u_int16_t sin6_family;
    u_int16_t sin6_port;
    struct in6_addr sin6_addr;
    u_int32_t sin6_scope_id;
};


struct sockaddr_storage {
    // sa_family_t in our case is an unsigned short
    // https://pubs.opengroup.org/onlinepubs/009604599/basedefs/sys/socket.h.html
    unsigned short ss_family;

};
