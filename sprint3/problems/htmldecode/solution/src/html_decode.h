#ifndef __HTML_DECODE_H__
#define __HTML_DECODE_H__

#include <string>
#include <string_view>

std::string HtmlDecode(std::string_view str);

#endif // __HTML_DECODE_H__