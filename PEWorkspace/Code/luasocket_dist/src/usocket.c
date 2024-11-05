#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

/*=========================================================================*\
* Socket compatibilization module for Unix
* LuaSocket toolkit
*
* The code is now interrupt-safe.
* The penalty of calling select to avoid busy-wait is only paid when
* the I/O call fail in the first place. 
*
* RCS ID: $Id: usocket.c,v 1.38 2007/10/13 23:55:20 diego Exp $
\*=========================================================================*/
#include <string.h> 

#if APIABSTRACTION_IOS
#include <signal.h>
#endif

#include "socket.h"

/*-------------------------------------------------------------------------*\
* Wait for readable/writable/connected socket with timeout
\*-------------------------------------------------------------------------*/
#if PE_PLAT_IS_PSVITA

#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_C        (WAITFD_R|WAITFD_W)
#define MAX_EVENTS	(1)

int socket_waitfd(p_socket ps, int sw, p_timeout tm) {
	/*fd_set rfds, wfds, *rp, *wp;*/
	/*struct timeval tv, *tp;*/
	double t;
	SceNetId eid = -1;
	SceNetEpollEvent ev;
	int nevents;
	SceNetEpollEvent events[MAX_EVENTS];
	int ret;

	if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
	do {
		eid = sceNetEpollCreate("waitfd", 0);
		if (eid < 0) {
			printf("sceNetEpollCreate() failed (errno=%d)\n", peusock_errno);
			return sce_net_errno;
		}

		/* must set bits within loop, because select may have modifed them */
		memset(&ev, 0, sizeof(ev));
		if (sw & WAITFD_R)
			ev.events = SCE_NET_EPOLLIN;
		if (sw & WAITFD_W)
			ev.events = ev.events | SCE_NET_EPOLLOUT;
		ev.data.ext.id = *ps;

		ret = sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, *ps, &ev);
		if (ret < 0) {
			printf("sceNetEpollControl(ADD) failed (errno=%d)\n", sce_net_errno);
			return sce_net_errno;
		}

		/*do stuff*/
		
		t = timeout_getretry(tm); /* time left for the call*/
		/*
		tp = NULL;
		if (t >= 0.0) {
			tv.tv_sec = (int)t;
			tv.tv_usec = (int)((t-tv.tv_sec)*1.0e6);
			tp = &tv;
		}
		*/
		/*ret = socketselect(*ps+1, rp, wp, NULL, tp);*/
		nevents = sceNetEpollWait(eid, events, MAX_EVENTS, t >= 0? t: -1);

		/* reference code for deleting events from epoll */
#if 0
		/* delete SCE_NET_EPOLLOUT */
		memset(&ev, 0, sizeof(ev));
		ev.events = SCE_NET_EPOLLIN;
		ev.data.ext.id = s;
		ret = sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_MOD, *ps, &ev);
		if (ret < 0) {
			printf("sceNetEpollControl(MOD) failed (errno=%d)\n",
				sce_net_errno);
			return sce_net_errno;
		}
#endif
		/* destroy epoll object*/
		sceNetEpollDestroy(eid);

	} while (nevents == -1 && peusock_errno == peusock_EINTR);
	if (nevents == -1) return peusock_errno;
	if (nevents == 0) return IO_TIMEOUT;
	/*if (sw == WAITFD_C && FD_ISSET(*ps, &rfds)) return IO_CLOSED;*/
	return IO_DONE;
}

#else

#ifdef SOCKET_POLL
#include <sys/poll.h>

#define WAITFD_R        POLLIN
#define WAITFD_W        POLLOUT
#define WAITFD_C        (POLLIN|POLLOUT)
int socket_waitfd(p_socket ps, int sw, p_timeout tm) {
    int ret;
    struct pollfd pfd;
    pfd.fd = *ps;
    pfd.events = sw;
    pfd.revents = 0;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    do {
		int t = (int)(timeout_getretry(tm)*1e3);
		ret = poll(&pfd, 1, t >= 0? t: -1);
	} while (ret == -1 && peusock_errno == peusock_EINTR);
    if (ret == -1) return peusock_errno;
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && (pfd.revents & (POLLIN|POLLERR))) return IO_CLOSED;
    return IO_DONE;
}
#else

