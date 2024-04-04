#pragma once

#include <optional>
#include <string>

struct nl_addr;
struct nl_cache;
struct nl_cache_mngr;
struct nl_object;
struct nl_sock;

namespace amarula::netlink {

using std::optional;
using std::string;

// Core

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

	/// Get the raw libnl nl_addr pointer
	const nl_addr * get() const { return addr; }

private:
	nl_addr * addr;
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

protected:
	nl_cache_mngr * mngr;
};

}

// Route

struct rtnl_addr;
struct rtnl_link;

namespace amarula::netlink {

/// Address object used in the routing netlink protocol
/**
 * Contains information about local and potentially peer address, along with
 * associated interface and other information. It is used to set or retrieve
 * addresses on network interfaces or in routing tables.
 */
class RouteAddress
{
public:
	RouteAddress();
	RouteAddress(const RouteAddress &) = delete;
	RouteAddress & operator=(const RouteAddress &) = delete;
	RouteAddress(RouteAddress &&);
	RouteAddress & operator=(RouteAddress &&);
	~RouteAddress();

	/// Get interface index
	int ifindex() const;
	/// Set interface index
	RouteAddress & ifindex(int index);

	/// Get address family
	int family() const;
	/// Set address family
	RouteAddress & family(int family);

	/// Get local address
	Address local() const;
	/// Set local address
	RouteAddress & local(const Address &);

	/// Get the raw libnl rtnl_addr pointer
	const rtnl_addr * get() const { return addr; }

private:
	rtnl_addr * addr;
};

/// Link (interface) object
class RouteLink
{
public:
	RouteLink();
	RouteLink(rtnl_link *);
	RouteLink(const RouteLink &) = delete;
	RouteLink & operator=(const RouteLink &) = delete;
	RouteLink(RouteLink &&);
	RouteLink & operator=(RouteLink &&);
	~RouteLink();

	operator bool() const { return bool(link); }

	/// Get name of the link object, or nullopt if not specified
	optional<string> name() const;

	/// Get interface index of the link object, or 0 if not set
	int ifindex() const;

	/// Get the raw libnll rtnl_link pointer
	rtnl_link * get() { return link; }
	/// Get the raw libnll rtnl_link pointer
	const rtnl_link * get() const { return link; }

private:
	rtnl_link * link;
};

/// Socket opened for the route netlink protocol
class RouteSocket : public Socket
{
public:
	RouteSocket();
	~RouteSocket();

	/// Add new protocol address
	/**
	 * All the necessary information like address, prefix length or
	 * interface must be configured on the passed RouteAddress object.
	 */
	void add(const RouteAddress &);

	/// Delete matched protocol address(es)
	/**
	 * Deletes all addresses matched by given RouteAddress object.
	 */
	void del(const RouteAddress &);
};

/// Cache containing information about available links (interfaces)
/**
 * The object can be iterated to get individual links.
 */
class RouteLinkCache : public Cache
{
public:
	explicit RouteLinkCache(nl_cache * cache):
		Cache(cache) {}

	/// Lookup link in cache by link name
	RouteLink getByName(const string & name);

	class Iterator : public Cache::Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type        = const RouteLink;
		using pointer           = value_type *;
		using reference         = value_type &;

		Iterator(nl_object *);
		Iterator(Iterator &&) = default;
		Iterator & operator=(Iterator &&) = default;
		~Iterator() = default;

		reference operator*() const;
		pointer operator->() const;

		bool operator==(const Iterator &) const;
		bool operator!=(const Iterator &) const;

		Iterator & operator++();
		Iterator operator++(int);

	private:
		RouteLink obj;
	};

	Iterator begin() const;
	Iterator end() const;
};

/// Cache manager for the routing netlink cache
class RouteCacheManager : public CacheManager
{
public:
	RouteCacheManager();
	~RouteCacheManager();

	/// Get cache for link objects
	RouteLinkCache linkCache();
};

}
