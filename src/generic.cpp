#include <nlcpp/generic.h>

#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

namespace nl {

using std::move;

GenericSocket::GenericSocket():
	Socket(NETLINK_GENERIC)
{}

int GenericSocket::resolve(const string & name)
{
	int res = genl_ctrl_resolve(sock, name.c_str());
	if (res < 0)
		Exception::throwCode("genl_ctrl_resolve failed", res);
	return res;
}

int GenericSocket::resolveGroup(const string & family, const string & group)
{
	int res = genl_ctrl_resolve_grp(sock, family.c_str(), group.c_str());
	if (res < 0)
		Exception::throwCode("genl_ctrl_resolve_grp failed", res);
	return res;
}

void GenericSocket::sendMessageSync(const GenericNetlinkMessage & message, function<void(GenericNetlinkMessage)> callback)
{
	Socket::sendMessageSync(message, [callback = move(callback)](Message msg) {
		callback(GenericNetlinkMessage(msg.get()));
	});
}

void GenericSocket::onValidMessage(function<void(GenericNetlinkMessage)> callback)
{
	validCallback = [callback = move(callback)](Message msg) {
		callback(GenericNetlinkMessage(msg.get()));
	};
}


GenericNetlinkMessage::GenericNetlinkMessage(uint32_t port, uint32_t seq, int family,
		int hdrlen, int flags, uint8_t cmd, uint8_t version)
{
	genlmsg_put(get(), port, seq, family, hdrlen, flags, cmd, version);
}

GenericNetlinkMessage::GenericNetlinkMessage(nl_msg * msg):
	Message(msg)
{}

unsigned int GenericNetlinkMessage::command() const
{
	return getHeader()->cmd;
}

unsigned int GenericNetlinkMessage::version() const
{
	return getHeader()->version;
}

const genlmsghdr * GenericNetlinkMessage::getHeader() const
{
	return static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(const_cast<nl_msg *>(get()))));
}

const nlattr * GenericNetlinkMessage::getAttrData(int hdrlen) const
{
	return genlmsg_attrdata(getHeader(), hdrlen);
}

int GenericNetlinkMessage::getAttrLen(int hdrlen) const
{
	return genlmsg_attrlen(getHeader(), hdrlen);
}

}
