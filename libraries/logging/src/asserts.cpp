#define DOCTEST_CONFIG_IMPLEMENT

#include <yavsg/asserts.hpp>

#include <yavsg/logging.hpp>

#include <fmt/ostream.h>    // std::quoted support

#include <cstdlib>  // std::abort
#include <iomanip>  // std::quoted
#include <string_view>


namespace
{
    using namespace std::string_view_literals;
    
    auto const log_ = JadeMatrix::yavsg::log_handle();
}


void JadeMatrix::yavsg::doctest_assert_handler(
    doctest::AssertData const& data
)
{
    ext::log::internal::format assert_format(
        /* string   = */ "{}"sv,
        /* when     = */ std::chrono::system_clock::now(),
        /* location = */ {
            /* line          = */ static_cast< std::uint_least32_t >( data.m_line ),
            /* column        = */ 0,
            /* file_name     = */ data.m_file,
            /* function_name = */ ext::source_location::current().function_name,
        }
    );
    
    const bool is_fatal = !( data.m_at & doctest::assertType::is_warn );
    
    const auto assert_level = (
        is_fatal
        ? ext::log::level::error
        : ext::log::level::warning
    );
    
    auto assert_message = fmt::format(
        "{}assertion {}({}"sv,
        is_fatal ? "\033[31m"sv : "\033[33m"sv,
        doctest::assertString( data.m_at ),
        data.m_expr
    );
    
    if( data.m_at & doctest::assertType::is_normal )
    {
        if( data.m_threw )
        {
            assert_message += fmt::format(
                ") threw exception: {}"sv,
                data.m_exception
            );
        }
        else
        {
            assert_message += fmt::format(
                ") failed: {}({})"sv,
                doctest::assertString( data.m_at ),
                data.m_decomp
            );
        }
    }
    else if( data.m_at & doctest::assertType::is_throws_as )
    {
        assert_message += fmt::format(
            ", {}) failed"sv,
            data.m_exception_type
        );
    }
    else if( data.m_at & doctest::assertType::is_throws_with )
    {
        assert_message += fmt::format(
            ", {}) failed"sv,
            std::quoted( data.m_exception_type )
        );
    }
    else // Throw/no-throw checks
    {
        assert_message += ") failed"sv;
    }
    
    if( is_fatal )
    {
        assert_message += "; aborting"sv;
    }
    
    log_.as( assert_level, assert_format, assert_message );
    
    if( is_fatal )
    {
        std::abort();
    }
}
