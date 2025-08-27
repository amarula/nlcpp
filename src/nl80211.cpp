#include <nlcpp/nl80211.h>

#include <linux/nl80211.h>
#include <netlink/genl/genl.h>

#include <nlcpp/message.h>

#include <array>

namespace nl {

using namespace nl80211;

using std::array;

GenericSocket::Nl80211 GenericSocket::nl80211()
{
	return Nl80211(*this);
}

GenericSocket::Nl80211::Nl80211(GenericSocket & genl_)
	: genl(genl_)
	, family(genl_.resolve(NL80211_GENL_NAME))
{
}

optional<InterfaceConfiguration> GenericSocket::Nl80211::getInterface(uint32_t ifindex)
{
	GenericNetlinkMessage msg(0, 0, family, 0, 0, NL80211_CMD_GET_INTERFACE, 0);
	msg.put_u32(NL80211_ATTR_IFINDEX, ifindex);

	optional<InterfaceConfiguration> result;

	genl.sendMessageSync(msg, [&](GenericNetlinkMessage reply) {
		result = InterfaceConfiguration {};
		parseInterfaceConfiguration(*result, reply);
	});

	return result;
}

vector<InterfaceConfiguration> GenericSocket::Nl80211::getInterfaces()
{
	GenericNetlinkMessage msg(0, 0, family, 0, NLM_F_DUMP, NL80211_CMD_GET_INTERFACE, 0);

	vector<InterfaceConfiguration> result;

	genl.sendMessageSync(msg, [&](GenericNetlinkMessage reply) {
		result.emplace_back();
		parseInterfaceConfiguration(result.back(), reply);
	});

	return result;
}

void GenericSocket::Nl80211::parseInterfaceConfiguration(InterfaceConfiguration & interface, const GenericNetlinkMessage & message) const
{
	array<nlattr *, NL80211_ATTR_MAX + 1> tb_msg {};
	nla_parse(tb_msg.data(), tb_msg.size() - 1,
			const_cast<nlattr*>(message.getAttrData()),
			message.getAttrLen(), nullptr);

	if (tb_msg[NL80211_ATTR_IFINDEX])
		interface.ifindex = nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]);

	if (tb_msg[NL80211_ATTR_WIPHY])
		interface.wiphy = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);

	if (tb_msg[NL80211_ATTR_IFTYPE])
		interface.iftype = static_cast<InterfaceType>(nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]));

	if (tb_msg[NL80211_ATTR_MAC])
		memcpy(interface.mac.data(), nla_data(tb_msg[NL80211_ATTR_MAC]), ETH_ALEN);

	if (tb_msg[NL80211_ATTR_4ADDR])
		interface.use4addr = nla_get_u8(tb_msg[NL80211_ATTR_4ADDR]);
}

}
