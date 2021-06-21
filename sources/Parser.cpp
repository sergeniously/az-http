#include "Parser.h"
#include <cstring>

namespace az::http
{

Parser::Parser(Message& message)
	: message(message)
{
}

bool Parser::pickLine(std::string& line, uint32_t length /* = 0 */)
{
	static const auto line_break_length = strlen(Message::LINE_BREAK);
	auto line_break_pos = content.npos;

	if (length == 0) {
		line_break_pos = content.find(Message::LINE_BREAK);
	}
	else if (content.length() >= length + line_break_length) {
		if (memcmp(&content[length], Message::LINE_BREAK, line_break_length) == 0) {
			line_break_pos = length;
		}
	}
	if (line_break_pos != content.npos) {
		line = content.substr(0, line_break_pos);
		content.erase(0, line_break_pos + line_break_length);
		return true;
	}
	return false;
}

bool Parser::complete(const char* data, uint32_t size)
{
	content.append(data, size);

	if (state == State::HEADLINE) {
		std::string headline;
		if (pickLine(headline) && message.setHeadline(headline)) {
			state = State::HEADERS;
		}
	}
	if (state == State::HEADERS) {
		std::string header;
		while (pickLine(header)) {
			if (header.empty()) {
				if (message.getTransferEncoding() == "chunked") {
					state = State::CHUNKED;
				}
				else {
					expected_content_length = message.getContentLength();
					state = State::CONTENT;
				}
				break;
			}
			auto colon_pos = header.find(':');
			if (colon_pos != header.npos) {
				auto value_pos = colon_pos;
				do value_pos++; while (isspace(header[value_pos]));
				message.setHeader(header.substr(0, colon_pos), header.substr(value_pos));
			}
		}
	}
	if (state == State::CONTENT) {
		if (content.length() >= expected_content_length) {
			message.setContent(content);
			return true;
		}
	}
	else if (state == State::CHUNKED) {
		/* Each chunk starts with the number of octets of the data it embeds
		 * expressed as a hexadecimal number in ASCII followed by optional parameters (chunk extension)
		 * and a terminating CRLF sequence, followed by the chunk data. The chunk is terminated by CRLF.
		 * The terminating chunk is a regular chunk, with the exception that its length is zero.
		 * */
		std::string chunk_line;
		while (pickLine(chunk_line, expected_content_length)) {
			if (expected_content_length == 0) {
				if (chunk_line.empty()) {
					return true; // empty chunk means done
				}
				expected_content_length = std::strtoul(chunk_line.c_str(), nullptr, 16);
			}
			else {
				message.addContent(chunk_line);
				expected_content_length = 0;
			}
		}
	}
	return false; // need more data
}

}