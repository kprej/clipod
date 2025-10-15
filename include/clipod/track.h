#pragma once

#include "clipod/album.h"
#include "clipod/artist.h"

#include <modern-uuid/uuid.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include <filesystem>
#include <string>

namespace clipod
{

struct track_t
{
    enum fields_t
    {
        F_ID = 1,
        F_TITLE = 2,
        F_FILEPATH = 3,
        F_NUMBER = 4,
        F_ARTISTID = 5,
        F_ALBUMID = 6,
        F_DISC = 7,
        F_SAMPLE_RATE = 8,
        F_LENGTH = 9,
    };

    enum columns_t
    {
        C_ID = 0,
        C_TITLE = 1,
        C_FILEPATH = 2,
        C_NUMBER = 3,
        C_ARTISTID = 4,
        C_ALBUMID = 5,
        C_DISC = 6,
        C_SAMPLE_RATE = 7,
        C_LENGTH = 8,
    };

    muuid::uuid id;
    std::filesystem::path path;
    std::string title;
    int number;
    int disc;
    int sampleRate;
    std::chrono::seconds length;

    album_t album;
    artist_t artist;

    track_t (std::filesystem::path const &path_);
    track_t (SQLite::Statement const &statement_);
    track_t () = default;

    bool operator== (track_t const &rhs_) const;

    void bindSQL (SQLite::Statement &statement_) const;
};

struct trackInfo_t
{
    muuid::uuid id;
    std::string title;
    std::string album;
    std::string artist;
    std::string number;
    std::string path;

    int sampleRate;
    int disc;

    std::chrono::seconds length;

    std::optional<std::chrono::duration<float>> timePoint;
    size_t queuePos;
};

} // namespace clipod
