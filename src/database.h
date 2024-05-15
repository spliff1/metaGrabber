#include <mysql.h>
#include <mysqld_error.h>
#include <mutex>
#include <string>
#include <iostream>
#include <vector>

class Database {
    public:
        bool connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
        void check();
        std::vector<std::vector<std::string>> multiGet(const char* query);
        bool singleGet(const std::string& input);
        bool execute(const std::string& sqlQuery);

    private:
        static MYSQL *connection;

};

extern Database database;
