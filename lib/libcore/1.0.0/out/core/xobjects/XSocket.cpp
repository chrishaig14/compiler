//
// Created by chris on 30/5/21.
//

#include "XSocket.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

DEFINE_FUNCTION(1, core_D_socket_D_Socket_D_accept)

TaggedObject* core_D_socket_D_Socket_D_accept_f(TaggedObject* socket) {
    XSocket* sock = CAST(socket, XSocket);
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    int new_sock = accept(sock->fd, (sockaddr*) &addr, (socklen_t*) &addrlen);
    return NEW(XSocket, new_sock);
}

DEFINE_FUNCTION(2, core_D_socket_D_Socket_D_new)

TaggedObject* core_D_socket_D_Socket_D_new_f(TaggedObject* domain, TaggedObject* type) {
    int fd = socket(GET_INT(domain), GET_INT(type), 0);
    if (fd == -1) {
        throw std::runtime_error("Could not create socket");
    }
    XSocket* sock = CAST(NEW(XSocket, fd), XSocket);
    GC::declare(TAG(sock));
    return TAG(sock);
}

DEFINE_FUNCTION(2, core_D_socket_D_Socket_D_bind)

TaggedObject* core_D_socket_D_Socket_D_bind_f(TaggedObject* socket, TaggedObject* port) {
    XSocket* sock = CAST(socket, XSocket);
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(GET_INT(port));
    int r = bind(sock->fd, (sockaddr*) &addr, sizeof(addr));
    std::cout << "r is :" << r << std::endl;
    return MAKE_BOOL(r == 0);
}

DEFINE_FUNCTION(2, core_D_socket_D_Socket_D_listen)

TaggedObject* core_D_socket_D_Socket_D_listen_f(TaggedObject* socket, TaggedObject* num) {
    XSocket* sock = CAST(socket, XSocket);
    listen(sock->fd, GET_INT(num));
    return nullptr;
}

DEFINE_FUNCTION(2, core_D_socket_D_Socket_D_recv)

TaggedObject* core_D_socket_D_Socket_D_recv_f(TaggedObject* socket, TaggedObject* num) {
    XSocket* sock = CAST(socket, XSocket);
    char* buf = (char*) malloc(GET_INT(num) + 1);
    int n = recv(sock->fd, buf, GET_INT(num), 0);
    buf[n] = '\0';
    return NEW(XString, buf);
}

DEFINE_FUNCTION(2, core_D_socket_D_Socket_D_send)

TaggedObject* core_D_socket_D_Socket_D_send_f(TaggedObject* socket, TaggedObject* msg) {
    XSocket* sock = CAST(socket, XSocket);
    size_t nsent = 0;
    size_t num = CAST(msg, XString)->s.size();
    while (nsent < num) {
        size_t n = send(sock->fd, CAST(msg, XString)->s.c_str() + nsent, num, 0);
        if (n == -1) {
            throw std::runtime_error("Error sending!");
        }
        nsent += n;
    }
    return MAKE_INT(nsent);
}

TaggedObject* core_D_socket_D_SocketDomain_AF_INET = MAKE_INT(2);
TaggedObject* core_D_socket_D_SocketType_SOCK_STREAM = MAKE_INT(1);