/** @file
 *  @brief Core netlink functionality
 */

#pragma once

#include <nlcpp/exception.h>

#include <cstdint>
#include <functional>
#include <optional>
#if __cplusplus >= 202002L
#include <ranges>
#endif
#include <string>

struct nl_addr;
struct nl_cache;
struct nl_cache_mngr;
struct nl_cb;
struct nl_msg;
struct nl_object;
struct nl_sock;
struct nlmsgerr;
struct sockaddr_nl;

namespace nl {

using std::function;
using std::optional;
using std::string;

class Message;

/// Base class for netlink sockets
class Socket
{
public:
	explicit Socket(int family);
	~Socket();

	/// Send given message and wait for ACK or FINISH.
	void sendMessageSync(const Message & message);

	/// Send given message and wait for ACK or FINISH, calling given
	/// callback for all valid replies.
	void sendMessageSync(const Message & message, function<void(Message)> callback);

	/// Join the specified multicast group
	void addMembership(int group);

	/// Leave the specified multicast group
	void dropMembership(int group);

	/// Receive messages from netlink socket, calling configured callback
	/** @return Number of received messages
	 */
	unsigned int receiveMessages();

	/// Get the underlying file descriptor of the socket, which can be used for polling
	int getFileDescriptor();

	/// Get the raw libnl nl_sock pointer
	const nl_sock * get() const { return sock; }
	nl_sock * get() { return sock; }

protected:
	static int finishCallbackWrapper(nl_msg * msg, void * arg);
	static int ackCallbackWrapper(nl_msg * msg, void * arg);
	static int validCallbackWrapper(nl_msg * msg, void * arg);
	static int errorCallbackWrapper(sockaddr_nl * saddr, nlmsgerr * err, void * arg);

	nl_sock * sock;
	nl_cb * callbacks;

	int32_t callStatus;
	function<void(Message)> validCallback;
};

/// Netlink address – represents network address along with its prefix length
class Address
{
public:
	Address(nl_addr * addr);
	explicit Address(const string & str);
	Address(const Address &);
	Address & operator=(const Address &);
	Address(Address &&);
	Address & operator=(Address &&);
	~Address();

	/// Get string representation of the address
	operator string() const;

#if __cplusplus >= 202002L
	/// Get binary representation of the address
	std::ranges::subrange<const uint8_t *> binary() const;
#endif

	/// Get prefix length
	unsigned int prefixlen() const;
	/// Set prefix length
	Address & prefixlen(unsigned int);

	/// Get the raw libnl nl_addr pointer
	const nl_addr * get() const { return addr; }
	nl_addr * get() { return addr; }

private:
	nl_addr * addr;
};


/// Action types used in watch callbacks
enum class Action : int
{
	Unspecified,
	New,
	Delete,
	Get,
	Set,
	Change,
};

/// Base class for libnl caches
class Cache
{
public:
	Cache(nl_cache * cache);
	Cache(const Cache &);
	Cache & operator=(const Cache &);
	Cache(Cache &&);
	Cache & operator=(Cache &&);
	~Cache();

	/// Get the raw libnl nl_cache pointer
	const nl_cache * get() const { return cache; }
	nl_cache * get() { return cache; }

	class Iterator
	{
	};

protected:
	nl_cache * cache;
};

/// Base class for cache managers
class CacheManager
{
public:
	explicit CacheManager(int protocol);
	~CacheManager();

	int getFd() const;
	int poll(int timeout);
	int dataReady();

protected:
	nl_cache_mngr * mngr;
};

}
