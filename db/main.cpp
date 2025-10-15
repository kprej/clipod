#include "clipod/database.h"

#include <cxxopts.hpp>

#include <modern-uuid/uuid.h>

#include <spdlog/spdlog.h>

#include <SQLiteCpp/SQLiteCpp.h>

#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>

#include <filesystem>
#include <string>

int main (int argc_, char **argv_)
{
    cxxopts::Options options ("clipod", "Music for the peeple");

    options.add_options () ("s,scan", "Scan") ("v,validate", "validate db") (
        "a,all", "ALL") ("h,help", "Print usage");

    auto result = options.parse (argc_, argv_);
    spdlog::set_level (spdlog::level::debug);

    if (result.count ("help"))
    {
        printf ("%s\n", options.help ().data ());
        return 0;
    }

    clipod::database_t db;

    std::filesystem::path filePath;

    if (result.count ("scan"))
    {
        if (!std::filesystem::exists ("./Music"))
            return 1;

        db.scanDirectory (std::filesystem::path ("./Music"));
        return 0;
    }

    if (result.count ("validate"))
    {
        if (!std::filesystem::exists ("./Music"))
            return 1;

        db.validateDatabase ();
        return 0;
    }

    if (result.count ("a"))
    {
        auto all = db.getAll ();

        for (auto t : all)
        {
            spdlog::debug ("{} {} {}", t.path.string (), t.title, t.length.count ());
        }
    }

    return 0;
}
