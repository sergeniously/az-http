#pragma once
#include "Message.h"
#include <vector>

namespace az::http
{

class Response : public Message
{
public:
	Response(uint32_t status_code = 0, const std::string& content = {});
	uint32_t getStatusCode() const { return status_code; }
	const std::string& getStatusMessage() const { return status_message; }
	bool isOk() const;

	std::string getHeadline() const override;
	bool setHeadline(const std::string&) override;

private:
	uint32_t status_code = 0;
	std::string status_message;
};

}