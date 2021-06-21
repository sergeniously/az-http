#include "Url.h"

namespace az::http
{

Url::Url(const std::string& url)
{
	enum class State {
		Begin, Scheme, SchemeEnd, HostOrPath, Host, Port
	} state = State::Begin;
	auto offset = url.begin();
	for (auto current = url.begin(); current != url.end(); current++) {
		switch (state) {
			case State::Begin:
				if (*current == '/') {
					state = State::HostOrPath;
				}
				else if (*current == ':') {
					state = State::SchemeEnd;
				}
				else if (isalpha(*current)) {
					state = State::Scheme;
				}
				else {
					// throw error
				}
				break;
			case State::Scheme:
				if (*current == ':') {
					state = State::SchemeEnd;
				}
				break;
			case State::SchemeEnd:
				scheme = std::string(offset, current - 1);
				if (*current == '/') {
					state = State::HostOrPath;
				}
				break;
			case State::HostOrPath:
				if (*current == '/') {
					state = State::Host;
					offset = current;
				}
				else {
					path = std::string(current, url.end());
					return;
				}
				break;
			case State::Host:
				if (*current == ':') {
					host = std::string(offset + 1, current);
					state = State::Port;
					offset = current;
				}
				else if (*current == '/') {
					host = std::string(offset + 1, current);
					path = std::string(current, url.end());
					return;
				}
				else if (*current == '@') {
					username = std::string(offset + 1, current);
					state = State::Host;
					offset = current;
				}
				break;
			case State::Port:
				if (*current == '/') {
					port = std::string(offset + 1, current);
					path = std::string(current, url.end());
					return;
				}
				else if (*current == '@') {
					username = std::move(host);
					password = std::string(offset + 1, current);
					state = State::Host;
					offset = current;
				}
				break;
		}
	}
	switch (state) {
		case State::Host:
			host = std::string(offset + 1, url.end());
			break;
		case State::Port:
			port = std::string(offset + 1, url.end());
			break;
		default:
			// throw error
			break;
	}
	path = '/';
}

bool Url::hasAuth() const
{
	return !username.empty();
}

std::string Url::getAuth() const
{
	std::string auth = username;
	if (!password.empty()) {
		auth.append(":").append(password);
	}
	return auth;
}

}