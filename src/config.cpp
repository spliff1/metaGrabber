#include "config.h"

const std::string Config::configFile = "config.ini";
libconfig::Config Config::cfg;
bool Config::initialized = false;

Config::Config() {
    if (!initialized) {
        if (!initialize()) {
            std::cerr << "Error initializing configuration." << std::endl;
        }
    }
}

bool Config::initialize() {
    try {
        cfg.readFile(configFile.c_str());
        initialized = true;
        return true;
    } catch(const libconfig::FileIOException &fioex) {
        std::cerr << "I/O error while reading file." << std::endl;
    } catch(const libconfig::ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
    }
    return false;
}

std::string Config::getValue(const std::string& stationId, const std::string& name) {
    std::string to_return;

    try {
        if (!initialized && !initialize()) {
            return to_return; // Return empty string if initialization fails
        }

        std::string stationKey = "station_" + stationId;

        if (cfg.exists(stationKey)) {
            const libconfig::Setting& record = cfg.lookup(stationKey);
            record.lookupValue(name, to_return);
        } else {
            std::cerr << "Configuration for station id " << stationId << " not found, skipping." << std::endl;
        }
    } catch(const libconfig::SettingNotFoundException &snfex) {
        std::cerr << "Setting not found: " << snfex.getPath() << std::endl;
    } catch(const libconfig::SettingTypeException &stex) {
        std::cerr << "Setting type mismatch: " << stex.getPath() << std::endl;
    }

    return to_return;
}

bool Config::database(std::string& host, std::string& user, std::string& pass, std::string& db) {
    try {
        if (!initialized && !initialize()) {
            return false; // Return false if initialization fails
        }

        const libconfig::Setting& root = cfg.getRoot()["database"];

        root.lookupValue("host", host);
        root.lookupValue("user", user);
        root.lookupValue("pass", pass);
        root.lookupValue("db", db);

        return true;
    } catch(const libconfig::SettingNotFoundException &snfex) {
        std::cerr << "Setting not found: " << snfex.getPath() << std::endl;
    } catch(const libconfig::SettingTypeException &stex) {
        std::cerr << "Setting type mismatch: " << stex.getPath() << std::endl;
    }

    return false;
}
