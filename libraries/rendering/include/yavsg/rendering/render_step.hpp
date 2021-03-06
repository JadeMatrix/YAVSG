#pragma once
#ifndef YAVSG_RENDERING_RENDER_STEP_HPP
#define YAVSG_RENDERING_RENDER_STEP_HPP


#include "scene.hpp"

#include <yavsg/gl/framebuffer.hpp>

#include <utility>  // std::size_t


namespace yavsg
{
    class render_step
    {
    public:
        virtual ~render_step() {}
        virtual void run(
            const scene&,
            gl::write_only_framebuffer&
        ) = 0;
    };
    
    template< class... ColorTargetTypes > class postprocess_step
    {
    public:
        virtual ~postprocess_step() {}
        virtual void run(
            const gl::framebuffer< ColorTargetTypes... >& source,
                  gl::write_only_framebuffer            & target
        ) = 0;
    };
}


#endif
