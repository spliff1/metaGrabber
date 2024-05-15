#include "meta-json.h"
#include "utils.h"
#include "database.h"
#include "config.h"
#include "fetch.h"

Utils utils;
Fetch fetch;

void metaJson::process(const std::string& id, const std::string& name, const std::string& meta_type, const std::string& meta_url) {

    rapidjson::Document json = fetch.json(meta_url, name);

    // Process JSON & early exit if JSON is invalid
    if (json.IsNull()) {
        std::cerr << name << " | JSON is empty, skipping." << std::endl;
        return;  
    } else {

        // Init
        std::string artist, title, show, artistKey, titleKey, showKey;
        artistKey = "artist";
        titleKey = "title";
        showKey = "show";
        artist = "NULL";
        title = "NULL";
        show = "NULL";


        const rapidjson::Value* jArtist = rapidjson::Pointer(config.getValue(id, artistKey).c_str()).Get(json);
        const rapidjson::Value* jTitle  = rapidjson::Pointer(config.getValue(id, titleKey).c_str()).Get(json);
        const rapidjson::Value* jShow   = rapidjson::Pointer(config.getValue(id, showKey).c_str()).Get(json);



        if (jArtist && jArtist->IsString()) {
            artist = jArtist ? ("'" + utils.escape(jArtist->GetString()) + "'") : "NULL";
        }

        if (jTitle && jTitle->IsString()) {
            title = jTitle ? ("'" + utils.escape(jTitle->GetString()) + "'") : "NULL";
            if (artist.empty() && !title.empty()) {
                size_t separatorPos = title.find(" - ");
                if (separatorPos != std::string::npos) {
                    artist = title.substr(0, separatorPos);
                    title = title.substr(separatorPos + 3); // +3 to skip the " - "
                }
            }
        }

        if (jShow && jShow->IsString()) {
            show = jShow ? ("'" + utils.escape(jShow->GetString()) + "'") : "NULL";
        }
        
        std::string escapedId = utils.escape(id);

        // Cut the Radio Impuls crap
        if (name == "Radio Impuls") {
            if (title == "'www.radioimpuls.ro'" || title == "'#WeAreImpuls'") {
                title = "NULL";
                artist = "NULL";
                show = "NULL";
            }
        }

        if (name == "Kiss FM") {
            if (artist == "'#1 Hit Radio'" && title == "'Kiss FM'") {
                artist = "NULL";
                title = "NULL";
                show = "NULL";
            }
        }

        if (name == "Rock FM") {
            if (artist == "'It Rocks!'" && title == "'Rock FM'") {
                title = "NULL";
                artist = "NULL";
                show = "NULL";
            }
        }

        std::string checkQuery = "SELECT artist, title, show_name FROM metadata WHERE station = " +
                                 escapedId +
                                 " AND id IN (SELECT MAX(id) FROM metadata WHERE station = " +
                                 escapedId +
                                 ") AND ((artist = " +
                                 artist +
                                 ") OR (artist IS NULL AND " +
                                 artist +
                                 " IS NULL)) AND ((title = " +
                                 title +
                                 ") OR (title IS NULL AND " +
                                 title +
                                 " IS NULL)) AND ((show_name = " +
                                 show +
                                 ") OR (show_name IS NULL AND " +
                                 show +
                                 " IS NULL));";

        if (!database.singleGet(checkQuery)) {
            if (artist != "NULL" || title != "NULL" || show != "NULL") {
                database.execute("INSERT INTO metadata (station, artist, title, show_name) VALUES ('" + escapedId + "', " + artist + ", " + title + ", " + show + ");");
            }
            database.execute("DELETE m1 FROM metadata m1 LEFT JOIN (SELECT id FROM (SELECT id FROM metadata WHERE station = " + escapedId + " ORDER BY id DESC LIMIT 10) AS subquery) AS m2 ON m1.id = m2.id WHERE m1.station = " + escapedId + " AND m2.id IS NULL;");
        }

        //std::cout << "Processing " << name << " (" << id << "): " << meta_url << " | " << artist << " - " << title << std::endl;

    }
}
