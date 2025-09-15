#include <nlcpp/ctrl.h>

#include <linux/genetlink.h>
#include <netlink/genl/genl.h>

#include <array>

namespace nl {

using std::array;
using std::nullopt;

static const string ctrlFamilyName { "nlctrl" };

GenericSocket::Ctrl GenericSocket::ctrl()
{
	return Ctrl(*this);
}

GenericSocket::Ctrl::Ctrl(GenericSocket & genl_)
	: genl(genl_)
{
}

const string & GenericSocket::Ctrl::familyName() const
{
	return ctrlFamilyName;
}

int GenericSocket::Ctrl::familyId() const
{
	return GENL_ID_CTRL;
}


optional<GenericFamily> GenericSocket::Ctrl::getFamily(const string & name)
{
	GenericNetlinkMessage msg(0, 0, familyId(), 0, 0, CTRL_CMD_GETFAMILY, 0);
	msg.put(CTRL_ATTR_FAMILY_NAME, name);

	optional<GenericFamily> result;

	genl.sendMessageSync(msg, [&](GenericNetlinkMessage reply) {
		result = GenericFamily {};
		parseGenericFamily(*result, reply);
	});

	return result;
}

vector<GenericFamily> GenericSocket::Ctrl::getFamilies()
{
	GenericNetlinkMessage msg(0, 0, familyId(), 0, NLM_F_DUMP, CTRL_CMD_GETFAMILY, 0);

	vector<GenericFamily> result;

	genl.sendMessageSync(msg, [&](GenericNetlinkMessage reply) {
		result.emplace_back();
		parseGenericFamily(result.back(), reply);
	});

	return result;
}

void GenericSocket::Ctrl::parseGenericFamily(GenericFamily & result, const GenericNetlinkMessage & message)
{
	array<nlattr *, CTRL_ATTR_MAX + 1> tb_msg {};
	nla_parse(tb_msg.data(), tb_msg.size() - 1,
			const_cast<nlattr*>(message.getAttrData()),
			message.getAttrLen(), nullptr);

	if (tb_msg[CTRL_ATTR_FAMILY_NAME])
		result.name = nla_get_string(tb_msg[CTRL_ATTR_FAMILY_NAME]);
	if (tb_msg[CTRL_ATTR_FAMILY_ID])
		result.id = nla_get_u32(tb_msg[CTRL_ATTR_FAMILY_ID]);

	struct nlattr *mcgrp;
	int rem_mcgrp;

	if (tb_msg[CTRL_ATTR_MCAST_GROUPS]) {
		nla_for_each_nested(mcgrp, tb_msg[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) {
			array<nlattr *, CTRL_ATTR_MCAST_GRP_MAX + 1> tb_mcgrp;
			nla_parse(tb_mcgrp.data(), tb_mcgrp.size() - 1, static_cast<nlattr*>(nla_data(mcgrp)), nla_len(mcgrp), nullptr);

			if (!tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME] || !tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID])
				continue;

			result.multicastGroups.emplace_back();
			result.multicastGroups.back().name =
				nla_get_string(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME]);
			result.multicastGroups.back().id =
				nla_get_u32(tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]);
		}
	}
}

}
