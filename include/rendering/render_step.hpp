#pragma once
#ifndef YAVSG_RENDERING_RENDER_STEP_HPP
#define YAVSG_RENDERING_RENDER_STEP_HPP


#include "../gl/framebuffer.hpp"

#include <utility>  // std::size_t


namespace yavsg
{
    class scene_render_step
    {
    public:
        virtual ~scene_render_step() {};
        virtual void run() = 0;
    };
    
    template< std::size_t ColorTargets > class postprocess_render_step
    {
    public:
        virtual ~postprocess_render_step() {};
        virtual void run( gl::framebuffer< ColorTargets >& source ) = 0;
    };
}


#endif
