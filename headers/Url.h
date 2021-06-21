#pragma once
#include <string>

namespace az::http
{

struct Url
{
	Url() = default;
	Url(const std::string&);
	bool hasAuth() const;
	std::string getAuth() const;

	std::string scheme;
	std::string username;
	std::string password;
	std::string host;
	std::string port;
	std::string path;
};

}