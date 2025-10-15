#pragma once

#include "artist.h"

#include <modern-uuid/uuid.h>

namespace clipod
{
struct album_t
{
    enum fields_t
    {
        F_ID = 1,
        F_ARTISTID = 2,
        F_NAME = 3,
    };

    enum columns_t
    {
        C_ID = 0,
        C_ARTISTID = 1,
        C_NAME = 2,
    };

    muuid::uuid id;
    std::string name;

    artist_t artist;

    album_t ();
    album_t (muuid::uuid const &id_);
    album_t (SQLite::Statement const &statement_);
};
} // namespace clipod
