#include <nlcpp/exception.h>

#include <netlink/errno.h>

namespace nl {

Exception::Exception(const string & prefix, int code):
	error { code },
	msg { prefix + ": " + nl_geterror(code) }
{}

const char * Exception::what() const noexcept
{
	return msg.c_str();
}

int Exception::code() const noexcept
{
	return error;
}

void Exception::throwCode(const string & prefix, int code)
{
	code = abs(code);

	switch (code) {
	case NLE_SUCCESS:
		return;
#if 0
	case NLE_FAILURE:
	case NLE_INTR:
	case NLE_BAD_SOCK:
	case NLE_AGAIN:
	case NLE_NOMEM:
	case NLE_EXIST:
	case NLE_INVAL:
	case NLE_RANGE:
	case NLE_MSGSIZE:
	case NLE_OPNOTSUPP:
	case NLE_AF_NOSUPPORT:
	case NLE_OBJ_NOTFOUND:
#endif
	case NLE_NOATTR:
		throw AttributeNotAvailable(prefix);
	case NLE_MISSING_ATTR:
		throw MissingAttribute(prefix);
#if 0
	case NLE_AF_MISMATCH:
	case NLE_SEQ_MISMATCH:
	case NLE_MSG_OVERFLOW:
	case NLE_MSG_TRUNC:
#endif
	case NLE_NOADDR:
		throw NoAddress(prefix);
#if 0
	case NLE_SRCRT_NOSUPPORT:
	case NLE_MSG_TOOSHORT:
	case NLE_MSGTYPE_NOSUPPORT:
	case NLE_OBJ_MISMATCH:
	case NLE_NOCACHE:
	case NLE_BUSY:
	case NLE_PROTO_MISMATCH:
	case NLE_NOACCESS:
	case NLE_PERM:
	case NLE_PKTLOC_FILE:
	case NLE_PARSE_ERR:
	case NLE_NODEV:
	case NLE_IMMUTABLE:
	case NLE_DUMP_INTR:
	case NLE_ATTRSIZE:
#endif
	}
	throw Exception(prefix, code);
}

AttributeNotAvailable::AttributeNotAvailable(const string & prefix): Exception(prefix, NLE_NOATTR) {}

MissingAttribute::MissingAttribute(const string & prefix): Exception(prefix, NLE_MISSING_ATTR) {}

NoAddress::NoAddress(const string & prefix): Exception(prefix, NLE_NOADDR) {}

}
