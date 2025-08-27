/** @file
 *  @brief Generic netlink support
 */

#pragma once

#include <nlcpp/message.h>
#include <nlcpp/netlink.h>

struct genlmsghdr;
struct nlattr;

namespace nl {

class GenericNetlinkMessage;

/// Socket opened for the generic netlink protocol
class GenericSocket : protected Socket
{
public:
	GenericSocket();

	int resolve(const string & name);
	int resolveGroup(const string & family, const string & group);

	/// Send given message and wait for ACK or FINISH, calling given
	/// callback for all valid replies.
	void sendMessageSync(const GenericNetlinkMessage & message, function<void(GenericNetlinkMessage)> callback);

	class Nl80211;
	/// Interface for nl80211 netlink family. Returned object can be used
	/// only as long as this GenericSocket object exists.
	Nl80211 nl80211();
};

class GenericNetlinkMessage : public Message
{
public:
	GenericNetlinkMessage(uint32_t port, uint32_t seq, int family,
			int hdrlen, int flags, uint8_t cmd, uint8_t version);
	explicit GenericNetlinkMessage(nl_msg *);

	/// Get command number
	unsigned int command() const;

	/// Get version number
	unsigned int version() const;

	/// Get the raw generic netlink message header pointer
	const genlmsghdr * getHeader() const;

	/// Get the raw libnl pointer to attribute data
	const nlattr * getAttrData(int hdrlen = 0) const;

	/// Get length of message attributes
	int getAttrLen(int hdrlen = 0) const;
};

}
