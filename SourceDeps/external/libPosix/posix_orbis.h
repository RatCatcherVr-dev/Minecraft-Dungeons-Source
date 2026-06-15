#pragma once

#if defined(__ORBIS__)

#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <kernel.h>
#include <net.h>
#include <libnet/epoll.h>

/*
 * Dummy, PS4 doesn't use declare or use IPV6
  */
struct in6_addr {
	union {
		unsigned char __u6_addr8[16];
		unsigned short __u6_addr16[8];
		unsigned long __u6_addr32[4];
	} __u6_addr;			/* 128-bit IP6 address */
};

#define s6_addr   __u6_addr.__u6_addr8

struct sockaddr_in6 {
	uint8_t		sin6_len;	/* length of this struct */
	sa_family_t	sin6_family;	/* AF_INET6 */
	in_port_t	sin6_port;	/* Transport layer port # */
	uint32_t	sin6_flowinfo;	/* IP6 flow information */
	struct in6_addr	sin6_addr;	/* IP6 address */
	uint32_t	sin6_scope_id;	/* scope zone index */
};

// This file contains minimal Orbis implementations of missing platform POSIX functions
// for functionality required by dependent libraries (typically RakNet or openssl) for the PS4.

// Since this is POSIX functionality, most of these functions are extern "C"
// Where necessary we also provided structures or constants needed by the implementations that
// are also missing from the standard libraries

// *NOTE: crypto/async is using the "null" implementation.  We could use
//     sceFibers if necessary.

#define	AI_ADDRCONFIG   0x00000400

// Error values for `getaddrinfo' function.  */
# define EAI_ADDRFAMILY 1   // Address family for NAME not supported.
# define EAI_AGAIN      2   // Temporary failure in name resolution.
# define EAI_BADFLAGS   3
# define EAI_FAIL       4
# define EAI_FAMILY     5
# define EAI_MEMORY     6
# define EAI_NODATA     7   // No address associated with NAME.
# define EAI_NONAME     8   // NAME or SERVICE is unknown.
# define EAI_SERVICE    9   // Service not supported
# define EAI_SOCKTYPE   10  // Unsupported socket type
# define EAI_SYSTEM     11  // Unknown error


/// Possible values of h_errno, set by gethostbyname
#define	HOST_NOT_FOUND  1       // Authoritative Answer Host not found.
#define	TRY_AGAIN       2       // Non-Authoritative Host not found, or SERVERFAIL.
#define	NO_RECOVERY     3       // Non recoverable errors, FORMERR, REFUSED, NOTIMP.
#define	NO_DATA         4       // Valid name, no data record of requested type.
#define	NO_ADDRESS      NO_DATA // No address, look for MX record.

#define SIOCGIFCONF    0x8912
#define SIOCGIFNETMASK 0x891b

#define FIONBIO        0x5421
#define FIONREAD       0x541B

// getnameinfo
#define	NI_MAXHOST 1025
#define	NI_MAXSERV 32

#define	NI_NOFQDN      0x00000001
#define	NI_NUMERICHOST 0x00000002
#define	NI_NAMEREQD    0x00000004
#define	NI_NUMERICSERV 0x00000008
#define	NI_DGRAM       0x00000010

#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif // IFNAMSIZ

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif // IFNAMSIZ

#define IFF_UP          0x1
#define IFF_LOOPBACK    0x8
#define IFF_DRV_RUNNING 0x40
#define IFF_MULTICAST   0x1000
#define IFF_RUNNING		IFF_DRV_RUNNING

extern int h_errno;

struct hostent {
	char *h_name;        // Official name of host
	char **h_aliases;    // Alias list
	short h_addrtype;    // Host address type
	short h_length;      // Length of address
	char **h_addr_list;  // List of addresses
};
#define h_addr h_addr_list[0] /* for backward compatibility */

struct servent {
	char *s_name;
	char **s_aliases;
	short s_port;
	char *s_proto;
};

struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t        ai_addrlen;
	struct sockaddr *ai_addr;
	char            *ai_canonname;
	struct addrinfo *ai_next;
};

struct ifmap {
	unsigned long   mem_start;
	unsigned long   mem_end;
	unsigned short  base_addr;
	unsigned char   irq;
	unsigned char   dma;
	unsigned char   port;
};

struct ifreq_buffer {
	size_t      length;
	void       *buffer;
};

