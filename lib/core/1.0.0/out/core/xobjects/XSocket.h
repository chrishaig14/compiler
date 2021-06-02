//
// Created by chris on 30/5/21.
//

#ifndef XLANG_XSOCKET_H
#define XLANG_XSOCKET_H

#include <fstream>
#include "XObject.h"
#include "../Function.h"
#include "XString.h"
#include "../macros.h"

class XSocket : public XObject {
public:
    int fd;

    XSocket(int fd): XObject("Socket") {
        this->fd = fd;
    }
};

extern Function1 core_D_socket_D_Socket_D_accept_o;
extern TaggedObject* core_D_socket_D_Socket_D_accept;
TaggedObject* core_D_socket_D_Socket_D_accept_f(TaggedObject* socket);

extern Function2 core_D_socket_D_Socket_D_bind_o;
extern TaggedObject* core_D_socket_D_Socket_D_bind;
TaggedObject* core_D_socket_D_Socket_D_bind_f(TaggedObject* socket, TaggedObject* port);

extern Function2 core_D_socket_D_Socket_D_listen_o;
extern TaggedObject* core_D_socket_D_Socket_D_listen;
TaggedObject* core_D_socket_D_Socket_D_listen_f(TaggedObject* domain, TaggedObject* type);

extern Function2 core_D_socket_D_Socket_D_new_o;
extern TaggedObject* core_D_socket_D_Socket_D_new;
TaggedObject* core_D_socket_D_Socket_D_new_f(TaggedObject* domain, TaggedObject* type);

extern Function2 core_D_socket_D_Socket_D_recv_o;
extern TaggedObject* core_D_socket_D_Socket_D_recv;
TaggedObject* core_D_socket_D_Socket_D_recv_f(TaggedObject* socket, TaggedObject* num);

extern Function2 core_D_socket_D_Socket_D_send_o;
extern TaggedObject* core_D_socket_D_Socket_D_send;
TaggedObject* core_D_socket_D_Socket_D_send_f(TaggedObject* socket, TaggedObject* msg);
#endif //XLANG_XSOCKET_H
