#pragma once
#include "Message.h"

namespace az::http
{

class Parser
{
public:
	Parser(Message&);
	bool complete(const char* data, uint32_t size);

private:
	bool pickLine(std::string& line, uint32_t length = 0);
	enum class State {
		HEADLINE,
		HEADERS,
		CONTENT,
		CHUNKED
	};

	Message& message;
	std::string content;
	uint32_t expected_content_length = 0;
	State state = State::HEADLINE;
};

}