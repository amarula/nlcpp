#include <nlcpp/route.h>

#include <netlink/route/addr.h>
#include <netlink/route/route.h>

#include <stdexcept>

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

optional<string> RouteAddress::label() const
{
	if (const char * str = rtnl_addr_get_label(addr))
		return string(str);
	return std::nullopt;
}

RouteAddress & RouteAddress::label(const string & value)
{
	rtnl_addr_set_label(addr, value.c_str());
	return *this;
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

RouteScope RouteAddress::scope() const
{
	return static_cast<RouteScope>(rtnl_addr_get_scope(addr));
}

RouteAddress & RouteAddress::scope(RouteScope value)
{
	rtnl_addr_set_scope(addr, static_cast<int>(value));
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

RouteLink & RouteLink::name(const string & value)
{
	rtnl_link_set_name(link, value.c_str());
	return *this;
}

unsigned int RouteLink::getFlags() const
{
	return rtnl_link_get_flags(link);
}

RouteLink & RouteLink::setFlags(unsigned int value)
{
	rtnl_link_set_flags(link, value);
	return *this;
}

RouteLink & RouteLink::unsetFlags(unsigned int value)
{
	rtnl_link_unset_flags(link, value);
	return *this;
}

int RouteLink::ifindex() const
{
	return rtnl_link_get_ifindex(link);
}

RouteLink & RouteLink::ifindex(int value)
{
	rtnl_link_set_ifindex(link, value);
	return *this;
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

NextHop::NextHop(rtnl_nexthop * nh):
	nexthop { rtnl_route_nh_clone(nh) }
{
	if (!nexthop)
		throw std::runtime_error("Failed to clone route netlink nexthop");
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

RouteScope Route::scope() const
{
	return static_cast<RouteScope>(rtnl_route_get_scope(route));
}

Route & Route::scope(RouteScope scope)
{
	rtnl_route_set_scope(route, static_cast<uint8_t>(scope));
	return *this;
}

RouteProtocol Route::protocol() const
{
	return static_cast<RouteProtocol>(rtnl_route_get_protocol(route));
}

Route & Route::protocol(RouteProtocol value)
{
	rtnl_route_set_protocol(route, static_cast<uint8_t>(value));
	return *this;
}

uint32_t Route::priority() const
{
	return rtnl_route_get_priority(route);
}

Route & Route::priority(uint32_t value)
{
	rtnl_route_set_priority(route, value);
	return *this;
}

uint8_t Route::family() const
{
	return rtnl_route_get_family(route);
}

Route & Route::family(uint8_t value)
{
	rtnl_route_set_family(route, value);
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

vector<NextHop> Route::nexthops() const
{
	vector<NextHop> result;
	result.reserve(rtnl_route_get_nnexthops(route));

	rtnl_route_foreach_nexthop(route, [](rtnl_nexthop * nh, void * arg) {
		auto presult = static_cast<vector<NextHop> *>(arg);
		presult->emplace_back(nh);
	}, &result);

	return result;
}


RouteSocket::RouteSocket():
	Socket(NETLINK_ROUTE)
{}

bool RouteSocket::add(const RouteAddress & addr)
{
	int err = rtnl_addr_add(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	if (err == -NLE_EXIST)
		return false;
	Exception::throwCode("rtnl_addr_add failed", err);
	return true;
}

bool RouteSocket::del(const RouteAddress & addr)
{
	int err = rtnl_addr_delete(sock, const_cast<rtnl_addr *>(addr.get()), 0);
	if (err == -NLE_NOADDR)
		return false;
	Exception::throwCode("rtnl_addr_delete failed", err);
	return true;
}

bool RouteSocket::add(const RouteLink & link)
{
	int err = rtnl_link_add(sock, const_cast<rtnl_link *>(link.get()), 0);
	Exception::throwCode("rtnl_link_add failed", err);
	return true;
}

bool RouteSocket::del(const RouteLink & link)
{
	int err = rtnl_link_delete(sock, link.get());
	if (err == -NLE_NODEV)
		return false;
	Exception::throwCode("rtnl_link_delete failed", err);
	return true;
}

bool RouteSocket::change(const RouteLink & orig, const RouteLink & changes)
{
	int err = rtnl_link_change(sock, const_cast<rtnl_link *>(orig.get()),
			const_cast<rtnl_link *>(changes.get()), 0);
	Exception::throwCode("rtnl_link_change failed", err);
	return true;
}

bool RouteSocket::add(const Route & route)
{
	int err = rtnl_route_add(sock, const_cast<rtnl_route *>(route.get()), NLM_F_EXCL);
	if (err == -NLE_EXIST)
		return false;
	Exception::throwCode("rtnl_route_add failed", err);
	return true;
}

bool RouteSocket::del(const Route & route)
{
	int err = rtnl_route_delete(sock, const_cast<rtnl_route *>(route.get()), 0);
	if (err == -NLE_OBJ_NOTFOUND)
		return false;
	Exception::throwCode("rtnl_route_delete failed", err);
	return true;
}

RouteCacheManager::RouteCacheManager():
	CacheManager(NETLINK_ROUTE)
{}

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

RouteCache RouteCacheManager::routeCache()
{
	nl_cache * cache;
	int err = nl_cache_mngr_add(mngr, "route/route", nullptr, nullptr, &cache);
	Exception::throwCode("nl_cache_mngr_add failed", err);
	return RouteCache(cache);
}


template<class T>
TypedCache<T>::Iterator::Iterator(nl_object * obj_):
	obj(reinterpret_cast<typename T::RawType *>(obj_))
{}

template<class T>
TypedCache<T>::Iterator::Iterator(nl_object * obj_, nl_object * filter_):
	obj(reinterpret_cast<typename T::RawType *>(obj_)),
	filter(reinterpret_cast<typename T::RawType *>(filter_))
{
	if (not nl_object_match_filter(obj_, filter_))
		++(*this);
}

template<class T>
typename TypedCache<T>::Iterator::reference TypedCache<T>::Iterator::operator*() const
{
	return obj;
}

template<class T>
typename TypedCache<T>::Iterator::pointer TypedCache<T>::Iterator::operator->() const
{
	return &obj;
}

template<class T>
bool TypedCache<T>::Iterator::operator==(const Iterator & other) const
{
	return obj.get() == other.obj.get();
}

template<class T>
bool TypedCache<T>::Iterator::operator!=(const Iterator & other) const
{
	return obj.get() != other.obj.get();
}

template<class T>
typename TypedCache<T>::Iterator & TypedCache<T>::Iterator::operator++()
{
	nl_object * next = reinterpret_cast<nl_object *>(obj.get());
	nl_object * raw_filter = reinterpret_cast<nl_object *>(filter.get());
	do {
		next = nl_cache_get_next(next);
	} while (next && raw_filter && not nl_object_match_filter(next, raw_filter));
	obj = T(reinterpret_cast<typename T::RawType *>(next));
	return *this;
}

template<class T>
typename TypedCache<T>::Iterator TypedCache<T>::Iterator::operator++(int)
{
	nl_object * next = reinterpret_cast<nl_object *>(obj.get());
	nl_object * raw_filter = reinterpret_cast<nl_object *>(filter.get());
	do {
		next = nl_cache_get_next(next);
	} while (next && raw_filter && not nl_object_match_filter(next, raw_filter));

	Iterator tmp = move(*this);
	obj = T(reinterpret_cast<typename T::RawType *>(next));
	filter = T(reinterpret_cast<typename T::RawType *>(raw_filter));
	return tmp;
}

template<class T>
typename TypedCache<T>::Iterator TypedCache<T>::begin() const
{
	return Iterator(nl_cache_get_first(cache));
}

template<class T>
typename TypedCache<T>::Iterator TypedCache<T>::end() const
{
	return Iterator(nullptr);
}


template class TypedCache<RouteAddress>;

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


template class TypedCache<RouteLink>;

RouteLink RouteLinkCache::getByName(const string & name) const
{
	RouteLink link { rtnl_link_get_by_name(cache, name.c_str()) };
	if (link)
		rtnl_link_put(link.get()); // refcount incremented by rtnl_link_get_by_name
	return link;
}


template class TypedCache<Route>;

}
