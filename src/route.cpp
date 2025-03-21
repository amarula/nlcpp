#include <nlcpp/route.h>

#include <netlink/route/addr.h>
#include <netlink/route/route.h>

using std::make_unique;
using std::move;
using std::nullopt;

namespace nl {

RouteAddress::RouteAddress():
	addr { rtnl_addr_alloc() }
{
	if (!addr)
		throw std::runtime_error("Failed to allocate route netlink address");
}

RouteAddress::RouteAddress(rtnl_addr * addr_):
	addr { addr_ }
{
	if (addr)
		nl_object_get(reinterpret_cast<nl_object *>(addr));
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
	rtnl_addr_set_local(addr, Address(laddr).get());
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

uint8_t RouteLink::operstate() const
{
	return rtnl_link_get_operstate(link);
}

RouteLink & RouteLink::operstate(uint8_t value)
{
	rtnl_link_set_operstate(link, value);
	return *this;
}


NextHop::NextHop():
	nexthop { rtnl_route_nh_alloc() }
{
	if (!nexthop)
		throw std::runtime_error("Failed to allocate route netlink nexthop");
}

NextHop::NextHop(const NextHop & other):
	nexthop { rtnl_route_nh_clone(other.nexthop) }
{
	if (!nexthop)
		throw std::runtime_error("Failed to clone route netlink nexthop");
}

NextHop & NextHop::operator=(const NextHop & other)
{
	if (nexthop)
		rtnl_route_nh_free(nexthop);
	nexthop = rtnl_route_nh_clone(other.nexthop);
	if (!nexthop)
		throw std::runtime_error("Failed to clone route netlink nexthop");
	return *this;
}

NextHop::NextHop(NextHop && other)
{
	nexthop = other.nexthop;
	other.nexthop = nullptr;
}

NextHop & NextHop::operator=(NextHop && other)
{
	if (nexthop)
		rtnl_route_nh_free(nexthop);
	nexthop = other.nexthop;
	other.nexthop = nullptr;
	return *this;
}

NextHop::~NextHop()
{
	if (nexthop)
		rtnl_route_nh_free(nexthop);
}

uint8_t NextHop::weight() const
{
	return rtnl_route_nh_get_weight(nexthop);
}

NextHop & NextHop::weight(uint8_t value)
{
	rtnl_route_nh_set_weight(nexthop, value);
	return *this;
}

int NextHop::ifindex() const
{
	return rtnl_route_nh_get_ifindex(nexthop);
}

NextHop & NextHop::ifindex(int index)
{
	rtnl_route_nh_set_ifindex(nexthop, index);
	return *this;
}

optional<Address> NextHop::gateway() const
{
	if (auto addr = rtnl_route_nh_get_gateway(nexthop))
		return Address(addr);
	return nullopt;
}

NextHop & NextHop::gateway(optional<Address> addr)
{
	auto cloned = addr ? nl_addr_clone(addr->get()) : nullptr;
	rtnl_route_nh_set_gateway(nexthop, cloned);
	nl_addr_put(cloned);
	return *this;
}

rtnl_nexthop * NextHop::take()
{
	rtnl_nexthop * ptr = nexthop;
	nexthop = nullptr;
	return ptr;
}


Route::Route():
	route { rtnl_route_alloc() }
{
	if (!route)
		throw std::runtime_error("Failed to allocate route netlink route");
}

Route::Route(rtnl_route * route_):
	route { route_ }
{
	if (route)
		rtnl_route_get(route);
}

Route::Route(Route && other)
{
	route = other.route;
	other.route = nullptr;
}

Route & Route::operator=(Route && other)
{
	rtnl_route_put(route);
	route = other.route;
	other.route = nullptr;
	return *this;
}

Route::~Route()
{
	rtnl_route_put(route);
}

uint8_t Route::scope() const
{
	return rtnl_route_get_scope(route);
}

Route & Route::scope(uint8_t scope)
{
	rtnl_route_set_scope(route, scope);
	return *this;
}

Address Route::dst() const
{
	return Address(rtnl_route_get_dst(route));
}

Route & Route::dst(Address && addr)
{
	int err = rtnl_route_set_dst(route, addr.get());
	Exception::throwCode("rtnl_route_set_dst failed", err);
	return *this;
}

Route & Route::add(const NextHop & nexthop)
{
	return add(NextHop(nexthop));
}

Route & Route::add(NextHop && nexthop)
{
	rtnl_route_add_nexthop(route, nexthop.take());
	return *this;
}


RouteSocket::RouteSocket():
	Socket(NETLINK_ROUTE)
{}

RouteSocket::~RouteSocket() = default;

RouteSocket & RouteSocket::add(const RouteAddress & addr)
{
	int err = rtnl_addr_add(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	Exception::throwCode("rtnl_addr_add failed", err);
	return *this;
}

RouteSocket & RouteSocket::del(const RouteAddress & addr)
{
	int err = rtnl_addr_delete(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	Exception::throwCode("rtnl_addr_delete failed", err);
	return *this;
}

RouteSocket & RouteSocket::add(const Route & route)
{
	int err = rtnl_route_add(sock, const_cast<rtnl_route *>(route.get()), NLM_F_EXCL);
	Exception::throwCode("rtnl_route_add failed", err);
	return *this;
}

RouteSocket & RouteSocket::del(const Route & route)
{
	int err = rtnl_route_delete(sock, const_cast<rtnl_route *>(route.get()), 0);
	Exception::throwCode("rtnl_route_delete failed", err);
	return *this;
}

RouteCacheManager::RouteCacheManager():
	CacheManager(NETLINK_ROUTE)
{}

RouteCacheManager::~RouteCacheManager() = default;

RouteAddressCache RouteCacheManager::addressCache()
{
	nl_cache * cache;
	auto callbacks = make_unique<vector<function<void(const RouteAddress &, Action)>>>();
	auto cb = [](nl_cache *, nl_object * obj, int action, void * vcallbacks) {
		auto * callbacks =
			static_cast<vector<function<void(const RouteAddress &, Action)>> *>(vcallbacks);
		RouteAddress addr(reinterpret_cast<rtnl_addr *>(obj));

		for (const auto & cb : *callbacks)
			cb(addr, Action(action));
	};
	int err = nl_cache_mngr_add(mngr, "route/addr", cb, callbacks.get(), &cache);
	Exception::throwCode("nl_cache_mngr_add failed", err);
	return RouteAddressCache(cache, move(callbacks));
}

RouteLinkCache RouteCacheManager::linkCache()
{
	nl_cache * cache;
	int err = nl_cache_mngr_add(mngr, "route/link", nullptr, nullptr, &cache);
	Exception::throwCode("nl_cache_mngr_add failed", err);
	return RouteLinkCache(cache);
}

RouteAddressCache::Iterator::Iterator(nl_object * obj_):
	obj(reinterpret_cast<rtnl_addr *>(obj_))
{}

RouteAddressCache::Iterator::Iterator(nl_object * obj_, nl_object * filter_):
	obj(reinterpret_cast<rtnl_addr *>(obj_)),
	filter(reinterpret_cast<rtnl_addr *>(filter_))
{
	if (not nl_object_match_filter(obj_, filter_))
		++(*this);
}

RouteAddressCache::Iterator::reference RouteAddressCache::Iterator::operator*() const
{
	return obj;
}

RouteAddressCache::Iterator::pointer RouteAddressCache::Iterator::operator->() const
{
	return &obj;
}

bool RouteAddressCache::Iterator::operator==(const Iterator & other) const
{
	return obj.get() == other.obj.get();
}

bool RouteAddressCache::Iterator::operator!=(const Iterator & other) const
{
	return obj.get() != other.obj.get();
}

RouteAddressCache::Iterator & RouteAddressCache::Iterator::operator++()
{
	nl_object * next = reinterpret_cast<nl_object *>(obj.get());
	nl_object * raw_filter = reinterpret_cast<nl_object *>(filter.get());
	do {
		next = nl_cache_get_next(next);
	} while (next && raw_filter && not nl_object_match_filter(next, raw_filter));
	obj = RouteAddress(reinterpret_cast<rtnl_addr *>(next));
	return *this;
}

RouteAddressCache::Iterator RouteAddressCache::Iterator::operator++(int)
{
	nl_object * next = reinterpret_cast<nl_object *>(obj.get());
	nl_object * raw_filter = reinterpret_cast<nl_object *>(filter.get());
	do {
		next = nl_cache_get_next(next);
	} while (next && raw_filter && not nl_object_match_filter(next, raw_filter));

	Iterator tmp = move(*this);
	obj = RouteAddress(reinterpret_cast<rtnl_addr *>(next));
	filter = RouteAddress(reinterpret_cast<rtnl_addr *>(raw_filter));
	return tmp;
}

RouteAddressCache::Iterator RouteAddressCache::begin() const
{
	return Iterator(nl_cache_get_first(cache));
}

RouteAddressCache::Iterator RouteAddressCache::end() const
{
	return Iterator(nullptr);
}

RouteAddressCache::Filtered RouteAddressCache::filter(RouteAddress && filter) const
{
	return Filtered(*this, move(filter));
}

void RouteAddressCache::watch(function<void(const RouteAddress &, Action)> callback)
{
	callbacks->push_back(callback);
}

RouteAddressCache::Iterator RouteAddressCache::Filtered::begin() const
{
	return Iterator(nl_cache_get_first(const_cast<nl_cache *>(cache.get())),
			const_cast<nl_object *>(reinterpret_cast<const nl_object *>(filter.get())));
}

RouteAddressCache::Iterator RouteAddressCache::Filtered::end() const
{
	return Iterator(nullptr);
}

RouteLink RouteLinkCache::getByName(const string & name) const
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
	Iterator tmp = move(*this);
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
