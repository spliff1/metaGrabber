#include <string>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/pointer.h>
#include <iostream>
#include <ostream>

class metaJson {
    public:
        void process(const std::string& id, const std::string& name, const std::string& meta_type, const std::string& meta_url);
};