#pragma once


#include "scene.hpp"

#include <yavsg/gl/framebuffer.hpp>

#include <cstddef>  // size_t


namespace JadeMatrix::yavsg
{
    class render_step
    {
    public:
        virtual ~render_step() = default;
        virtual void run( scene const&, gl::write_only_framebuffer& ) = 0;
    };
    
    template< class... ColorTargetTypes > class postprocess_step
    {
    public:
        virtual ~postprocess_step() = default;
        virtual void run(
            gl::framebuffer< ColorTargetTypes... > const& source,
            gl::write_only_framebuffer                  & target
        ) = 0;
    };
}
