#include <yavsg/logging.hpp>

#include <fmt/ostream.h>    // print

#include <iostream> // clog


namespace
{
    using namespace std::string_view_literals;
}


JadeMatrix::yavsg::log_handle::log_handle() : ext::log::handle( {
    /* should_emit_for = */ []( ext::log::level ){ return true; },
    /* emit = */ [](
        ext::log::level                       lvl,
        std::chrono::system_clock::time_point now,
        ext::source_location                  loc,
        std::string&&                         msg
    )
    {
        switch( lvl )
        {
            using ext::log::level;
        case level::verbose: std::clog << "\033[35m\033[1mverbose: "sv; break;
        case level::info   : std::clog << "\033[32m\033[1minfo: "sv   ; break;
        case level::warning: std::clog << "\033[33m\033[1mwarning: "sv; break;
        case level::error  : std::clog << "\033[31m\033[1merror: "sv  ; break;
        }
        
        fmt::print(
            std::clog,
            "\033[0m{}\033[90m\033[3m ({}:{}:{})\033[0m\n"sv,
            msg,
            loc.file_name,
            loc.line,
            loc.column
        );
    }
} )
{}