#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_C        (WAITFD_R|WAITFD_W)

int socket_waitfd(p_socket ps, int sw, p_timeout tm) {
#if LUA_SOCKET_USE_SELECT
    int ret;
    fd_set rfds, wfds, *rp, *wp;
    struct timeval tv, *tp;
    double t;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    do {
        /* must set bits within loop, because select may have modifed them */
        rp = wp = NULL;
        if (sw & WAITFD_R) { FD_ZERO(&rfds); FD_SET(*ps, &rfds); rp = &rfds; }
        if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*ps, &wfds); wp = &wfds; }
        t = timeout_getretry(tm);
        tp = NULL;
        if (t >= 0.0) {
            tv.tv_sec = (int)t;
            tv.tv_usec = (int)((t-tv.tv_sec)*1.0e6);
            tp = &tv;
        }
        #if APIABSTRACTION_PS3
			ret = socketselect(*ps+1, rp, wp, NULL, tp);
		#else
			ret = select(*ps+1, rp, wp, NULL, tp);  
		#endif
    } while (ret == -1 && peusock_errno == peusock_EINTR);
    if (ret == -1) return peusock_errno;
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*ps, &rfds)) return IO_CLOSED;
    return IO_DONE;
#else
	printf("socket_waitfd is not implemented for this platform! don't call it!");
	return IO_UNKNOWN;
#endif
}
#endif /*end of use select*/
#endif /* end of not ps vita*/


/*-------------------------------------------------------------------------*\
* Initializes module 
\*-------------------------------------------------------------------------*/
int g_libnetMemId = -1;
int socket_open(void) {
    /* instals a handler to ignore sigpipe or it will crash us */
     signal(SIGPIPE, SIG_IGN); // make sure we ignore SIGPIPE signal that is called whenever sockets close
    return 1;
}

/*-------------------------------------------------------------------------*\
* Close module 
\*-------------------------------------------------------------------------*/
int socket_close(void) {
    return 1;
}

/*-------------------------------------------------------------------------*\
* Close and inutilize socket
\*-------------------------------------------------------------------------*/
void socket_destroy(p_socket ps) {
    if (*ps != SOCKET_INVALID) {
        socket_setblocking(ps);
        close(*ps);
        *ps = SOCKET_INVALID;
    }
}

/*-------------------------------------------------------------------------*\
* Select with timeout control
\*-------------------------------------------------------------------------*/
#if !PE_PLAT_IS_PSVITA
int socket_select(t_socket n, fd_set *rfds, fd_set *wfds, fd_set *efds, 
        p_timeout tm) {
    int ret;
    do {
        struct timeval tv;
        double t = timeout_getretry(tm);
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
        /* timeout = 0 means no wait */
#if APIABSTRACTION_PS3
        ret = socketselect(n, rfds, wfds, efds, t >= 0.0 ? &tv: NULL);
#else
        ret = select(n, rfds, wfds, efds, t >= 0.0 ? &tv: NULL);
#endif
    } while (ret < 0 && peusock_errno == peusock_EINTR);
    return ret;
}
#endif
/*-------------------------------------------------------------------------*\
* Creates and sets up a socket
\*-------------------------------------------------------------------------*/
int socket_create(p_socket ps, int domain, int type, int protocol) {
    *ps = socket(domain, type, protocol);
    if (*ps != SOCKET_INVALID) return IO_DONE; 
    else return peusock_errno; 
}

/*-------------------------------------------------------------------------*\
* Binds or returns error message
\*-------------------------------------------------------------------------*/
int socket_bind(p_socket ps, SA *addr, socklen_t len) {
    int err = IO_DONE;
    socket_setblocking(ps);
    if (bind(*ps, addr, len) < 0) err = peusock_errno; 
    socket_setnonblocking(ps);
    return err;
}

/*-------------------------------------------------------------------------*\
* 
\*-------------------------------------------------------------------------*/
int socket_listen(p_socket ps, int backlog) {
    int err = IO_DONE; 
    socket_setblocking(ps);
    if (listen(*ps, backlog)) err = peusock_errno; 
    socket_setnonblocking(ps);
    return err;
}

