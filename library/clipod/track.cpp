#include "clipod/track.h"
#include "clipod/databaseUtil.h"
using namespace clipod;

#include <spdlog/spdlog.h>

#include <fileref.h>
#include <tag.h>
#include <tfilestream.h>
#include <tpropertymap.h>

#include <filesystem>

track_t::track_t (std::filesystem::path const &path_)
    : id ()
    , path (std::filesystem::relative (path_))
    , title ()
    , number ()
    , disc (1)
    , sampleRate ()
    , length ()
    , album ()
    , artist ()
{
    auto tagref = TagLib::FileRef (path_.string ().c_str ());

    auto props = tagref.properties ();

    std::optional<muuid::uuid> trackId;
    if (props.contains ("MUSICBRAINZ_TRACKID"))
        trackId = muuid::uuid::from_chars (
            props.value ("MUSICBRAINZ_TRACKID").toString ().to8Bit (true));

    std::optional<muuid::uuid> albumId;
    if (props.contains ("MUSICBRAINZ_ALBUMID"))
        albumId = muuid::uuid::from_chars (
            props.value ("MUSICBRAINZ_ALBUMID").toString ().to8Bit (true));

    std::optional<muuid::uuid> albumArtistId;
    if (props.contains ("MUSICBRAINZ_ALBUMARTISTID"))
        albumArtistId = muuid::uuid::from_chars (
            props.value ("MUSICBRAINZ_ALBUMARTISTID").toString ().to8Bit (true));

    if (props.contains ("ALBUMARTIST"))
        artist.name = props.value ("ALBUMARTIST").toString ().to8Bit (true);

    id = trackId ? trackId.value () : muuid::uuid::generate_reordered_time_based ();
    album.id = albumId ? albumId.value () : muuid::uuid::generate_reordered_time_based ();
    artist.id = albumArtistId ? albumArtistId.value ()
                              : muuid::uuid::generate_reordered_time_based ();

    album.name = tagref.tag ()->album ().to8Bit (true);
    album.artist = artist;

    title = tagref.tag ()->title ().to8Bit (true);
    number = tagref.tag ()->track ();
    sampleRate = tagref.audioProperties ()->sampleRate ();
    length = std::chrono::seconds (tagref.audioProperties ()->lengthInSeconds ());

    if (props.contains ("DISCNUMBER"))
        disc = std::stoi (props.value ("DISCNUMBER")
                              .toString ()
                              .split ("/")
                              .begin ()
                              ->toCString (true));
}

track_t::track_t (SQLite::Statement const &statement_)
    : id (fromBlob (statement_.getColumn (C_ID).getBlob ()))
    , path (statement_.getColumn (C_FILEPATH).getString ())
    , title (statement_.getColumn (C_TITLE).getString ())
    , number (statement_.getColumn (C_NUMBER).getInt ())
    , disc (statement_.getColumn (C_DISC).getInt ())
    , length (statement_.getColumn (C_LENGTH).getInt ())
    , album (fromBlob (statement_.getColumn (C_ALBUMID)))
    , sampleRate (statement_.getColumn (C_SAMPLE_RATE).getInt ())
    , artist (fromBlob (statement_.getColumn (C_ARTISTID)))
{
}

bool track_t::operator== (track_t const &rhs_) const
{
    return id == rhs_.id && path == rhs_.path && title == rhs_.title &&
           number == rhs_.number && disc == rhs_.disc && length == rhs_.length &&
           album.id == rhs_.album.id && sampleRate == rhs_.sampleRate &&
           artist.id == rhs_.artist.id;
}

void track_t::bindSQL (SQLite::Statement &statement_) const
{
    statement_.bind (F_ID, id.bytes.data (), id.bytes.size ());
    statement_.bind (F_TITLE, title);
    statement_.bind (F_FILEPATH, path.string ());
    statement_.bind (F_NUMBER, number);
    statement_.bind (F_ARTISTID, artist.id.bytes.data (), artist.id.bytes.size ());
    statement_.bind (F_ALBUMID, album.id.bytes.data (), album.id.bytes.size ());
    statement_.bind (F_DISC, disc);
    statement_.bind (F_LENGTH, length.count ());
    statement_.bind (F_SAMPLE_RATE, sampleRate);
}
