#include "clipod/databaseUtil.h"

#include <array>

muuid::uuid clipod::fromBlob (void const *blob_)
{
    return muuid::uuid (*static_cast<std::array<uint8_t, 16> const *> (blob_));
}

// clang-format off
std::string const &clipod::ARTIST_LOOKUP_GLOB = "SELECT ";

std::string const &clipod::CREATE_TRACK_TABLE =
"CREATE TABLE IF NOT EXISTS TRACKS("
"ID BLOB PRIMARY KEY     NOT NULL,"
"TITLE           TEXT    NOT NULL,"
"FILEPATH        TEXT    NOT NULL,"
"NUMBER          INT     NOT NULL,"
"ARTISTID        BLOB    NOT NULL,"
"ALBUMID         BLOB    NOT NULL,"
"DISC            INT,"
"SAMPLE_RATE     INT,"
"LENGTH          INT,"
"FOREIGN KEY (ARTISTID)  REFERENCES ARTIST(ID),"
"FOREIGN KEY (ALBUMID)  REFERENCES ALBUM(ID),"
"UNIQUE(FILEPATH));";

std::string const &TRACK_SAMPLE_RATE_AND_PATH_BY_ID = 
"SELECT "
"TRACKS.PATH, "
"TRACKS.SAMPLE_RATE, "
"WHERE TRACKS.ID = ?;";

std::string const &clipod::LOOKUP_TRACK_INFO_BY_ID =
"SELECT "
"TRACKS.TITLE, "
"TRACKS.NUMBER, "
"TRACKS.DISC, "
"TRACKS.LENGTH, "
"ARTISTS.NAME, "
"ALBUMS.NAME FROM TRACKS INNER JOIN ARTISTS ON ARTISTS.ID = TRACKS.ARTISTID "
"INNER JOIN ALBUMS ON ALBUMS.ID = TRACKS.ALBUMID "
"WHERE TRACKS.ID = ?;";

/*
std::string const clipod::LOOKUP_TRACK_BY_ID =
"SELECT "
"TITLE, "
"NUMBER, "
"DISC, "
"LENGTH, "
"ARISTID, "
"ALBUMID, "
"WHERE TRACKS.ID = ?;";
*/

std::string const &clipod::INSERT_TRACK =
"INSERT OR IGNORE INTO TRACKS "
"VALUES("
"?,"
"?,"
"?,"
"?,"
"?,"
"?,"
"?,"
"?,"
"?"
") "
"ON CONFLICT(FILEPATH) DO NOTHING;";
// clang-format on
