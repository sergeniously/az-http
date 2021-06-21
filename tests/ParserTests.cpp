#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <Parser.h>
#include <Request.h>
#include <Response.h>

using namespace az;

BOOST_AUTO_TEST_SUITE(ParserTests)

BOOST_AUTO_TEST_CASE(parse_request)
{
	http::Request request("POST", http::Url("http://www.example.org/resource"), "Something");
	request.setHeader("Content-Type", "text");

	BOOST_CHECK_EQUAL(request.getMethod(), "POST");
	BOOST_CHECK_EQUAL(request.getResource(), "/resource");
	BOOST_CHECK_EQUAL(request.getHost(), "www.example.org");
	BOOST_CHECK_EQUAL(request.getHeader("Content-Type"), "text");
	BOOST_CHECK_EQUAL(request.getContentLength(), 9);
	BOOST_CHECK_EQUAL(request.getContent(), "Something");

	auto data = request.stringify();
	http::Request parsed_request;
	BOOST_REQUIRE(http::Parser(parsed_request).complete(data.c_str(), data.size()));

	BOOST_CHECK_EQUAL(request.getMethod(), parsed_request.getMethod());
	BOOST_CHECK_EQUAL(request.getResource(), parsed_request.getResource());
	BOOST_CHECK_EQUAL(request.getHost(), parsed_request.getHost());
	BOOST_CHECK_EQUAL(request.getHeader("Content-Type"), parsed_request.getHeader("Content-Type"));
	BOOST_CHECK_EQUAL(request.getContentLength(), parsed_request.getContentLength());
	BOOST_CHECK_EQUAL(request.getContent(), parsed_request.getContent());
}

BOOST_AUTO_TEST_CASE(parse_response)
{
	http::Response response(200, "Some");
	response.setProtocol("HTTP/2");
	response.setHeader("Content-Type", "text");
	response.addContent("thing");

	BOOST_CHECK_EQUAL(response.getStatusCode(), 200);
	BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/2");
	BOOST_CHECK_EQUAL(response.getContentLength(), 9);
	BOOST_CHECK_EQUAL(response.getContent(), "Something");
	BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), "text");

	auto data = response.stringify();
	http::Response parsed_response;
	BOOST_REQUIRE(http::Parser(parsed_response).complete(data.c_str(), data.size()));

	BOOST_CHECK_EQUAL(response.getStatusCode(), parsed_response.getStatusCode());
	BOOST_CHECK_EQUAL(response.getProtocol(), parsed_response.getProtocol());
	BOOST_CHECK_EQUAL(response.getContentLength(), parsed_response.getContentLength());
	BOOST_CHECK_EQUAL(response.getContent(), parsed_response.getContent());
	BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), parsed_response.getHeader("Content-Type"));
}

BOOST_AUTO_TEST_CASE(parse_request_without_content)
{
	const std::string data = "GET /index.php HTTP/1.1\r\n"
		"Host: www.example.org\r\n"
		"\r\n";

	http::Request request;
	BOOST_CHECK(http::Parser(request).complete(data.c_str(), data.size()));
	BOOST_CHECK_EQUAL(request.getMethod(), "GET");
	BOOST_CHECK_EQUAL(request.getResource(), "/index.php");
	BOOST_CHECK_EQUAL(request.getProtocol(), "HTTP/1.1");
	BOOST_CHECK_EQUAL(request.getHost(), "www.example.org");
	BOOST_CHECK_EQUAL(request.getContentLength(), 0);
	BOOST_CHECK(request.getContent().empty());
}

BOOST_AUTO_TEST_CASE(parse_request_with_content)
{
	const std::string data = "POST   /update.py    HTTP/2\r\n"
		"Host:           www.example.org\r\n"
		"Content-Length: 15\r\n"
		"\r\n"
		"Awesome content";

	http::Request request;
	BOOST_CHECK(http::Parser(request).complete(data.c_str(), data.size()));
	BOOST_CHECK_EQUAL(request.getMethod(), "POST");
	BOOST_CHECK_EQUAL(request.getResource(), "/update.py");
	BOOST_CHECK_EQUAL(request.getProtocol(), "HTTP/2");
	BOOST_CHECK_EQUAL(request.getHost(), "www.example.org");
	BOOST_CHECK_EQUAL(request.getContentLength(), 15);
	BOOST_CHECK_EQUAL(request.getContent(), "Awesome content");
}

