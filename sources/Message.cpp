#include "Message.h"
#include <algorithm>

namespace az::http
{

const char* Message::LINE_BREAK = "\r\n";

void Message::setProtocol(const std::string& protocol)
{
	this->protocol = protocol;
}

void Message::setHeader(const std::string& name, const std::string& value)
{
	headers[name] = value;
}

std::string Message::getHeader(const std::string& name) const
{
	if (headers.contains(name)) {
		return headers.at(name);
	}
	return {};
}

bool Message::hasHeader(const std::string& name) const
{
	return headers.contains(name);
}

void Message::setContentType(const std::string& type)
{
	setHeader("Content-Type", type);
}

void Message::setContentLength(uint32_t length)
{
	setHeader("Content-Length", std::to_string(length));
}

uint32_t Message::getContentLength() const
{
	try {
		return std::stoul(getHeader("Content-Length"));
	}
	catch (const std::exception&) {
		return content.length();
	}
}

void Message::setContent(const std::string& content)
{
	this->content = content;
	setContentLength(this->content.length());
}

void Message::addContent(const std::string& content)
{
	this->content += content;
	setContentLength(this->content.length());
}

std::string Message::getTransferEncoding() const
{
	return getHeader("Transfer-Encoding");
}

std::string Message::stringify() const
{
	std::string message_data = getHeadline() + LINE_BREAK;
	for (const auto& header : headers) {
		message_data += header.first + ": " + header.second + LINE_BREAK;
	}
	message_data += LINE_BREAK;
	message_data += content;
	return message_data;
}

std::string Message::encodeBase64(const std::string& content)
{
	// todo
	return {};
}

}