#pragma once

#include <modern-uuid/uuid.h>

namespace clipod
{

extern std::string const &ARTIST_LOOKUP_GLOB;
extern std::string const &CREATE_TRACK_TABLE;
extern std::string const &TRACK_SAMPLE_RATE_AND_PATH_BY_ID;
extern std::string const &LOOKUP_TRACK_INFO_BY_ID;
extern std::string const &UPDATE_TRACK_BY_ID;
extern std::string const &INSERT_TRACK;

extern muuid::uuid fromBlob (void const *blob_);
} // namespace clipod
