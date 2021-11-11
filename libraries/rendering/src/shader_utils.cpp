#include <yavsg/rendering/shader_utils.hpp>

#include <cstdlib>      // getenv
#include <stdexcept>    // runtime_error


std::string JadeMatrix::yavsg::shaders_dir()
{
    if( auto const* dir = std::getenv( "YAVSG_SHADERS_DIR" ) )
    {
        return dir;
    }
    throw std::runtime_error(
        "environment variable YAVSG_SHADERS_DIR not set"
    );
}
