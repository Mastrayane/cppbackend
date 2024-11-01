#pragma once

#include <string>

namespace utils {

	std::string UrlDecode(const std::string& url);
	std::string GetMimeType(const std::string& extension);

}  // namespace utils