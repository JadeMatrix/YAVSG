#pragma once


#include <ext/logging.hpp>

#include <string_view>


namespace JadeMatrix::yavsg
{
    class log_handle : public ext::log::handle
    {
        static constexpr std::string_view passthrough_fmt_{ "{}" };
        
    public:
        log_handle();
        
        // Take a message string (e.g. from an external library) and split it by
        // lines, logging each with the given level and format (optional).  If a
        // custom format is given, it must format exactly one string argument.
        void split_on_newlines_as(
            ext::log::level                   lvl,
            std::string_view                  messages,
            ext::log::internal::format const& with_format = passthrough_fmt_
        ) const;
    };
}
