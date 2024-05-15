#include "database.h"
#include "config.h"

Database database;

MYSQL_RES *result;
MYSQL_ROW row;
int query_state;
std::mutex db_mutex;
MYSQL mysql;
MYSQL* Database::connection = nullptr;

bool Database::connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& db) {
    if (connection == nullptr) {
        std::cout << "Connecting to MySQL...";
        mysql_init(&mysql);
        connection = mysql_real_connect(&mysql, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), 0, 0, 0);

        if (connection == NULL) {
            std::cout << "Connection failed: " << mysql_error(&mysql) << std::endl;
            return false;
        } else {
            mysql_set_character_set(connection, "utf8mb4");
            std::cout << "Connection established." << std::endl;
            return true;
        }
    } else {
        return true;
    }
}

void Database::check() {
    if (connection == NULL || mysql_ping(connection)) {
        Config config;

        std::cout << "Lost MySQL connection. Attempting to reconnect..." << std::endl;
        std::string host, user, pass, db;
        if (!config.database(host, user, pass, db)) {
            std::cerr << "Failed to read database configuration from config.ini." << std::endl;
            exit(1);
        }
        if (!Database::connect(host, user, pass, db)) {
            std::cerr << "Failed to reconnect to database." << std::endl;
            exit(1);
        }
    }
}

std::vector<std::vector<std::string>> Database::multiGet(const char* query) {
    std::lock_guard<std::mutex> guard(db_mutex);
    std::vector<std::vector<std::string>> dataRows;
    Database::check(); // Ensure the connection is alive before executing the query
    query_state = mysql_query(connection, query);
    if (query_state != 0) {
        std::cout << mysql_error(connection) << std::endl;
    } else {
        result = mysql_store_result(connection);
        int num_fields = mysql_num_fields(result);
        while ((row = mysql_fetch_row(result)) != NULL) {
            std::vector<std::string> dataRow;
            for(int i = 0; i < num_fields; i++) {
                dataRow.push_back(row[i] ? row[i] : "NULL");
            }
            dataRows.push_back(dataRow);
        }
        mysql_free_result(result);
    }
    return dataRows;
}

bool Database::singleGet(const std::string& input) {
    std::lock_guard<std::mutex> guard(db_mutex); // Ensure thread safety
    Database::check(); // Ensure the connection is alive

    // Form the query string
    std::string query = input;

    // Execute the query
    if (mysql_query(connection, query.c_str())) {
        std::cerr << "MySQL query failed: " << mysql_error(connection) << std::endl;
        // Attempt to reconnect if the connection is lost
        if (mysql_errno(connection) == CR_SERVER_LOST || mysql_errno(connection) == CR_SERVER_GONE_ERROR) {
            Database::check();
        }
        return false;
    }

    // Store the result
    MYSQL_RES* result = mysql_store_result(connection);
    if (!result) {
        std::cerr << "MySQL store result failed: " << mysql_error(connection) << std::endl;
        mysql_free_result(result);
        return false;
    }

    // Check if there are any rows returned
    bool hasResult = (mysql_num_rows(result) > 0);
    mysql_free_result(result); // Free the result after use

    return hasResult;
}

bool Database::execute(const std::string& sqlQuery) {
    std::lock_guard<std::mutex> guard(db_mutex);
    Database::check();

    if (mysql_query(connection, sqlQuery.c_str())) {
        std::cout << "Query: " << sqlQuery << std::endl;
        std::cerr << "Error executing SQL query: " << mysql_error(connection) << std::endl;
        return false;
    }

    if (mysql_affected_rows(connection) > 0) {
        return true;
    } else {
        return false;
    }
    
}