/*-------------------------------------------------------------------------*\
* 
\*-------------------------------------------------------------------------*/
void socket_shutdown(p_socket ps, int how) {
    socket_setblocking(ps);
    shutdown(*ps, how);
    socket_setnonblocking(ps);
}

/*-------------------------------------------------------------------------*\
* Connects or returns error message
\*-------------------------------------------------------------------------*/
int socket_connect(p_socket ps, SA *addr, socklen_t len, p_timeout tm) {
    int err;
    /* avoid calling on closed sockets */
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    /* call connect until done or failed without being interrupted */
    do if (connect(*ps, addr, len) == 0) return IO_DONE;
    while ((err = peusock_errno) == peusock_EINTR);
    /* if connection failed immediately, return error code */
    if (err != peusock_EINPROGRESS && err != peusock_EAGAIN) return err; 
    /* zero timeout case optimization */
    if (timeout_iszero(tm)) return IO_TIMEOUT;
    /* wait until we have the result of the connection attempt or timeout */
    err = socket_waitfd(ps, WAITFD_C, tm);
    if (err == IO_CLOSED) {
        if (recv(*ps, (char *) &err, 0, 0) == 0) return IO_DONE;
        else return peusock_errno;
    } else return err;
}

/*-------------------------------------------------------------------------*\
* Accept with timeout
\*-------------------------------------------------------------------------*/
int socket_accept(p_socket ps, p_socket pa, SA *addr, socklen_t *len, p_timeout tm) {
    SA daddr;
    socklen_t dlen = sizeof(daddr);
    if (*ps == SOCKET_INVALID) return IO_CLOSED; 
    if (!addr) addr = &daddr;
    if (!len) len = &dlen;
    for ( ;; ) {
        int err;
        if ((*pa = peusock_accept(*ps, addr, len)) >= 0)
		{
			printf("LuaSocket(C++ core): socket_accept(): accept succeeded\n");
			return IO_DONE;
		}
        err = peusock_errno;
        if (err == peusock_EINTR) continue;

        if (err != peusock_EAGAIN && err != peusock_ECONNABORTED) return err; 
        
		if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    /* can't reach here */
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Send with timeout
\*-------------------------------------------------------------------------*/
int socket_send(p_socket ps, const char *data, size_t count, 
        size_t *sent, p_timeout tm)
{
    int err;
    *sent = 0;
    /* avoid making system calls on closed sockets */
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    /* loop until we send something or we give up on error */
    for ( ;; ) {
        long put = (long) send(*ps, data, count, 0);
        /* if we sent anything, we are done */
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = peusock_errno;
        /* send can't really return 0, but EPIPE means the connection was 
           closed */
        if (put == 0 || err == peusock_EPIPE) return IO_CLOSED;
        /* we call was interrupted, just try again */
        if (err == peusock_EINTR) continue;
        /* if failed fatal reason, report error */
        if (err != peusock_EAGAIN) return err;
        /* wait until we can send something or we timeout */
        if ((err = socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    }
    /* can't reach here */
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Sendto with timeout
\*-------------------------------------------------------------------------*/
int socket_sendto(p_socket ps, const char *data, size_t count, size_t *sent, 
        SA *addr, socklen_t len, p_timeout tm)
{
    int err;
    *sent = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        long put = (long) sendto(*ps, data, count, 0, addr, len);  
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = peusock_errno;
        if (put == 0 || err == peusock_EPIPE) return IO_CLOSED;
        if (err == peusock_EINTR) continue;
        if (err != peusock_EAGAIN) return err;
        if ((err = socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Receive with timeout
\*-------------------------------------------------------------------------*/
int socket_recv(p_socket ps, char *data, size_t count, size_t *got, p_timeout tm) {
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        long taken = (long) recv(*ps, data, count, 0);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        err = peusock_errno;
        if (taken == 0) return IO_CLOSED;
        if (err == peusock_EINTR) continue;

        if (err != peusock_EAGAIN) return err;

        if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err; 
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Recvfrom with timeout
\*-------------------------------------------------------------------------*/
int socket_recvfrom(p_socket ps, char *data, size_t count, size_t *got, 
        SA *addr, socklen_t *len, p_timeout tm) {
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        long taken = (long) recvfrom(*ps, data, count, 0, addr, len);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        err = peusock_errno;
        if (taken == 0) return IO_CLOSED;
        if (err == peusock_EINTR) continue;
        if (err != peusock_EAGAIN) return err; 
        if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err; 
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Put socket into blocking mode
\*-------------------------------------------------------------------------*/
void socket_setblocking(p_socket ps) {
#if APIABSTRACTION_IOS
    int flags = fcntl(*ps, F_GETFL, 0);
    flags &= (~(O_NONBLOCK));
    fcntl(*ps, F_SETFL, flags);
	
#else
	int val = 0;
	setsockopt(*ps, SOL_SOCKET, SO_NBIO, &val, sizeof(val));
#endif
}

/*-------------------------------------------------------------------------*\
* Put socket into non-blocking mode
\*-------------------------------------------------------------------------*/
void socket_setnonblocking(p_socket ps) {
#if APIABSTRACTION_IOS
    int flags = fcntl(*ps, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(*ps, F_SETFL, flags);
#else
	int val = 1;
	setsockopt(*ps, SOL_SOCKET, SO_NBIO, &val, sizeof(val));
#endif
}

/*-------------------------------------------------------------------------*\
* DNS helpers 
\*-------------------------------------------------------------------------*/
#if LUA_SOCKET_USE_DNS
int socket_gethostbyaddr(const char *addr, socklen_t len, struct hostent **hp)
{

	*hp = gethostbyaddr(addr, len, AF_INET);
    if (*hp) return IO_DONE;

    else if (socket_h_errno) return socket_h_errno;
    else if (peusock_errno) return peusock_errno;
   
    else return IO_UNKNOWN;
}

//	This functionality is not supported on psvita, need to use sceNetResolver if this function is needed..
int socket_gethostbyname(const char *addr, struct hostent **hp) {
    *hp = gethostbyname(addr);
    if (*hp) return IO_DONE;

	else if (socket_h_errno) return socket_h_errno;
    else if (peusock_errno) return peusock_errno;
   
    else return IO_UNKNOWN;
}
#endif

void socket_localaddr()
{
#if APIABSTRACTION_PC
    
	int status;
	struct addrinfo hints, *res, *p;
	char ipstr[INET6_ADDRSTRLEN];
    
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
	if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
    
	for(p = res;p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;
        
		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
        
		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		printf("  %s: %s\n", ipver, ipstr);
	}
    
	freeaddrinfo(res); // free the linked list
    
#endif
	/* looks like unix has a function that just returns ips. dont see it on win32*/
#if 0
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];
    
	if(getifaddrs(&myaddrs) != 0)
	{
		perror("getifaddrs");
		exit(1);
	}
    
	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;
		if (!(ifa->ifa_flags & IFF_UP))
			continue;
        
		switch (ifa->ifa_addr->sa_family)
		{
            case AF_INET:
			{
				struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
				in_addr = &s4->sin_addr;
				break;
			}
                
            case AF_INET6:
			{
				struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
				in_addr = &s6->sin6_addr;
				break;
			}
                
            default:
                continue;
		}
        
		if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
		{
			printf("%s: inet_ntop failed!\n", ifa->ifa_name);
		}
		else
		{
			printf("%s: %s\n", ifa->ifa_name, buf);
		}
	}
    
	freeifaddrs(myaddrs);
	return 0;
#endif
}



/*-------------------------------------------------------------------------*\
* Error translation functions
* Make sure important error messages are standard
\*-------------------------------------------------------------------------*/
const char *socket_hoststrerror(int err) {
    if (err <= 0) return io_strerror(err);
    switch (err) {
        case HOST_NOT_FOUND: return "host not found";
        default: return "return hstrerror(err)" ; /*return hstrerror(err);*/
    }
}

const char *socket_strerror(int err) {
    if (err <= 0) return io_strerror(err);
    switch (err) {
        case EADDRINUSE: return "address already in use";
        case EISCONN: return "already connected";
        case EACCES: return "permission denied";
        case ECONNREFUSED: return "connection refused";
        case ECONNABORTED: return "closed";
        case ECONNRESET: return "closed";
        case ETIMEDOUT: return "timeout";
        default: return strerror(peusock_errno);
    }
}

const char *socket_ioerror(p_socket ps, int err) {
    (void) ps;
    return socket_strerror(err);
} 
