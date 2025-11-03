#pragma once

#include <filesystem>
#include <memory>

namespace clipod
{
class settings_t
{
public:
    ~settings_t ();
    settings_t ();

    /// \returns True if the file exists
    bool configFile (std::filesystem::path const &configFile_);

    /// \returns True if the file is formatted correctly
    bool parse ();

    /// \returns True if the file saves
    bool save ();

    /// \brief Location of the root music directory
    std::filesystem::path const &musicDir () const;

    void setMusicDir (std::filesystem::path const &musicDir_);

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
} // namespace clipod
