#include "clipod/database.h"
#include "clipod/databaseUtil.h"
#include <SQLiteCpp/Transaction.h>
#include <spdlog/spdlog.h>
using namespace clipod;

bool isValidExt (std::filesystem::path const &ext_)
{
    return ext_ == ".wav" || ext_ == ".m4a" || ext_ == ".flac" || ext_ == ".mp3";
}

database_t::~database_t ()
{
}

database_t::database_t ()
{
}

void database_t::scanDirectory (std::filesystem::path const &path_, bool recursive_)
{
    bool createTables = !std::filesystem::exists ("media.db");

    auto m_db =
        SQLite::Database ("media.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    if (createTables)
    {
        m_db.exec ("CREATE TABLE IF NOT EXISTS ARTISTS("
                   "ID BLOB PRIMARY KEY     NOT NULL,"
                   "NAME           TEXT     NOT NULL,"
                   "UNIQUE(NAME));");

        m_db.exec ("CREATE TABLE IF NOT EXISTS ALBUMS("
                   "ID BLOB PRIMARY KEY     NOT NULL,"
                   "ARTISTID        BLOB    NOT NULL,"
                   "NAME           TEXT     NOT NULL,"
                   "FOREIGN KEY (ARTISTID)  REFERENCES ARTIST(ID),"
                   "UNIQUE(NAME));");

        m_db.exec (CREATE_TRACK_TABLE);
    }

    auto const currentPath = std::filesystem::current_path ();
    spdlog::debug ("Scanning: {}", path_.string ());
    std::filesystem::current_path (path_);

    auto transaction = SQLite::Transaction (m_db);
    if (recursive_)
    {
        for (auto const &dirIter : std::filesystem::recursive_directory_iterator (
                 std::filesystem::current_path ()))
        {
            processDirectoryEntry (dirIter, m_db);
        }
        transaction.commit ();
        return;
    }

    for (auto const &dirIter :
         std::filesystem::directory_iterator (std::filesystem::current_path ()))
    {
        processDirectoryEntry (dirIter, m_db);
    }
    std::filesystem::current_path (currentPath);
    transaction.commit ();
}

void database_t::validateDatabase ()
{
    auto m_db =
        SQLite::Database ("media.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    auto m_getAll = SQLite::Statement (m_db, "SELECT * FROM TRACKS;");

    std::vector<muuid::uuid> toDelete;

    while (m_getAll.executeStep ())
    {
        track_t dbTrack (m_getAll);
        if (!std::filesystem::exists (dbTrack.path))
        {
            spdlog::debug ("File does not exist: {}", dbTrack.title);
            toDelete.push_back (dbTrack.id);
            continue;
        }

        track_t file (dbTrack.path);
        if (file == dbTrack)
            continue;

        spdlog::debug ("Files mismatch: {}", dbTrack.title);
    }

    auto transaction = SQLite::Transaction (m_db);
    for (auto id : toDelete)
    {
        auto del = SQLite::Statement (m_db, "DELETE FROM TRACKS WHERE ID == ?;");
        del.bind (1, id.bytes.data (), id.bytes.size ());
        del.exec ();
    }
    transaction.commit ();
}

transaction_t database_t::raiiTransaction ()
{
    transaction_t t;
    // t.transaction = std::make_shared<SQLite::Transaction> (m_db);

    return t;
}

void database_t::addTrack (track_t const &track_, SQLite::Database &db_)
{
    spdlog::debug ("Adding: {}", track_.title);
    auto m_insertTrack = SQLite::Statement (db_, INSERT_TRACK);
    track_.bindSQL (m_insertTrack);
    m_insertTrack.exec ();
    m_insertTrack.clearBindings ();
    m_insertTrack.reset ();
}

void database_t::addArtist (artist_t const &artist_, SQLite::Database &db_)
{
    auto m_insertArtist = SQLite::Statement (
        db_, "INSERT OR IGNORE INTO ARTISTS VALUES(?, ?) ON CONFLICT(NAME) DO NOTHING;");
    m_insertArtist.bind (
        artist_t::F_ID, artist_.id.bytes.data (), artist_.id.bytes.size ());
    m_insertArtist.bind (artist_t::F_NAME, artist_.name);

    m_insertArtist.exec ();
    m_insertArtist.clearBindings ();
    m_insertArtist.reset ();
}

void database_t::addAlbum (album_t const &album_, SQLite::Database &db_)
{
    auto m_insertAlbum = SQLite::Statement (
        db_,
        "INSERT OR IGNORE INTO ALBUMS VALUES(?, ?, ?) ON CONFLICT(NAME) DO NOTHING;");

    m_insertAlbum.bind (album_t::C_ID, album_.id.bytes.data (), album_.id.bytes.size ());
    m_insertAlbum.bind (album_t::C_ARTISTID,
                        album_.artist.id.bytes.data (),
                        album_.artist.id.bytes.size ());
    m_insertAlbum.bind (album_t::C_NAME, album_.name);

    m_insertAlbum.exec ();
    m_insertAlbum.clearBindings ();
    m_insertAlbum.reset ();
}

trackInfo_t database_t::trackInfo (muuid::uuid const &id_)
{
    auto m_db = SQLite::Database ("media.db", SQLite::OPEN_READONLY);
    trackInfo_t trackInfo;
    trackInfo.id = id_;
    auto st = SQLite::Statement (m_db, LOOKUP_TRACK_INFO_BY_ID);

    st.bind (1, id_.bytes.data (), id_.bytes.size ());
    while (st.executeStep ())
    {
        trackInfo.title = st.getColumn (0).getString ();
        trackInfo.number = st.getColumn (1).getInt ();
        trackInfo.disc = st.getColumn (2);
        trackInfo.length = std::chrono::seconds (st.getColumn (3));
        trackInfo.artist = st.getColumn (4).getString ();
        trackInfo.album = st.getColumn (5).getString ();
    }

    return trackInfo;
}

soundSourceQuery_t database_t::soundSource (muuid::uuid const &id_)
{
    auto m_db = SQLite::Database ("media.db", SQLite::OPEN_READONLY);
    soundSourceQuery_t soundSource;

    auto m_lookupSoundSource =
        SQLite::Statement (m_db,
                           "SELECT FILEPATH, SAMPLE_RATE FROM TRACKS "
                           "WHERE TRACKS.ID = ?;");
    m_lookupSoundSource.bind (1, id_.bytes.data (), id_.bytes.size ());
    while (m_lookupSoundSource.executeStep ())
    {
        soundSource.path = m_lookupSoundSource.getColumn (0).getString ();
        soundSource.sampleRate = m_lookupSoundSource.getColumn (1).getInt ();
    }

    m_lookupSoundSource.clearBindings ();
    m_lookupSoundSource.reset ();

    return soundSource;
}

std::filesystem::path database_t::trackFilePath (muuid::uuid const &id_)
{
    auto db = SQLite::Database ("media.db", SQLite::OPEN_READONLY);
    std::filesystem::path path;
    auto st = SQLite::Statement (db,
                                 "SELECT FILEPATH FROM TRACKS "
                                 "WHERE TRACKS.ID = ?;");
    st.bind (1, id_.bytes.data (), id_.bytes.size ());
    while (st.executeStep ())
    {
        path = std::filesystem::path (st.getColumn (0).getString ());
    }

    assert (std::filesystem::exists (path));

    return path;
}

std::vector<artist_t> database_t::allArtistInfo ()
{
    std::vector<artist_t> out;
    try
    {
        auto m_db = SQLite::Database ("./media.db", SQLite::OPEN_READONLY);

        auto m_allArtists = SQLite::Statement (m_db, "SELECT ID, NAME FROM ARTISTS;");

        while (m_allArtists.executeStep ())
        {
            out.push_back (artist_t (m_allArtists));
        }
    }
    catch (std::exception e_)
    {
        spdlog::error ("{}", e_.what ());
    }

    return out;
}

std::vector<album_t> database_t::artistAlbumsInfo (muuid::uuid const &id_)
{
    std::vector<album_t> out;
    try
    {
        auto m_db = SQLite::Database ("./media.db", SQLite::OPEN_READONLY);

        auto albums = SQLite::Statement (m_db,
                                         "SELECT ID, NAME FROM ALBUMS WHERE "
                                         "ARTISTID = ?;");
        albums.bind (1, id_.bytes.data (), id_.bytes.size ());

        while (albums.executeStep ())
        {
            album_t album;
            album.id = fromBlob (albums.getColumn (0).getBlob ());
            album.name = albums.getColumn (1).getString ();
            out.push_back (album);
        }
    }
    catch (std::exception e_)
    {
        spdlog::error ("{}", e_.what ());
    }

    return out;
}

std::vector<track_t> database_t::albumTrackInfo (muuid::uuid const &id_)
{
    std::vector<track_t> out;
    try
    {
        auto m_db = SQLite::Database ("./media.db", SQLite::OPEN_READONLY);

        auto tracks = SQLite::Statement (m_db,
                                         "SELECT ID, TITLE FROM TRACKS WHERE "
                                         "ALBUMID = ? ORDER BY DISC ASC, NUMBER ASC;");
        tracks.bind (1, id_.bytes.data (), id_.bytes.size ());

        while (tracks.executeStep ())
        {
            track_t track;
            track.id = fromBlob (tracks.getColumn (0).getBlob ());
            track.title = tracks.getColumn (1).getString ();
            out.push_back (track);
        }
    }
    catch (std::exception e_)
    {
        spdlog::error ("{}", e_.what ());
    }

    return out;
}

std::vector<muuid::uuid> database_t::allTrackIds ()
{
    std::vector<muuid::uuid> out;
    try
    {
        printf ("%s\n", std::filesystem::current_path ().string ().c_str ());
        auto m_db = SQLite::Database ("./media.db", SQLite::OPEN_READONLY);

        auto m_allTrackIds = SQLite::Statement (m_db, "SELECT ID FROM TRACKS;");

        while (m_allTrackIds.executeStep ())
        {
            out.push_back (fromBlob (m_allTrackIds.getColumn (0).getBlob ()));
        }
    }
    catch (std::exception e_)
    {
        printf ("%s", e_.what ());
    }

    return out;
}

std::vector<track_t> database_t::getAll ()
{
    auto m_db = SQLite::Database ("media.db", SQLite::OPEN_READONLY);
    auto m_getAll = SQLite::Statement (m_db, "SELECT * FROM TRACKS;");
    std::vector<track_t> out;
    while (m_getAll.executeStep ())
    {
        out.push_back (track_t (m_getAll));
    }

    return out;
}

void database_t::processDirectoryEntry (std::filesystem::directory_entry const &entry_,
                                        SQLite::Database &db_)
{
    if (!entry_.path ().has_extension ())
    {
        return;
    }

    if (!isValidExt (entry_.path ().extension ()))
    {
        return;
    }

    track_t track (entry_.path ());

    addArtist (track.artist, db_);
    addAlbum (track.album, db_);
    addTrack (track, db_);
}
