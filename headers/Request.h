#pragma once
#include "Message.h"
#include "Url.h"

namespace az::http
{

class Request : public Message
{
public:
	Request(const std::string& method = {}, const Url& url = {}, const std::string& content = {});
	void setUrl(const Url&);
	void setHost(const std::string&);
	void setResource(const std::string&);
	void setAuthorization(const std::string& type, const std::string& credentials);
	void setProxyAuthorization(const std::string& type, const std::string& credentials);
	const std::string& getMethod() const { return method; }
	const std::string& getScheme() const { return scheme; }
	const std::string& getResource() const { return resource; }
	std::string getHost() const;
	uint16_t getPort() const;
private:
	std::string getHeadline() const override;
	bool setHeadline(const std::string&) override;
private:
	std::string method;
	std::string scheme;
	std::string resource;
};

}