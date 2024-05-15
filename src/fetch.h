#include <string>
#include <iostream>
#include <ostream>
#include <mutex>
#include <curl.h>
#include <queue>
#include <condition_variable>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/pointer.h>
#include "utils.h"

class Fetch {
    public:
        Fetch(); // Constructor
        ~Fetch(); // Destructor
        void initializeCurlPool(int size); // Declaration
        rapidjson::Document json(const std::string& url, const std::string& station);
        std::string page(const std::string& url, const std::string& station);
        std::string performCurlRequest(const std::string& url, const std::string& station, rapidjson::Document* json = nullptr); // Declaration

    private:
        std::queue<CURL*> curlPool;
        std::mutex poolMutex;
        std::condition_variable cv;
        static const int DEFAULT_POOL_SIZE = 10;
};