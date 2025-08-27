/** @file
 *  @brief Core netlink functionality
 */

#pragma once

#include <nlcpp/exception.h>

#include <cstdint>
#include <optional>
#if __cplusplus >= 202002L
#include <ranges>
#endif
#include <string>

struct nl_addr;
struct nl_cache;
struct nl_cache_mngr;
struct nl_object;
struct nl_sock;

namespace nl {

using std::optional;
using std::string;

/// Base class for netlink sockets
class Socket
{
public:
	explicit Socket(int family);
	~Socket();

	/// Get the raw libnl nl_sock pointer
	const nl_sock * get() const { return sock; }

protected:
	nl_sock * sock;
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
