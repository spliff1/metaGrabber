#include "utils.h"

// Escape strings to prevent SQL injection
std::string Utils::escape(const std::string& input) {

    std::string output;
    output.reserve(input.size() * 2);

    for (char c : input) {
        if (c == '\'') {
            output += "''"; // Replace a single quote with two single quotes for SQL
        } else {
            output += c;
        }
    }

    return output;
}

// For CURL
size_t Utils::writeCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    response->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string Utils::cleanTitle(const std::string& str, const std::string& station) {
    if(station == "Radio Tequila Hiphop") {
        size_t yearPosition = str.rfind(" ");
        if (yearPosition != std::string::npos) {
            if (isdigit(str[yearPosition + 1]) && isdigit(str[yearPosition + 2]) && isdigit(str[yearPosition + 3]) && isdigit(str[yearPosition + 4])) {
                return str.substr(0, yearPosition);
            }
        }
    }

    return str;
}

std::string Utils::cleanArtist(const std::string& str, const std::string& station) {
    if (station == "Radio Cartier") {
        size_t dashPosition = str.find(" - ");
        if (dashPosition != std::string::npos) {
            return str.substr(dashPosition + 3); // Return a copy, not modifying the original
        }
    }
    return str; // Return a copy, not modifying the original
}