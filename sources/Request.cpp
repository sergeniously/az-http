#include "Request.h"
#include <regex>

namespace az::http
{

Request::Request(const std::string& method, const Url& url, const std::string& content)
	: method(method)
{
	setUrl(url);

	if (!content.empty()) {
		setContent(content);
	}
}

void Request::setUrl(const Url& url)
{
	setHost(url.host);
	scheme = url.scheme;
	resource = url.path;

	if (url.hasAuth()) {
		setAuthorization("Basic", url.getAuth());
	}
}

void Request::setHost(const std::string& host)
{
	setHeader("Host", host);
}

void Request::setResource(const std::string& resource)
{
	this->resource = resource;
}

void Request::setAuthorization(const std::string& type, const std::string& credentials)
{
	setHeader("Authorization", type + " " + encodeBase64(credentials));
}

void Request::setProxyAuthorization(const std::string& type, const std::string& credentials)
{
	setHeader("Proxy-Authorization", type + " " + encodeBase64(credentials));
}

std::string Request::getHost() const
{
	return getHeader("Host");
}

uint16_t Request::getPort() const
{
	auto host = getHeader("Host");
	if (!host.empty()) {
		auto colon_pos = host.find(':');
		if (colon_pos != std::string::npos) {
			return std::stoul(host.substr(colon_pos + 1));
		}
	}
	return scheme == "https" ? 443 : 80;
}

std::string Request::getHeadline() const
{
	return method + " " + resource + " " + getProtocol();
}

bool Request::setHeadline(const std::string& headline)
{
	std::smatch match;
	std::regex pattern("(\\w+)\\s+(\\S+)\\s+(HTTP/[\\d\\.]+)");
	if (std::regex_match(headline, match, pattern)) {
		method = match[1];
		resource = match[2];
		setProtocol(match[3]);
		return true;
	}
	return false;
}

}