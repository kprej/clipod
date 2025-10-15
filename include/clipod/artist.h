#pragma once

#include <modern-uuid/uuid.h>

#include <SQLiteCpp/SQLiteCpp.h>

namespace clipod
{

struct artist_t
{
    enum fields_t
    {
        F_ID = 1,
        F_NAME = 2,
    };

    enum columns_t
    {
        C_ID = 0,
        C_NAME = 1,
    };

    muuid::uuid id;
    std::string name;

    artist_t ();
    artist_t (muuid::uuid const &id_);
    artist_t (SQLite::Statement const &statement_);
};
} // namespace clipod
