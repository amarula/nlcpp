/** @file
 *  @brief Route netlink
 */

#pragma once

#include <nlcpp/netlink.h>

#include <linux/rtnetlink.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct rtnl_addr;
struct rtnl_link;
struct rtnl_nexthop;
struct rtnl_route;

namespace nl {

using std::function;
using std::optional;
using std::string;
using std::unique_ptr;
using std::vector;

enum class RouteScope
{
	UNIVERSE = RT_SCOPE_UNIVERSE,
	SITE = RT_SCOPE_SITE,
	LINK = RT_SCOPE_LINK,
	HOST = RT_SCOPE_HOST,
	NOWHERE = RT_SCOPE_NOWHERE
};

enum class RouteProtocol
{
	UNSPEC = RTPROT_UNSPEC,
	REDIRECT = RTPROT_REDIRECT, /// Route installed by ICMP redirects; not used by current IPv4
	KERNEL = RTPROT_KERNEL, /// Route installed by kernel
	BOOT = RTPROT_BOOT, /// Route installed during boot
	STATIC = RTPROT_STATIC, /// Route installed by administrator
	GATED = RTPROT_GATED, /// Apparently, GateD
	RA = RTPROT_RA, /// RDISC/ND router advertisements
	MRT = RTPROT_MRT, /// Merit MRT
	ZEBRA = RTPROT_ZEBRA, /// Zebra
	BIRD = RTPROT_BIRD, /// BIRD
	DNROUTED = RTPROT_DNROUTED, /// DECnet routing daemon
	XORP = RTPROT_XORP, /// XORP
	NTK = RTPROT_NTK, /// Netsukuku
	DHCP = RTPROT_DHCP, /// DHCP client
	MROUTED = RTPROT_MROUTED, /// Multicast daemon
	KEEPALIVED = RTPROT_KEEPALIVED, /// Keepalived daemon
	BABEL = RTPROT_BABEL, /// Babel daemon
	OPENR = RTPROT_OPENR, /// Open Routing (Open/R) Routes
	BGP = RTPROT_BGP, /// BGP Routes
	ISIS = RTPROT_ISIS, /// ISIS Routes
	OSPF = RTPROT_OSPF, /// OSPF Routes
	RIP = RTPROT_RIP, /// RIP Routes
	EIGRP = RTPROT_EIGRP, /// EIGRP Routes
};

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
	[[deprecated("Use RouteAddress(rtnl_addr *, ShareOwnershipOfPointer) instead")]]
		RouteAddress(rtnl_addr *);
	RouteAddress(rtnl_addr *, TakeOwnershipOfPointer);
	RouteAddress(rtnl_addr *, ShareOwnershipOfPointer);
	RouteAddress(const RouteAddress &) = delete;
	RouteAddress & operator=(const RouteAddress &) = delete;
	RouteAddress(RouteAddress &&);
	RouteAddress & operator=(RouteAddress &&);
	~RouteAddress();

	using RawType = rtnl_addr;

	explicit operator bool() const { return bool(addr); }

	/// Get label
	optional<string> label() const;
	/// Set label
	RouteAddress & label(const string & value);

	/// Get interface index
	int ifindex() const;
	/// Set interface index
	RouteAddress & ifindex(int index);

	/// Get address family
	int family() const;
	/// Set address family
	RouteAddress & family(int family);

	/// Get scope
	RouteScope scope() const;
	/// Set scope
	RouteAddress & scope(RouteScope value);

	/// Get local address
	Address local() const;
	/// Set local address
	RouteAddress & local(const Address &);

	/// Get the raw libnl rtnl_addr pointer
	const rtnl_addr * get() const { return addr; }
	/// Get the raw libnl rtnl_addr pointer
	rtnl_addr * get() { return addr; }

private:
	rtnl_addr * addr;
};

/// Link (interface) object
class RouteLink
{
public:
	RouteLink();
	[[deprecated("Use RouteLink(rtnl_link *, ShareOwnershipOfPointer) instead")]]
		RouteLink(rtnl_link *);
	RouteLink(rtnl_link *, TakeOwnershipOfPointer);
	RouteLink(rtnl_link *, ShareOwnershipOfPointer);
	RouteLink(const RouteLink &) = delete;
	RouteLink & operator=(const RouteLink &) = delete;
	RouteLink(RouteLink &&);
	RouteLink & operator=(RouteLink &&);
	~RouteLink();

	using RawType = rtnl_link;

	explicit operator bool() const { return bool(link); }

	/// Get name of the link object, or nullopt if not specified
	optional<string> name() const;
	/// Set name of the link object
	RouteLink & name(const string &);

	/// Get flags of the link object
	unsigned int getFlags() const;
	/// Set flags of the link object
	RouteLink & setFlags(unsigned int);
	/// Unset flags of the link object
	RouteLink & unsetFlags(unsigned int);

	/// Get interface index of the link object, or 0 if not set
	int ifindex() const;
	/// Set interface index
	RouteLink & ifindex(int index);

	/// Get operational status
	uint8_t operstate() const;
	/// Set operational status
	RouteLink & operstate(uint8_t);

	/// Get type of the link object
	optional<string> type() const;
	/// Set type of the link object
	RouteLink & type(const string &);

	/// Get the raw libnl rtnl_link pointer
	rtnl_link * get() { return link; }
	/// Get the raw libnl rtnl_link pointer
	const rtnl_link * get() const { return link; }

private:
	rtnl_link * link;
};

