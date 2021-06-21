#pragma once
#include <map>
#include <string>
#include <vector>

namespace az::http
{

class Message 
{
public:
	static const char* LINE_BREAK;

	void setProtocol(const std::string&);
	void setHeader(const std::string& name, const std::string& value);
	void setContentType(const std::string&);
	void setContentLength(uint32_t);
	void setContent(const std::string&);
	void addContent(const std::string&);

	const std::string& getProtocol() const { return protocol; }
	const std::string& getContent() const { return content; }
	const std::map<std::string, std::string>& getHeaders() const { return headers; }
	std::string getHeader(const std::string&) const;
	bool hasHeader(const std::string&) const;
	uint32_t getContentLength() const;
	std::string getTransferEncoding() const;

	std::string stringify() const;
	virtual std::string getHeadline() const = 0;
	virtual bool setHeadline(const std::string&) = 0;

	static std::string encodeBase64(const std::string& content);

private:
	std::string protocol = "HTTP/1.1";
	std::map<std::string, std::string> headers;
	std::string content;
};

}
