#include "src/config.h"
#include "src/database.h"
#include "src/utils.h"
#include "src/fetch.h"
#include "src/meta-text.h"
#include "src/meta-json.h"
#include <thread>
#include <chrono>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>

metaText metaText;
metaJson metaJson;

void init() {
    
    std::string host, user, pass, db;


    if (!config.database(host, user, pass, db)) {
        std::cerr << "Failed to read database configuration from config.ini." << std::endl;
        exit(1);
    }

    if (!database.connect(host, user, pass, db)) {
        std::cerr << "Failed to connect to database." << std::endl;
        exit(1);
    }
}



int main() {
    init();
    unsigned long long iterationNum = 0;
    std::srand(std::time(nullptr)); // Seed for random number generation

    while (true) {
        ++iterationNum;
        std::vector<std::vector<std::string>> rows = database.multiGet("SELECT id, name, meta_type, meta_url FROM stations WHERE meta_type IS NOT NULL");

        for (const auto& row : rows) {
            // Create a thread for each station
            std::thread t([=]() {
                // Random sleep between 1 and 10 seconds, so we won't kill the poor CPU
                int sleepDuration = std::rand() % 10 + 1;
                std::this_thread::sleep_for(std::chrono::seconds(sleepDuration));

                if(row[2] == "json") {
                    metaJson.process(row[0], row[1], row[2], row[3]);
                } 

                if (row[2] == "text") {
                    metaText.process(row[0], row[1], row[2], row[3]);
                }
            });
            t.detach();
        }

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::cout << "\033[32m" << std::put_time(std::localtime(&in_time_t), "[%d.%m.%Y][%H:%M]: ") << "\033[0m" << "Iteration #" << iterationNum << " ran. Waiting 25 seconds." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(25));
    }

    return 0;
}
