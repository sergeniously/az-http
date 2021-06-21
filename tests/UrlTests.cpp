#include <boost/test/unit_test.hpp>
#include <Url.h>

BOOST_AUTO_TEST_SUITE(UrlTests)

BOOST_AUTO_TEST_CASE(parse_http_url)
{
	const std::string url_string = "http://www.example.com:123";
	az::http::Url url;
	BOOST_REQUIRE_NO_THROW(url = az::http::Url(url_string));
	BOOST_CHECK_EQUAL(url.scheme, "http");
	BOOST_CHECK_EQUAL(url.host, "www.example.com");
	BOOST_CHECK_EQUAL(url.port, "123");
	BOOST_CHECK_EQUAL(url.path, "/");
}

BOOST_AUTO_TEST_CASE(parse_https_url)
{
	const std::string url_string = "https://user:pass@www.example.net:123/index.php?var=val#id";
	az::http::Url url;
	BOOST_REQUIRE_NO_THROW(url = az::http::Url(url_string));
	BOOST_CHECK_EQUAL(url.scheme, "https");
	BOOST_CHECK_EQUAL(url.getAuth(), "user:pass");
	BOOST_CHECK_EQUAL(url.host, "www.example.net");
	BOOST_CHECK_EQUAL(url.port, "123");
	BOOST_CHECK_EQUAL(url.path, "/index.php?var=val#id");
}

BOOST_AUTO_TEST_CASE(parse_short_url)
{
	const std::string url_string = "//www.example.org";
	az::http::Url url;
	BOOST_REQUIRE_NO_THROW(url = az::http::Url(url_string));
	BOOST_CHECK(url.scheme.empty());
	BOOST_CHECK(!url.hasAuth());
	BOOST_CHECK_EQUAL(url.host, "www.example.org");
	BOOST_CHECK_EQUAL(url.path, "/");
}

BOOST_AUTO_TEST_SUITE_END()
