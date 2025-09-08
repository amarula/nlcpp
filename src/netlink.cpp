#include <nlcpp/netlink.h>

#include <netlink/cache.h>
#include <netlink/netlink.h>

#include <nlcpp/message.h>

#if __cplusplus >= 202002L
#include <ranges>
#endif
#include <stdexcept>

using std::move;
using std::nullopt;

namespace nl {

Socket::Socket(int protocol)
{
	sock = nl_socket_alloc_cb(nl_cb_alloc(NL_CB_VERBOSE));
	if (!sock)
		throw std::runtime_error("Failed to allocate netlink socket");

	if (nl_connect(sock, protocol) < 0) {
		nl_socket_free(sock);
		throw std::runtime_error("Failed to connect netlink socket");
	}

	callbacks = nl_cb_alloc(NL_CB_DEFAULT);
	if (! callbacks) {
		nl_socket_free(sock);
		throw std::runtime_error("Failed to allocate callback set");
	}

	nl_cb_err(callbacks, NL_CB_CUSTOM, &Socket::errorCallbackWrapper, this);
	nl_cb_set(callbacks, NL_CB_FINISH, NL_CB_CUSTOM, &Socket::finishCallbackWrapper, this);
	nl_cb_set(callbacks, NL_CB_ACK, NL_CB_CUSTOM, &Socket::ackCallbackWrapper, this);
	nl_cb_set(callbacks, NL_CB_VALID, NL_CB_CUSTOM, &Socket::validCallbackWrapper, this);
}

Socket::~Socket()
{
	nl_cb_put(callbacks);
	nl_socket_free(sock);
}

void Socket::sendMessageSync(const Message & message)
{
	nl_send_auto(sock, const_cast<nl_msg *>(message.get()));
	callStatus = 1; // set to 0 when finished or negative error code
	while (callStatus > 0)
		nl_recvmsgs(sock, callbacks);
}

void Socket::sendMessageSync(const Message & message, function<void(Message)> callback)
{
	std::swap(validCallback, callback);
	sendMessageSync(message);
	std::swap(validCallback, callback);
}

void Socket::addMembership(int group)
{
	int err = nl_socket_add_membership(sock, group);
	Exception::throwCode("failed to add membership", err);
}

void Socket::dropMembership(int group)
{
	int err = nl_socket_drop_membership(sock, group);
	Exception::throwCode("failed to add membership", err);
}

int Socket::finishCallbackWrapper(nl_msg * /*msg*/, void * arg)
{
	auto * socket = static_cast<Socket *>(arg);
	socket->callStatus = 0;
	return NL_SKIP;
}

int Socket::ackCallbackWrapper(nl_msg * /*msg*/, void * arg)
{
	auto * socket = static_cast<Socket *>(arg);
	socket->callStatus = 0;
	return NL_STOP;
}

int Socket::validCallbackWrapper(nl_msg * msg, void * arg)
{
	auto * socket = static_cast<Socket *>(arg);
	if (socket->validCallback)
		socket->validCallback(Message(msg));
	return NL_SKIP;
}

int Socket::errorCallbackWrapper(sockaddr_nl * /*saddr*/, nlmsgerr * err, void * arg)
{
	auto * socket = static_cast<Socket *>(arg);
	if (err->error > 0)
		socket->callStatus = -EPROTO;
	else
		socket->callStatus = err->error;

	return NL_STOP;
}

Address::Address(nl_addr * addr_):
	addr { nl_addr_clone(addr_) }
{
	if (!addr)
		throw std::runtime_error("Failed to copy address");
}

Address::Address(const string & str)
{
	int err = nl_addr_parse(str.c_str(), AF_UNSPEC, &addr);
	Exception::throwCode("Failed to parse address \"" + str + "\": ", err);
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

Address::operator string() const
{
	char buf[256];
	nl_addr2str(addr, buf, sizeof(buf));
	return string(buf);
}

#if __cplusplus >= 202002L
std::ranges::subrange<const uint8_t *> Address::binary() const
{
	auto * ptr = static_cast<const uint8_t *>(nl_addr_get_binary_addr(addr));
	return std::ranges::subrange(ptr, ptr + nl_addr_get_len(addr));
}
#endif

unsigned int Address::prefixlen() const
{
	return nl_addr_get_prefixlen(addr);
}

Address & Address::prefixlen(unsigned int prefixlen)
{
	nl_addr_set_prefixlen(addr, static_cast<int>(prefixlen));
	return *this;
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
	Exception::throwCode("nl_cache_mngr_alloc failed", err);
}

CacheManager::~CacheManager()
{
	nl_cache_mngr_free(mngr);
}

int CacheManager::getFd() const
{
	return nl_cache_mngr_get_fd(mngr);
}

int CacheManager::poll(int timeout)
{
	int ret = nl_cache_mngr_poll(mngr, timeout);
	if (ret < 0)
		Exception::throwCode("nl_cache_mngr_poll failed", ret);
	return ret;
}

int CacheManager::dataReady()
{
	int ret = nl_cache_mngr_data_ready(mngr);
	if (ret < 0)
		Exception::throwCode("nl_cache_mngr_data_ready failed", ret);
	return ret;
}

}