BOOST_AUTO_TEST_CASE(parse_response_without_content)
{
	const std::string data = "HTTP/1.0 301 Moved Permanently\r\n"
		"Location: http://www.example.org/index.asp\r\n"
		"\r\n";

	http::Response response;
	BOOST_CHECK(http::Parser(response).complete(data.c_str(), data.size()));
	BOOST_CHECK_EQUAL(response.getStatusCode(), 301);
	BOOST_CHECK_EQUAL(response.getStatusMessage(), "Moved Permanently");
	BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/1.0");
	BOOST_CHECK_EQUAL(response.getHeader("Location"), "http://www.example.org/index.asp");
	BOOST_CHECK_EQUAL(response.getContentLength(), 0);
	BOOST_CHECK(response.getContent().empty());
}

BOOST_AUTO_TEST_CASE(parse_response_with_content)
{
	const std::string data = "HTTP/2   404  Not Found\r\n"
		"Date:           Mon, 23 May 2005 22:38:34 GMT\r\n"
		"Content-Type:   text/html; charset=UTF-8\r\n"
		"Content-Length: 30\r\n"
		"Last-Modified:  Wed, 08 Jan 2003 23:11:55 GMT\r\n"
		"Server:         Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\n"
		"ETag:           \"3f80f-1b6-3e1cb03b\"\r\n"
		"Accept-Ranges:  bytes\r\n"
		"Connection:     close\r\n"
		"\r\n"
		"<html>There is nothing!</html>";

	http::Response response;
	BOOST_CHECK(http::Parser(response).complete(data.c_str(), data.size()));
	BOOST_CHECK_EQUAL(response.getStatusCode(), 404);
	BOOST_CHECK_EQUAL(response.getStatusMessage(), "Not Found");
	BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/2");
	BOOST_CHECK_EQUAL(response.getHeader("Server"), "Apache/1.3.3.7 (Unix) (Red-Hat/Linux)");
	BOOST_CHECK_EQUAL(response.getHeader("Date"), "Mon, 23 May 2005 22:38:34 GMT");
	BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), "text/html; charset=UTF-8");
	BOOST_CHECK_EQUAL(response.getHeader("Last-Modified"), "Wed, 08 Jan 2003 23:11:55 GMT");
	BOOST_CHECK_EQUAL(response.getHeader("ETag"), "\"3f80f-1b6-3e1cb03b\"");
	BOOST_CHECK_EQUAL(response.getHeader("Accept-Ranges"), "bytes");
	BOOST_CHECK_EQUAL(response.getHeader("Connection"), "close");
	BOOST_CHECK_EQUAL(response.getContentLength(), 30);
	BOOST_CHECK_EQUAL(response.getContent(), "<html>There is nothing!</html>");
}

BOOST_AUTO_TEST_CASE(parse_response_with_content_partially)
{
	const std::string data = "HTTP/2   404  Not Found\r\n"
		"Date:           Mon, 23 May 2005 22:38:34 GMT\r\n"
		"Content-Type:   text/html; charset=UTF-8\r\n"
		"Content-Length: 30\r\n"
		"Last-Modified:  Wed, 08 Jan 2003 23:11:55 GMT\r\n"
		"Server:         Apache/1.3.3.7 (Unix) (Red-Hat/Linux)\r\n"
		"ETag:           \"3f80f-1b6-3e1cb03b\"\r\n"
		"Accept-Ranges:  bytes\r\n"
		"Connection:     close\r\n"
		"\r\n"
		"<html>There is nothing!</html>";

	for (uint32_t div = 1; div < data.size() - 1; div++) {
		auto part1 = data.substr(0, div);
		auto part2 = data.substr(div);

		http::Response response;
		http::Parser parser(response);
		BOOST_CHECK(!parser.complete(part1.c_str(), part1.size()));
		BOOST_CHECK(parser.complete(part2.c_str(), part2.size()));

		BOOST_CHECK_EQUAL(response.getStatusCode(), 404);
		BOOST_CHECK_EQUAL(response.getStatusMessage(), "Not Found");
		BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/2");
		BOOST_CHECK_EQUAL(response.getHeader("Server"), "Apache/1.3.3.7 (Unix) (Red-Hat/Linux)");
		BOOST_CHECK_EQUAL(response.getHeader("Date"), "Mon, 23 May 2005 22:38:34 GMT");
		BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), "text/html; charset=UTF-8");
		BOOST_CHECK_EQUAL(response.getHeader("Last-Modified"), "Wed, 08 Jan 2003 23:11:55 GMT");
		BOOST_CHECK_EQUAL(response.getHeader("ETag"), "\"3f80f-1b6-3e1cb03b\"");
		BOOST_CHECK_EQUAL(response.getHeader("Accept-Ranges"), "bytes");
		BOOST_CHECK_EQUAL(response.getHeader("Connection"), "close");
		BOOST_CHECK_EQUAL(response.getContentLength(), 30);
		BOOST_CHECK_EQUAL(response.getContent(), "<html>There is nothing!</html>");
	}
}