struct ifreq {
	char            ifr_name[IF_NAMESIZE];     /* if name, e.g. "en0" */
	union {
		struct      sockaddr ifru_addr;
		struct      sockaddr ifru_dstaddr;
		struct      sockaddr ifru_broadaddr;
		struct      ifreq_buffer ifru_buffer;
		short       ifru_flags[2];
		short       ifru_index;
		int         ifru_jid;
		int         ifru_metric;
		int         ifru_mtu;
		int         ifru_phys;
		int         ifru_media;
		char*       ifru_data;
		int         ifru_cap[2];
		uint32_t    ifru_fib;
	} ifr_ifru;
#define ifr_addr        ifr_ifru.ifru_addr      /* address */
#define ifr_dstaddr     ifr_ifru.ifru_dstaddr   /* other end of p-to-p link */
#define ifr_broadaddr   ifr_ifru.ifru_broadaddr /* broadcast address */
#define ifr_buffer      ifr_ifru.ifru_buffer    /* user supplied buffer with its length */
#define ifr_flags       ifr_ifru.ifru_flags[0]  /* flags (low 16 bits) */
#define ifr_flagshigh   ifr_ifru.ifru_flags[1]  /* flags (high 16 bits) */
#define ifr_jid         ifr_ifru.ifru_jid       /* jail/vnet */
#define ifr_metric      ifr_ifru.ifru_metric    /* metric */
#define ifr_mtu         ifr_ifru.ifru_mtu       /* mtu */
#define ifr_phys        ifr_ifru.ifru_phys      /* physical wire */
#define ifr_media       ifr_ifru.ifru_media     /* physical media */
#define ifr_data        ifr_ifru.ifru_data      /* for use by interface */
#define ifr_reqcap      ifr_ifru.ifru_cap[0]    /* requested capabilities */
#define ifr_curcap      ifr_ifru.ifru_cap[1]    /* current capabilities */
#define ifr_index       ifr_ifru.ifru_index     /* interface index */
#define ifr_fib         ifr_ifru.ifru_fib       /* interface fib */
};

struct ifconf {
	int         ifc_len;                            /* size of associated buffer */
	union {
		char*   ifcu_buf;
		struct  ifreq *ifcu_req;
	} ifc_ifcu;
#define ifc_buf ifc_ifcu.ifcu_buf                   /* buffer address */
#define ifc_req ifc_ifcu.ifcu_req                   /* array of structures returned */
};

struct ifaddrs {
	struct ifaddrs  *ifa_next;    /* Next item in list */
	char            *ifa_name;    /* Name of interface */
	unsigned int     ifa_flags;   /* Flags from SIOCGIFFLAGS */
	struct sockaddr *ifa_addr;    /* Address of interface */
	struct sockaddr *ifa_netmask; /* Netmask of interface */
	union {
		struct sockaddr *ifu_broadaddr;
		/* Broadcast address of interface */
		struct sockaddr *ifu_dstaddr;
		/* Point-to-point destination address */
	} ifa_ifu;
#define              ifa_broadaddr ifa_ifu.ifu_broadaddr
#define              ifa_dstaddr   ifa_ifu.ifu_dstaddr
	void            *ifa_data;    /* Address-specific data */
};

struct pollfd {
	int   fd;         /* file descriptor */
	short events;     /* requested events */
	short revents;    /* returned events */
};

typedef unsigned long nfds_t;
//typedef SceNetEpollEvent epoll_event;

#define epoll_event SceNetEpollEvent
#define POLLIN SCE_NET_EPOLLIN
#define POLLPRI POLLIN
#define POLLOUT SCE_NET_EPOLLOUT
#define POLLERR SCE_NET_EPOLLERR
#define POLLHUP SCE_NET_EPOLLHUP
#define EPOLLIN SCE_NET_EPOLLIN
#define EPOLLOUT SCE_NET_EPOLLOUT

#define SO_NOSIGPIPE MSG_NOSIGNAL
#define SOMAXCONN 64

//const int max_iov_len = SCE_KERNEL_IOV_MAX;
#ifdef  __cplusplus
extern "C"
{
#endif
int orbisInitNetworkingLibraries();
int orbisFinalizeNetworkingLibraries();
int orbisGetSslCtxId();


int get_last_rtl_error();

long sysconf(int name);

int ioctl(int fd, unsigned long request, void *param1, ... /* arg */);

struct tm *gmtime_r(const time_t* time, struct tm * res);

in_addr_t inet_addr(const char *cp);
const char *inet_ntoa(struct in_addr in);

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags);

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);

int gethostname(char *name, size_t len);
struct hostent* gethostbyname(const char* name);
void getipbyname(const char *domainName, char ip[65]);
void getmyip(char ip[65]);
struct servent* getservbyname(const char *name, const char *proto);

uid_t getuid(void);

in_addr_t inet_addr(const char *cp);
void getipbyname(const char *domainName, char ip[65]);
int getifaddrs(struct ifaddrs **ifap);
void freeifaddrs(struct ifaddrs *ifa);
unsigned int if_nametoindex(const char *ifname);
char *if_indextoname(unsigned int ifindex, char *ifname);

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);
int sigfillset(sigset_t *set);
int pipe(int fildes[2]);

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int socketpair(int domain, int type, int protocol, int sv[2]);

int sockatmark(int sockfd);
char* getenv(char const* name);
#ifdef  __cplusplus
}
#endif
#endif
