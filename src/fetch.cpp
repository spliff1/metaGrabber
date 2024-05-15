#include "fetch.h"
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <memory>
#include <iostream>

Fetch::Fetch() {
    initializeCurlPool(DEFAULT_POOL_SIZE); // Assuming DEFAULT_POOL_SIZE is defined in fetch.h
}

Fetch::~Fetch() {
    while (!curlPool.empty()) {
        curl_easy_cleanup(curlPool.front());
        curlPool.pop();
    }
}

void Fetch::initializeCurlPool(int size) {
    for (int i = 0; i < size; ++i) {
        CURL *curl = curl_easy_init();
        if (curl) {
            curlPool.push(curl);
        } else {
            std::cerr << "Failed to initialize CURL handle " << i << std::endl;
        }
    }
}

std::string Fetch::page(const std::string& url, const std::string& station) {
    return performCurlRequest(url, station);
}

rapidjson::Document Fetch::json(const std::string& url, const std::string& station) {
    rapidjson::Document json;
    performCurlRequest(url, station, &json);
    return json;
}

std::string Fetch::performCurlRequest(const std::string& url, const std::string& station, rapidjson::Document* json) {
    std::string response;

    std::unique_lock<std::mutex> lock(poolMutex);
    cv.wait(lock, [this]() { return !curlPool.empty(); });

    CURL *curl = curlPool.front();
    curlPool.pop();
    lock.unlock();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils.writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "UTF-8");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable SSL certificate verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Disable host verification
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Set a maximum timeout of 10 seconds
    curl_easy_setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, 1L); // USE DNS GLOBAL CACHE
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 1200L);


    CURLcode res = curl_easy_perform(curl);



    if (res != CURLE_OK) {
        std::cerr << station << " | curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            std::cerr << station << " | HTTP response code: " << http_code << std::endl;
            response.clear(); // Clear response in case of error
            if (json) {
                json->SetObject(); // Set an empty JSON object
            }
        } else if (json) {
            // Parse JSON response if a pointer to a rapidjson::Document is provided
            json->Parse(response.c_str());
            if (json->HasParseError()) {
                std::cerr << station << " | Error parsing JSON: " << GetParseError_En(json->GetParseError()) << std::endl;
            }
        }
    }

    
    lock.lock();
    curl_easy_cleanup(curl);
    curl = curl_easy_init();
    curlPool.push(curl);
    lock.unlock();
    cv.notify_one();
    return response;
}