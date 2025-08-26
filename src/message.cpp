#include <nlcpp/message.h>

#include <netlink/msg.h>

#include <nlcpp/netlink.h>
#include <nlcpp/exception.h>

#include <stdexcept>

namespace nl
{

Message::Message()
	: msg(nlmsg_alloc())
{
        if (! msg)
		throw std::runtime_error("Failed to allocate netlink message");
}

Message::Message(nl_msg * msg_)
	: msg(msg_)
{
	nlmsg_get(msg);
}

Message::Message(Message && other)
{
	msg = other.msg;
	other.msg = nullptr;
}

Message & Message::operator=(Message && other)
{
	nlmsg_free(msg);
	msg = other.msg;
	other.msg = nullptr;
	return *this;
}

Message::~Message()
{
	nlmsg_free(msg);
}

Message & Message::put(int attrtype, size_t size, const void * data)
{
	int err = nla_put(msg, attrtype, size, data);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put(int attrtype, const vector<uint8_t> & value)
{
	int err = nla_put(msg, attrtype, value.size(), value.data());
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

#if __cplusplus >= 202002L
Message & Message::put(int attrtype, const std::ranges::subrange<const uint8_t *> & value)
{
	int err = nla_put(msg, attrtype, value.size(), value.data());
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}
#endif

Message & Message::put(int attrtype, const Address & address)
{
	int err = nla_put_addr(msg, attrtype, const_cast<nl_addr *>(address.get()));
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_s8(int attrtype, int8_t value)
{
	int err = nla_put_s8(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_u8(int attrtype, uint8_t value)
{
	int err = nla_put_u8(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_s16(int attrtype, int16_t value)
{
	int err = nla_put_s16(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_u16(int attrtype, uint16_t value)
{
	int err = nla_put_u16(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_s32(int attrtype, int32_t value)
{
	int err = nla_put_s32(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_u32(int attrtype, uint32_t value)
{
	int err = nla_put_u32(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_s64(int attrtype, int64_t value)
{
	int err = nla_put_s64(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put_u64(int attrtype, uint64_t value)
{
	int err = nla_put_u64(msg, attrtype, value);
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put(int attrtype, const string & value)
{
	int err = nla_put_string(msg, attrtype, value.c_str());
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

Message & Message::put(int attrtype, const Message & nested)
{
	int err = nla_put_nested(msg, attrtype, nested.get());
	Exception::throwCode(__FUNCTION__, err);
	return *this;
}

}
