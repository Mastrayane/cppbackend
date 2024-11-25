#include "url_decode.h"

std::string UrlDecode(const std::string& url) {
  std::string str;
  str.reserve(url.size());
  for (size_t i = 0; i < url.size(); ++i) {
    if (url[i] == '%' && i + 2 < url.size()) {
      const std::string r = url.substr(i + 1, 2);
      str.push_back(std::stoul(r, nullptr, 16));
      i += 2;
      continue;
    } else if (url[i] == '+') {
      str.push_back(' ');
    } else {
      str.push_back(url[i]);
    }
  }
  return str;
}