/// Hop object for routing
class NextHop
{
public:
	NextHop();
	[[deprecated("Use NextHop(rtnl_nexthop *, CreateCopyFromPointer) instead")]]
		NextHop(rtnl_nexthop *);
	NextHop(rtnl_nexthop *, CreateCopyFromPointer);
	NextHop(rtnl_nexthop *, TakeOwnershipOfPointer);
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
	[[deprecated("Use Route(rtnl_route *, ShareOwnershipOfPointer) instead")]]
		Route(rtnl_route *);
	Route(rtnl_route *, TakeOwnershipOfPointer);
	Route(rtnl_route *, ShareOwnershipOfPointer);
	Route(const Route &) = delete;
	Route & operator=(const Route &) = delete;
	Route(Route &&);
	Route & operator=(Route &&);
	~Route();

	using RawType = rtnl_route;

	explicit operator bool() const { return bool(route); }

	/// Get scope
	RouteScope scope() const;
	/// Set scope
	Route & scope(RouteScope);

	/// Get protocol
	RouteProtocol protocol() const;
	/// Set protocol
	Route & protocol(RouteProtocol);

	/// Get priority
	uint32_t priority() const;
	/// Set priority
	Route & priority(uint32_t);

	/// Get family
	uint8_t family() const;
	/// Set family
	Route & family(uint8_t);

	/// Get destination address
	Address dst() const;
	/// Set destination address
	Route & dst(Address &&);

	/// Add next hop
	Route & add(const NextHop &);
	/// Add next hop
	Route & add(NextHop &&);

	/// Get list of next hops
	vector<NextHop> nexthops() const;

	/// Get the raw libnl rtnl_route pointer
	rtnl_route * get() { return route; }
	/// Get the raw libnl rtnl_route pointer
	const rtnl_route * get() const { return route; }

private:
	rtnl_route * route;
};

/// Socket opened for the route netlink protocol
class RouteSocket : public Socket
{
public:
	RouteSocket();

	/// Add new protocol address
	/**
	 * All the necessary information like address, prefix length or
	 * interface must be configured on the passed RouteAddress object.
	 */
	bool add(const RouteAddress &);

	/// Delete matched protocol address(es)
	/**
	 * Deletes all addresses matched by given RouteAddress object.
	 */
	bool del(const RouteAddress &);


	/// Add new link (interface)
	bool add(const RouteLink &);

	/// Delete existing link (interface)
	bool del(const RouteLink &);

	/// Change existing link (interface)
	bool change(const RouteLink & orig, const RouteLink & changes);


	/// Add new route
	bool add(const Route &);

	/// Delete route
	bool del(const Route &);
};

template<class T>
class TypedCache : public Cache
{
protected:
	TypedCache(nl_cache * cache_, ShareOwnershipOfPointer tag): Cache(cache_, tag) {}

public:
	class Iterator : public Cache::Iterator
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type        = const T;
		using pointer           = value_type *;
		using reference         = value_type &;

		Iterator(nl_object * obj);
		Iterator(nl_object * obj, nl_object * filter);
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
		T obj;
		T filter;
	};

	Iterator begin() const;
	Iterator end() const;
};

extern template class TypedCache<RouteAddress>;

/// Cache containing information about network addresses
/**
 * The object can be iterated to get individual addresses.
 */
class RouteAddressCache : public TypedCache<RouteAddress>
{
	explicit RouteAddressCache(nl_cache * cache_, ShareOwnershipOfPointer tag,
			unique_ptr<vector<function<void(const RouteAddress &, Action)>>> && cbs):
		TypedCache(cache_, tag),
		callbacks(move(cbs))
	{}

	friend class RouteCacheManager;

public:
	class Filtered
	{
	public:
		Filtered(const RouteAddressCache & cache, RouteAddress && filter):
			cache(cache), filter(std::move(filter)) {}

		Iterator begin() const;
		Iterator end() const;

	private:
		const RouteAddressCache & cache;
		RouteAddress filter;
	};

	/// Get iterable object listing addresses matching given filter
	Filtered filter(RouteAddress && filter) const;

	void watch(function<void(const RouteAddress &, Action)> callback);

protected:
	unique_ptr<vector<function<void(const RouteAddress &, Action)>>> callbacks;
};

extern template class TypedCache<RouteLink>;

/// Cache containing information about available links (interfaces)
/**
 * The object can be iterated to get individual links.
 */
class RouteLinkCache : public TypedCache<RouteLink>
{
	explicit RouteLinkCache(nl_cache * cache_, ShareOwnershipOfPointer tag):
		TypedCache(cache_, tag) {}

	friend class RouteCacheManager;
public:
	/// Lookup link in cache by link name
	RouteLink getByName(const string & name) const;
};

extern template class TypedCache<Route>;

/// Cache containing information about available routes
/**
 * The object can be iterated to get individual links.
 */
class RouteCache : public TypedCache<Route>
{
	explicit RouteCache(nl_cache * cache_, ShareOwnershipOfPointer tag):
		TypedCache(cache_, tag) {}

	friend class RouteCacheManager;
public:
};

/// Cache manager for the routing netlink cache
class RouteCacheManager : public CacheManager
{
public:
	RouteCacheManager();

	/// Get cache for address objects
	RouteAddressCache addressCache();

	/// Get cache for link objects
	RouteLinkCache linkCache();

	/// Get cache for route objects
	RouteCache routeCache();
};

}
