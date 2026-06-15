#pragma once

#if defined(NN_NINTENDO_SDK)

// POSIX wrapper implementations for the NX Switch

#define NX_VIRTUAL_INTERFACE_NAME "NX"
#include "nn/socket.h"

// Compatibility layer (used to map ioctl FIONBIO to fcntl O_NONBLOCK)
#ifndef FIONBIO
#define FIONBIO		0x5421
#endif // FIONBIO

void SetAdHocExclusiveMode(int value);

extern "C" uint16_t htons(uint16_t hostshort);

extern "C" uint16_t ntohs(uint16_t netshort);

extern "C" uint32_t htonl(uint32_t hostlong);

extern "C" uint32_t ntohl(uint32_t netlong);

extern "C" int inet_pton(int af, const char *src, void *dst);

extern "C" char *inet_ntoa(struct in_addr inp);

extern "C" in_addr_t inet_addr(const char *cp);

extern "C" const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

extern "C" long sysconf(int name);

extern "C" unsigned int if_nametoindex(const char *ifname);

extern "C" char *if_indextoname(unsigned int ifindex, char *ifname);

extern "C" int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);

extern "C" struct hostent* gethostbyname(const char* name);

extern "C" void freeaddrinfo(struct addrinfo *res);

extern "C" int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags);

extern "C" int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

extern "C" int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

extern "C" int gethostname(char *name, size_t len);

extern "C" int fcntl(int fd, int cmd, ...);

//extern "C" int ioctl(int fd, unsigned long request, ... /* (void *) */);

extern "C" int socket(int domain, int type, int protocol);

extern "C" int bind(int socket, const struct sockaddr *address, socklen_t address_len);

extern "C" int listen(int socket, int backlog);

extern "C" int shutdown(int socket, int how);

extern "C" int accept(int socket, struct sockaddr *address, socklen_t *address_len);

extern "C" int socketpair(int domain, int type, int protocol, int sv[2]);

extern "C" int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

extern "C" int poll(struct pollfd *fds, nfds_t nfds, int timeout);

extern "C" ssize_t recv(int sockfd, void *buf, socklen_t len, int flags);

extern "C" ssize_t recvfrom(int sockfd, void *buf, socklen_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

extern "C" ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

extern "C" ssize_t send(int sockfd, const void *buf, socklen_t len, int flags);

extern "C" ssize_t sendto(int sockfd, const void *buf, socklen_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

extern "C" ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

extern "C" int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);

extern "C" int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

extern "C" int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

extern "C" int unlink(const char* pathname);

//extern "C" pthread_t pthread_self(void);
//
//extern "C" int pthread_detach(pthread_t thread);
//
//extern "C" int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

extern "C" int rand_bytes_NX(void *buf, int size);
extern "C" int rand_bytes_pseudo_NX(void *buf, int size);
extern "C" unsigned int randomNumber_NX();
extern "C" unsigned int geteuid();
extern "C" unsigned int getuid();
extern "C" unsigned int getegid();
extern "C" unsigned int getgid();

extern "C" void timeSinceEpoch(time_t* result);

/* WebRTC OpenSSL proxy defines & typedefs */

/* WebRTC OpenSSL proxy functions */

struct ifaddrs {
	struct ifaddrs* ifa_next;
	char* ifa_name;
	unsigned int ifa_flags;
	struct sockaddr* ifa_addr;
	struct sockaddr* ifa_netmask;
	// Real ifaddrs has broadcast, point to point and data members.
	// We don't need them (yet?).
};
extern "C" int getifaddrs(struct ifaddrs** result);
extern "C" void freeifaddrs(struct ifaddrs* addrs);

/* Macros */

#endif
