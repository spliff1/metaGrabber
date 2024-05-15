#include "meta-text.h"
#include "utils.h"
#include "fetch.h"
#include "database.h"

static Fetch fetch;

void metaText::process(const std::string& id, const std::string& name, const std::string& meta_type, const std::string& meta_url) {

  std::string artist, title;

  // Fetch webpage content
  std::string webPageContent = fetch.page(meta_url, name);


    if (name == "Radio Cartier") {
        size_t dashPosition = webPageContent.find(" - ");
        if (dashPosition != std::string::npos) {
            webPageContent = webPageContent.substr(dashPosition + 3);
        }
    }

    size_t separatorPos = webPageContent.find(" - ");
    if (separatorPos != std::string::npos) {
        artist = webPageContent.substr(0, separatorPos);
        title = utils.cleanTitle(webPageContent.substr(separatorPos + 3), name); // +3 to skip " - "
    } else {
        title = utils.cleanTitle(webPageContent, name);
    }


    // 2. Radio-Specific Handling
    if (name == "Radio ZU" || name == "Radio Guerrilla") {
        if (title == name) {
            artist.clear();
            title.clear();
        }
    }

    std::string escapedId = utils.escape(id);
    std::string escapedArtist = utils.escape(artist);
    std::string escapedTitle = utils.escape(title);

    std::stringstream checkQuery;
    checkQuery << "SELECT artist, title FROM metadata WHERE station = '" << escapedId
                << "' AND id = (SELECT MAX(id) FROM metadata WHERE station = '" << escapedId << "') "
                << "AND (artist = '" << escapedArtist << "' OR (artist IS NULL AND '" << escapedArtist << "' IS NULL)) "
                << "AND (title = '" << escapedTitle << "' OR (title IS NULL AND '" << escapedTitle << "' IS NULL));"; 

    // Database operations with conditional insert and delete
    if (!database.singleGet(checkQuery.str())) {
        if (!artist.empty() || !title.empty()) {
            std::stringstream insertQuery;
            insertQuery << "INSERT INTO metadata (station, artist, title) VALUES ('" << escapedId << "', '" << escapedArtist << "', '" << escapedTitle << "');";
            database.execute(insertQuery.str());
        }
        std::stringstream deleteQuery;
        deleteQuery << "DELETE m1 FROM metadata m1 LEFT JOIN (SELECT id FROM (SELECT id FROM metadata WHERE station = '" << escapedId << "' ORDER BY id DESC LIMIT 10) AS subquery) AS m2 ON m1.id = m2.id WHERE m1.station = '" << escapedId << "' AND m2.id IS NULL;";
        database.execute(deleteQuery.str());
    }

    std::cout << "Processing " << name << " (" << id << "): " << meta_url << " | " << escapedArtist << " - " << escapedTitle << std::endl;
}
