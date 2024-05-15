#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <libconfig.h++>

class Config {
private:
    static const std::string configFile;
    static libconfig::Config cfg;
    static bool initialized;

public:
    Config();

    static bool initialize();

    static std::string getValue(const std::string& stationId, const std::string& name);

    static bool database(std::string& host, std::string& user, std::string& pass, std::string& db);
};
extern Config config;

#endif // CONFIG_H
