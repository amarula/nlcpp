#include <amarula/netlink.h>

#include <netlink/netlink.h>
#include <netlink/route/addr.h>
#include <netlink/route/route.h>
#include <stdexcept>

using std::nullopt;

namespace amarula::netlink {

Socket::Socket(int protocol)
{
	sock = nl_socket_alloc_cb(nl_cb_alloc(NL_CB_VERBOSE));
	if (!sock)
		throw std::runtime_error("Failed to allocate netlink socket");

	if (nl_connect(sock, protocol) < 0) {
		nl_socket_free(sock);
		throw std::runtime_error("Failed to connect netlink socket");
	}
}

Socket::~Socket()
{
	nl_socket_free(sock);
}

Address::Address(nl_addr * addr_):
	addr(addr_)
{
	if (addr)
		nl_addr_get(addr);
}

Address::Address(const string & str)
{
	int err = nl_addr_parse(str.c_str(), AF_UNSPEC, &addr);
	if (err < 0)
		throw std::invalid_argument("Failed to parse address \"" + str + "\": " + nl_geterror(err));
}

Address::Address(const Address & other):
	addr { nl_addr_clone(other.addr) }
{
	if (!addr)
		throw std::runtime_error("Failed to copy address");
}

Address & Address::operator=(const Address & other)
{
	nl_addr_put(addr);
	addr = nl_addr_clone(other.addr);
	return *this;
}

Address::Address(Address && other)
{
	addr = other.addr;
	other.addr = nullptr;
}

Address & Address::operator=(Address && other)
{
	nl_addr_put(addr);
	addr = other.addr;
	other.addr = nullptr;
	return *this;
}

Address::~Address()
{
	nl_addr_put(addr);
}

Cache::Cache(nl_cache * cache_):
	cache(cache_)
{
	nl_cache_get(cache);
}

Cache::Cache(const Cache & other):
	cache { other.cache }
{
	nl_cache_get(cache);
}

Cache & Cache::operator=(const Cache & other)
{
	nl_cache_put(cache);
	cache = other.cache;
	nl_cache_get(cache);
	return *this;
}

Cache::Cache(Cache && other):
	cache { other.cache }
{
	other.cache = nullptr;
}

Cache & Cache::operator=(Cache && other)
{
	nl_cache_put(cache);
	cache = other.cache;
	other.cache = nullptr;
	return *this;
}

Cache::~Cache()
{
	nl_cache_put(cache);
}

CacheManager::CacheManager(int protocol)
{
	int err = nl_cache_mngr_alloc(nullptr, protocol, NL_AUTO_PROVIDE, &mngr);
	if (err < 0)
		throw std::runtime_error(string("nl_cache_mngr_alloc failed: ") + nl_geterror(err));
}

CacheManager::~CacheManager()
{
	nl_cache_mngr_free(mngr);
}


////////////////////////////////////////////////////////////////////////////////
// Route

RouteAddress::RouteAddress():
	addr { rtnl_addr_alloc() }
{
	if (!addr)
		throw std::runtime_error("Failed to allocate route netlink address");
}

RouteAddress::RouteAddress(RouteAddress && other)
{
	addr = other.addr;
	other.addr = nullptr;
}

RouteAddress & RouteAddress::operator=(RouteAddress && other)
{
	rtnl_addr_put(addr);
	addr = other.addr;
	other.addr = nullptr;
	return *this;
}

RouteAddress::~RouteAddress()
{
	rtnl_addr_put(addr);
}

int RouteAddress::ifindex() const
{
	return rtnl_addr_get_ifindex(addr);
}

RouteAddress & RouteAddress::ifindex(int index)
{
	rtnl_addr_set_ifindex(addr, index);
	return *this;
}

int RouteAddress::family() const
{
	return rtnl_addr_get_family(addr);
}

RouteAddress & RouteAddress::family(int family)
{
	rtnl_addr_set_family(addr, family);
	return *this;
}

Address RouteAddress::local() const
{
	return Address(rtnl_addr_get_local(addr));
}

RouteAddress & RouteAddress::local(const Address & laddr)
{
	rtnl_addr_set_local(addr, const_cast<nl_addr*>(laddr.get()));
	return *this;
}

RouteLink::RouteLink():
	link { rtnl_link_alloc() }
{
	if (!link)
		throw std::runtime_error("Failed to allocate route netlink link");
}

RouteLink::RouteLink(rtnl_link * link_):
	link { link_ }
{
	if (link)
		nl_object_get(reinterpret_cast<nl_object *>(link));
}

RouteLink::RouteLink(RouteLink && other)
{
	link = other.link;
	other.link = nullptr;
}

RouteLink & RouteLink::operator=(RouteLink && other)
{
	rtnl_link_put(link);
	link = other.link;
	other.link = nullptr;
	return *this;
}

RouteLink::~RouteLink()
{
	rtnl_link_put(link);
}

optional<string> RouteLink::name() const
{
	if (const char * str = rtnl_link_get_name(link))
		return str;
	return nullopt;
}

int RouteLink::ifindex() const
{
	return rtnl_link_get_ifindex(link);
}

RouteSocket::RouteSocket():
	Socket(NETLINK_ROUTE)
{}

RouteSocket::~RouteSocket() = default;

void RouteSocket::add(const RouteAddress & addr)
{
	int err = rtnl_addr_add(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	if (err < 0)
		throw std::runtime_error(string("rtnl_addr_add failed: ") + nl_geterror(err));
}

void RouteSocket::del(const RouteAddress & addr)
{
	int err = rtnl_addr_delete(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	if (err < 0)
		throw std::runtime_error(string("rtnl_addr_delete failed: ") + nl_geterror(err));
}

RouteCacheManager::RouteCacheManager():
	CacheManager(NETLINK_ROUTE)
{}

RouteCacheManager::~RouteCacheManager() = default;

RouteLinkCache RouteCacheManager::linkCache()
{
	nl_cache * cache;
	int err = nl_cache_mngr_add(mngr, "route/link", nullptr, nullptr, &cache);
	if (err < 0)
		throw std::runtime_error(string("nl_cache_mngr_add failed: ") + nl_geterror(err));

	return RouteLinkCache(cache);
}

RouteLink RouteLinkCache::getByName(const string & name)
{
	RouteLink link { rtnl_link_get_by_name(cache, name.c_str()) };
	if (link)
		rtnl_link_put(link.get()); // refcount incremented by rtnl_link_get_by_name
	return link;
}

RouteLinkCache::Iterator::Iterator(nl_object * obj_):
	obj(reinterpret_cast<rtnl_link *>(obj_))
{}

RouteLinkCache::Iterator::reference RouteLinkCache::Iterator::operator*() const
{
	return obj;
}

RouteLinkCache::Iterator::pointer RouteLinkCache::Iterator::operator->() const
{
	return &obj;
}

bool RouteLinkCache::Iterator::operator==(const Iterator & other) const
{
	return obj.get() == other.obj.get();
}

bool RouteLinkCache::Iterator::operator!=(const Iterator & other) const
{
	return obj.get() != other.obj.get();
}

RouteLinkCache::Iterator & RouteLinkCache::Iterator::operator++()
{
	obj = RouteLink(reinterpret_cast<rtnl_link *>(nl_cache_get_next(reinterpret_cast<nl_object *>(obj.get()))));
	return *this;
}

RouteLinkCache::Iterator RouteLinkCache::Iterator::operator++(int)
{
	Iterator tmp = std::move(*this);
	obj = RouteLink(reinterpret_cast<rtnl_link *>(nl_cache_get_next(reinterpret_cast<nl_object *>(tmp.obj.get()))));
	return tmp;
}

RouteLinkCache::Iterator RouteLinkCache::begin() const
{
	return Iterator(nl_cache_get_first(cache));
}

RouteLinkCache::Iterator RouteLinkCache::end() const
{
	return Iterator(nullptr);
}

}
