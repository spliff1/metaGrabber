#ifndef UTILS_H
#define UTILS_H
#include <string>

class Utils {
    public:
        std::string escape(const std::string& input);
        static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *response);
        std::string cleanArtist(const std::string& str, const std::string& station); // station is now const ref
        std::string cleanTitle(const std::string& str, const std::string& station);  // station is now const ref

};

extern Utils utils;
#endif // UTILS_H