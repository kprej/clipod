#include "clipod/artist.h"
#include "clipod/databaseUtil.h"

using namespace clipod;

artist_t::artist_t ()
    : id ()
    , name ()
{
}

artist_t::artist_t (muuid::uuid const &id_)
    : id (id_)
    , name ()
{
}

artist_t::artist_t (SQLite::Statement const &statement_)
    : id (fromBlob (statement_.getColumn (artist_t::C_ID).getBlob ()))
    , name (statement_.getColumn (C_NAME).getString ())
{
}
