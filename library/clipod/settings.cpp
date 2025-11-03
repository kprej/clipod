#include "clipod/settings.h"

using namespace clipod;

#include <spdlog/spdlog.h>

class settings_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : configFile ()
        , musicDir ()
    {
    }

    std::filesystem::path configFile;
    std::filesystem::path musicDir;
};

settings_t::~settings_t ()
{
}

settings_t::settings_t ()
    : m_d (std::make_unique<private_t> ())
{
}

bool settings_t::configFile (std::filesystem::path const &configFile_)
{
    if (!std::filesystem::exists (configFile_))
    {
        spdlog::warn ("Config file does not exist: {}", configFile_.string ());
        return false;
    }

    m_d->configFile = configFile_;
    return true;
}

std::filesystem::path const &settings_t::musicDir () const
{
    return m_d->musicDir;
}

void settings_t::setMusicDir (std::filesystem::path const &musicDir_)
{
    m_d->musicDir = musicDir_;
}
