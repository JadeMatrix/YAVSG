#pragma once
#ifndef YAVSG_RENDERING_RENDER_STEP_HPP
#define YAVSG_RENDERING_RENDER_STEP_HPP


#include "../gl/framebuffer.hpp"

#include <utility>  // std::size_t


namespace yavsg
{
    class render_step
    {
    public:
        virtual ~render_step() {}
        virtual void run( gl::write_only_framebuffer& target ) = 0;
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
