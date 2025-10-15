#include "clipod/brain.h"

#include "uiLoop.h"

#include <ctrl-c.h>

#include <cxxopts.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <fileref.h>
#include <tag.h>

#include <memory>
#include <string>

int main (int argc_, char **argv_)
{
    cxxopts::Options options ("clipod", "Music for the peeple");

    options.add_options () ("h,help", "Print usage");

    auto result = options.parse (argc_, argv_);

    std::filesystem::current_path (".");
    auto pb = clipod::brain_t::bb ()->pb;

    auto logger = spdlog::basic_logger_mt ("basic_logger", "logs/clipod.log");

    spdlog::set_default_logger (logger);
    spdlog::set_level (spdlog::level::debug);

    if (!pb)
    {
        return -1;
    }

    if (result.count ("help"))
    {
        printf ("%s\n", options.help ().data ());
        return 0;
    }

    auto db = clipod::brain_t::bb ()->db;
    auto qu = clipod::brain_t::bb ()->qu;

    printf ("%s\n", std::filesystem::current_path ().string ().c_str ());

    std::filesystem::current_path ("./Music");

    auto all = db->allTrackIds ();

    qu->enqueue (all);

    uiLoop_t uiLoop;

    uiLoop.run ();

    pb->unload ();

    return 0;
}
