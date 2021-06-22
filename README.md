# Features

- Supports all HTTP methods.
- Supports chunked transfer encoding.
- Can parse either responses or requests.

# Examples

Assume we have created a connection via socket and have taken a file descriptor **fd**. Let's see how we could perform a simple HTTP transaction:

```c++
// create an HTTP request
az::http::Request request("GET", "http://www.example.net/get/");
request.setHeader("User-Agent", "Google Chrome");

// transmit the request
auto request_text = request.stringify();
send(fd, request_text.data(), request_text.size(), 0);

// prepare an HTTP response
az::http::Response response;
az::http::Parser parser(response);

// receive the response
char data[512] = {};
size_t size = 0;
do {
    size = recv(fd, data, sizeof(data), 0);
} while (!parser.complete(data, size));

// analyze the response
if (response.isOk()) {
    std::cout << response.getContent();
}
```

Now let's see how to create a POST request with some content:
```c++
az::http::Request request("POST", "https://www.example.net/post/");
request.setContentType("application/text");
request.setContent("text");
```