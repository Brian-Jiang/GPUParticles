#ifndef USOCKET_H
#define USOCKET_H


#if PE_PLAT_IS_PSVITA
#elif PE_PLAT_IS_PS3
#elif PE_PLAT_IS_PS4


#else

/*=========================================================================*\
* Socket compatibilization module for Unix
* LuaSocket toolkit
*
* RCS ID: $Id: usocket.h,v 1.7 2005/10/07 04:40:59 diego Exp $
\*=========================================================================*/

/*=========================================================================*\
* BSD include files
\*=========================================================================*/
/* error codes */
#include <errno.h>

#if PE_PLAT_IS_PSVITA
#elif PE_PLAT_IS_PS4
#else
/* close function */
#include <unistd.h>
/* fnctnl function and associated constants */
#include <fcntl.h>

/* struct sockaddr */
#include <sys/types.h>
/* socket function */
#include <sys/socket.h>
/* struct timeval */
#include <sys/time.h>
/* gethostbyname and gethostbyaddr functions */
#include <netdb.h>

/* sigpipe handling */
//#include <signal.h>

/* IP stuff*/
#include <netinet/in.h>
#include <arpa/inet.h>
/* TCP options (nagle algorithm disable) */
#include <netinet/tcp.h>

#endif

typedef int t_socket;
typedef t_socket *p_socket;

#if PE_PLAT_IS_PSVITA
#elif PE_PLAT_IS_PS4
#else
	/* errors */
	#define peusock_errno errno
	#define socket_h_errno h_errno
	#define peusock_EAGAIN EAGAIN
	#define peusock_EINTR EINTR
	#define peusock_EINPROGRESS EINPROGRESS
	#define peusock_ECONNABORTED ECONNABORTED
	#define peusock_EPIPE EPIPE

    #define peusock_accept accept

#endif

#define SOCKET_INVALID (-1)
#endif
#endif /* USOCKET_H */
