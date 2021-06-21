#include "Response.h"
#include <regex>

namespace az::http
{

Response::Response(uint32_t status_code, const std::string& content)
	: status_code(status_code)
{
	if (!content.empty()) {
		setContent(content);
	}
}

bool Response::setHeadline(const std::string& headline)
{
	std::smatch match;
	std::regex pattern("(HTTP/[\\d\\.]+)\\s+(\\d+)\\s*(.*)");
	if (std::regex_match(headline, match, pattern)) {
		status_code = std::stoul(match[2]);
		status_message = match[3];
		setProtocol(match[1]);
		return true;
	}
	return false;
}

std::string Response::getHeadline() const
{
	return getProtocol() + " " + std::to_string(status_code) + " " + status_message;
}

bool Response::isOk() const
{
	return status_code == 200;
}

}