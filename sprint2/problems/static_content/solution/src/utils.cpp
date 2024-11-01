#include "utils.h"
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace utils {

    std::string UrlDecode(const std::string& url) {
        std::ostringstream decoded;
        for (size_t i = 0; i < url.size(); ++i) {
            if (url[i] == '%') {
                if (i + 2 < url.size()) {
                    std::istringstream iss(url.substr(i + 1, 2));
                    int hex_value;
                    iss >> std::hex >> hex_value;
                    decoded << static_cast<char>(hex_value);
                    i += 2;
                }
            }
            else if (url[i] == '+') {
                decoded << ' ';
            }
            else {
                decoded << url[i];
            }
        }
        return decoded.str();
    }

    std::string GetMimeType(const std::string& extension) {
        static const std::unordered_map<std::string, std::string> mimeTypes = {
             {".htm", "text/html"}, {".html", "text/html"},
             {".css", "text/css"}, {".txt", "text/plain"},
             {".js", "text/javascript"}, {".json", "application/json"},
             {".xml", "application/xml"}, {".png", "image/png"},
             {".jpg", "image/jpeg"}, {".jpeg", "image/jpeg"}, {".jpe", "image/jpeg"},
             {".gif", "image/gif"}, {".bmp", "image/bmp"},
             {".ico", "image/vnd.microsoft.icon"}, {".tiff", "image/tiff"},
             {".tif", "image/tiff"}, {".svg", "image/svg+xml"},
             {".svgz", "image/svg+xml"}, {".mp3", "audio/mpeg"}
        };

        // Приводим расширение к нижнему регистру
        std::string lower_extension = extension;
        std::transform(lower_extension.begin(), lower_extension.end(), lower_extension.begin(), ::tolower);

        auto it = mimeTypes.find(lower_extension);
        if (it != mimeTypes.end()) {
            return it->second;
        }
        return "application/octet-stream";
    }

}  // namespace utils