BOOST_AUTO_TEST_CASE(parse_chunked_response)
{
	const std::string data = "HTTP/1.1 200 OK\r\n"
		"Server: nginx/1.0.4\r\n"
		"Date: Thu, 06 Oct 2011 16:14:01 GMT\r\n"
		"Content-Type: text/html\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"24\r\n"
		"This is the data in the first chunk \r\n"
		"1d\r\n"
		"and this is the second one!\r\n\r\n"
		"A\r\n"
		"That's it!\r\n"
		"0\r\n\r\n";

	http::Response response;
	BOOST_CHECK(http::Parser(response).complete(data.c_str(), data.size()));
	BOOST_CHECK_EQUAL(response.getStatusCode(), 200);
	BOOST_CHECK_EQUAL(response.getStatusMessage(), "OK");
	BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/1.1");
	BOOST_CHECK_EQUAL(response.getTransferEncoding(), "chunked");
	BOOST_CHECK_EQUAL(response.getHeader("Server"), "nginx/1.0.4");
	BOOST_CHECK_EQUAL(response.getHeader("Date"), "Thu, 06 Oct 2011 16:14:01 GMT");
	BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), "text/html");
	BOOST_CHECK_EQUAL(response.getHeader("Connection"), "keep-alive");
	BOOST_CHECK_EQUAL(response.getContentLength(), 75);
	BOOST_CHECK_EQUAL(response.getContent(), "This is the data in the first chunk and this is the second one!\r\nThat's it!");
}

BOOST_AUTO_TEST_CASE(parse_chunked_response_partially)
{
	const std::string data = "HTTP/1.1 200 OK\r\n"
		"Server: nginx/1.0.4\r\n"
		"Date: Thu, 06 Oct 2011 16:14:01 GMT\r\n"
		"Content-Type: text/html\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"24\r\n"
		"This is the data in the first chunk \r\n"
		"1D\r\n"
		"and this is the second one!\r\n\r\n"
		"A\r\n"
		"That's it!\r\n"
		"0\r\n\r\n";

	for (uint32_t div = 1; div < data.size() - 1; div++) {
		auto part1 = data.substr(0, div);
		auto part2 = data.substr(div);

		http::Response response;
		http::Parser parser(response);

		BOOST_CHECK(!parser.complete(part1.c_str(), part1.size()));
		BOOST_CHECK(parser.complete(part2.c_str(), part2.size()));

		BOOST_CHECK_EQUAL(response.getStatusCode(), 200);
		BOOST_CHECK_EQUAL(response.getStatusMessage(), "OK");
		BOOST_CHECK_EQUAL(response.getProtocol(), "HTTP/1.1");
		BOOST_CHECK_EQUAL(response.getTransferEncoding(), "chunked");
		BOOST_CHECK_EQUAL(response.getHeader("Server"), "nginx/1.0.4");
		BOOST_CHECK_EQUAL(response.getHeader("Date"), "Thu, 06 Oct 2011 16:14:01 GMT");
		BOOST_CHECK_EQUAL(response.getHeader("Content-Type"), "text/html");
		BOOST_CHECK_EQUAL(response.getHeader("Connection"), "keep-alive");
		BOOST_CHECK_EQUAL(response.getContentLength(), 75);
		BOOST_CHECK_EQUAL(response.getContent(), "This is the data in the first chunk and this is the second one!\r\nThat's it!");
	}
}

BOOST_AUTO_TEST_SUITE_END()
