#ifndef INET_H 
#define INET_H 
/*=========================================================================*\
* Internet domain functions
* LuaSocket toolkit
*
* This module implements the creation and connection of internet domain
* sockets, on top of the socket.h interface, and the interface of with the
* resolver. 
*
* The function inet_aton is provided for the platforms where it is not
* available. The module also implements the interface of the internet
* getpeername and getsockname functions as seen by Lua programs.
*
* The Lua functions toip and tohostname are also implemented here.
*
* RCS ID: $Id: inet.h,v 1.16 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/
#include "lua.h"
#include "socket.h"
#include "timeout.h"

#if defined(_WIN32)
#define LUA_SOCKET_IMPLEMENT_ATON 1
#else
#define LUA_SOCKET_IMPLEMENT_ATON 0
#endif

int inet_open(lua_State *L);

const char *inet_trycreate(p_socket ps, int type);
const char *inet_tryconnect(p_socket ps, const char *address, 
        unsigned short port, p_timeout tm);
const char *inet_trybind(p_socket ps, const char *address, 
        unsigned short port);
/*pe-begin: akovalovs*/
/* adding this method for consistency. in lua socket, listen is performed straight on socket, without a inet_ function*/
const char *inet_trylisten(p_socket ps, int backlog);
/*pe-end*/
int inet_meth_getpeername(lua_State *L, p_socket ps);
int inet_meth_getsockname(lua_State *L, p_socket ps);

/*pe-begin: akovalovs*/
int pesocket_inet_aton(const char *cp, struct in_addr *inp);
/*pe-end*/
#endif /* INET_H */
