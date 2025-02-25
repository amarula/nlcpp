/** @file
 *  @brief Route netlink
 */

#pragma once

#include <nlcpp/netlink.h>

struct rtnl_addr;
struct rtnl_link;
struct rtnl_nexthop;
struct rtnl_route;

namespace nl {

using std::optional;

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

	/// Get operational status
	uint8_t operstate() const;
	/// Set operational status
	RouteLink & operstate(uint8_t);

	/// Get the raw libnll rtnl_link pointer
	rtnl_link * get() { return link; }
	/// Get the raw libnll rtnl_link pointer
	const rtnl_link * get() const { return link; }

private:
	rtnl_link * link;
};

/// Hop object for routing
class NextHop
{
public:
	NextHop();
	NextHop(const NextHop &);
	NextHop & operator=(const NextHop &);
	NextHop(NextHop &&);
	NextHop & operator=(NextHop &&);
	~NextHop();

	/// Get weight
	uint8_t weight() const;
	/// Set weight
	NextHop & weight(uint8_t);

	/// Get interface index
	int ifindex() const;
	/// Set interface index
	NextHop & ifindex(int index);

	/// Get gateway address
	optional<Address> gateway() const;
	/// Set gateway address
	NextHop & gateway(optional<Address>);

	/// Get the raw libnl rtnl_nexthop pointer
	const rtnl_nexthop * get() const { return nexthop; }
	rtnl_nexthop * take();

private:
	rtnl_nexthop * nexthop;
};

/// Netlink route
class Route
{
public:
	Route();
	Route(rtnl_route *);
	Route(const Route &) = delete;
	Route & operator=(const Route &) = delete;
	Route(Route &&);
	Route & operator=(Route &&);
	~Route();

	/// Get scope
	uint8_t scope() const;
	/// Set scope
	Route & scope(uint8_t);

	/// Get destination address
	Address dst() const;
	/// Set destination address
	Route & dst(Address &&);

	/// Add next hop
	Route & add(const NextHop &);
	/// Add next hop
	Route & add(NextHop &&);

	const rtnl_route * get() const { return route; }

private:
	rtnl_route * route;
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
	RouteSocket & add(const RouteAddress &);

	/// Delete matched protocol address(es)
	/**
	 * Deletes all addresses matched by given RouteAddress object.
	 */
	RouteSocket & del(const RouteAddress &);

	/// Add new route
	RouteSocket & add(const Route &);

	/// Delete route
	RouteSocket & del(const Route &);
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
