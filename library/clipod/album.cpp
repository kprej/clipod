#include "clipod/album.h"
#include "clipod/databaseUtil.h"

using namespace clipod;

album_t::album_t ()
    : id ()
    , name ()
{
}

album_t::album_t (muuid::uuid const &id_)
    : id (id_)
    , name ()
{
}

album_t::album_t (SQLite::Statement const &statement_)
    : id (fromBlob (statement_.getColumn (album_t::C_ID).getBlob ()))
    , artist (fromBlob (statement_.getColumn (C_ARTISTID).getBlob ()))
    , name (statement_.getColumn (C_NAME).getString ())
{
}
