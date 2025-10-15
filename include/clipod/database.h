#pragma once

#include "clipod/album.h"
#include "clipod/artist.h"
#include "clipod/track.h"

#include <SQLiteCpp/Database.h>
#include <modern-uuid/uuid.h>

#include <spdlog/spdlog.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include <filesystem>

namespace clipod
{
struct transaction_t;

struct soundSourceQuery_t
{
    std::filesystem::path path;
    int sampleRate;
};

class database_t
{
public:
    ~database_t ();
    database_t ();

    void scanDirectory (std::filesystem::path const &path_, bool recursive_ = true);
    void validateDatabase ();

    transaction_t raiiTransaction ();

    trackInfo_t trackInfo (muuid::uuid const &id_);
    soundSourceQuery_t soundSource (muuid::uuid const &id_);
    std::filesystem::path trackFilePath (muuid::uuid const &id_);

    std::vector<artist_t> allArtistInfo ();
    std::vector<album_t> artistAlbumsInfo (muuid::uuid const &id_);
    std::vector<track_t> albumTrackInfo (muuid::uuid const &id_);
    std::vector<muuid::uuid> allTrackIds ();
    std::vector<track_t> getAll ();

private:
    void processDirectoryEntry (std::filesystem::directory_entry const &entry_,
                                SQLite::Database &db_);

    void addTrack (track_t const &track_, SQLite::Database &db_);
    void addArtist (artist_t const &artist_, SQLite::Database &db_);
    void addAlbum (album_t const &artist_, SQLite::Database &db_);
};

struct transaction_t
{
    ~transaction_t ()
    {
        if (transaction.use_count () == 1)
        {
            transaction->commit ();
            spdlog::warn ("Commited");
        }
    }

private:
    friend class database_t;
    std::shared_ptr<SQLite::Transaction> transaction;
};
} // namespace clipod
