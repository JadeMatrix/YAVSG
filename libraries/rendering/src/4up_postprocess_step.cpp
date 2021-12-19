#include <yavsg/rendering/4up_postprocess_step.hpp>

#include <yavsg/gl/error.hpp>
#include <yavsg/gl/shader.hpp>
#include <yavsg/rendering/basic_postprocess_step.hpp>
#include <yavsg/rendering/shader_utils.hpp> // shaders_dir
#include <yavsg/rendering/shader_variable_names.hpp>

#include <doctest/doctest.h>

#include <limits>
#include <string>
#include <tuple>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    
    using child_ptr = std::unique_ptr<
        JadeMatrix::yavsg::debug_4up_postprocess_step::child_type
    >;
    
    child_ptr child_step_or_default( child_ptr maybe_child )
    {
        if( maybe_child )
        {
            return maybe_child;
        }
        return std::make_unique< JadeMatrix::yavsg::basic_postprocess_step >(
            JadeMatrix::yavsg::shaders_dir() / "postprocess.frag"sv
        );
    }
}


JadeMatrix::yavsg::debug_4up_postprocess_step::debug_4up_postprocess_step(
    std::unique_ptr< child_type > top_left,
    std::unique_ptr< child_type > top_right,
    std::unique_ptr< child_type > bottom_left,
    std::unique_ptr< child_type > bottom_right
) :
    top_left    { child_step_or_default( std::move( top_left     ) ) },
    top_right   { child_step_or_default( std::move( top_right    ) ) },
    bottom_left { child_step_or_default( std::move( bottom_left  ) ) },
    bottom_right{ child_step_or_default( std::move( bottom_right ) ) }
{}

void JadeMatrix::yavsg::debug_4up_postprocess_step::run(
    source_type          const& source,
    gl::write_only_framebuffer& target
)
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear(
          GL_COLOR_BUFFER_BIT
        | GL_DEPTH_BUFFER_BIT
        | GL_STENCIL_BUFFER_BIT
    );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't clear buffer for yavsg::debug_4up_postprocess_step::run()"s
    );
    
    glDisable( GL_DEPTH_TEST );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't disable depth testing for "
        "yavsg::debug_4up_postprocess_step::run()"s
    );
    
    for( auto [ step, x_offset_factor, y_offset_factor ] : {
        std::tuple
        {     top_left.get(), 0u, 1u },
        {    top_right.get(), 1u, 1u },
        {  bottom_left.get(), 0u, 0u },
        { bottom_right.get(), 1u, 0u },
    } )
    {
        auto half_width  = target. width() / 2;
        auto half_height = target.height() / 2;
        
        REQUIRE( half_height <= std::numeric_limits< GLint >::max() );
        REQUIRE( half_width  <= std::numeric_limits< GLint >::max() );
        
        glViewport(
            static_cast< GLint >( x_offset_factor * half_width  ),
            static_cast< GLint >( y_offset_factor * half_height ),
            static_cast< GLint >(                   half_width  ),
            static_cast< GLint >(                   half_height )
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set viewport for "
            "yavsg::debug_4up_postprocess_step::run()"s
        );
        
        glEnable( GL_SCISSOR_TEST );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't enable scissor testing for "
            "yavsg::debug_4up_postprocess_step::run()"s
        );
        
        glScissor(
            static_cast< GLint >( x_offset_factor * half_width  ),
            static_cast< GLint >( y_offset_factor * half_height ),
            static_cast< GLint >(                   half_width  ),
            static_cast< GLint >(                   half_height )
        );
        YAVSG_GL_THROW_FOR_ERRORS(
            "couldn't set scissor for yavsg::debug_4up_postprocess_step::run()"s
        );
        
        if( step )
        {
            target.bind();
            step->run( source, target );
        }
    }
    
    glDisable( GL_SCISSOR_TEST );
    YAVSG_GL_THROW_FOR_ERRORS(
        "couldn't disable scissor testing for "
        "yavsg::debug_4up_postprocess_step::run()"s
    );
}
