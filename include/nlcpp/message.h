/** @file
 *  @brief Netlink messages
 */

#pragma once

#include <cstdint>
#if __cplusplus >= 202002L
#include <ranges>
#endif
#include <string>
#include <vector>

struct nl_msg;

namespace nl
{

using std::vector;
using std::string;

class Address;

/// Represents netlink message and can be used to create one.
class Message
{
public:
	Message();
	Message(nl_msg *);
	Message(Message &&);
	Message & operator=(Message &&);
	~Message();

	/// Put unspecific attribute to the message
	Message & put(int attrtype, size_t size, const void * data);
	/// Put unspecific attribute to the message
	Message & put(int attrtype, const vector<uint8_t> & value);
#if __cplusplus >= 202002L
	/// Put unspecific attribute to the message
	Message & put(int attrtype, const std::ranges::subrange<const uint8_t *> & value);
#endif
	/// Put unspecific attribute to the message
	Message & put(int attrtype, const Address & address);

	/// Put signed 8-bit integer attribute to the message
	Message & put_s8(int attrtype, int8_t value);
	/// Put unsigned 8-bit integer attribute to the message
	Message & put_u8(int attrtype, uint8_t value);
	/// Put signed 16-bit integer attribute to the message
	Message & put_s16(int attrtype, int16_t value);
	/// Put unsigned 16-bit integer attribute to the message
	Message & put_u16(int attrtype, uint16_t value);
	/// Put signed 32-bit integer attribute to the message
	Message & put_s32(int attrtype, int32_t value);
	/// Put unsigned 32-bit integer attribute to the message
	Message & put_u32(int attrtype, uint32_t value);
	/// Put signed 64-bit integer attribute to the message
	Message & put_s64(int attrtype, int64_t value);
	/// Put unsigned 64-bit integer attribute to the message
	Message & put_u64(int attrtype, uint64_t value);

	/// Put string attribute to the message
	Message & put(int attrtype, const string & value);

	/// Put nested attribute to the message
	Message & put(int attrtype, const Message & nested);

	/// Get the raw libnl nl_msg pointer
	const nl_msg * get() const { return msg; }
	/// Get the raw libnl nl_msg pointer
	nl_msg * get() { return msg; }

private:
	nl_msg * msg;
};

}
