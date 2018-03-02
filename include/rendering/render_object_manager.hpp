#pragma once
#ifndef YAVSG_GL_RENDER_OBJECT_MANAGER_HPP
#define YAVSG_GL_RENDER_OBJECT_MANAGER_HPP


#include "../gl/_gl_base.hpp"
#include "../gl/attribute_buffer.hpp"

#include <mutex>
#include <vector>


namespace yavsg
{
    // TODO: More abstract for different implementations? Or just make more
    // efficient?
    template< class AttributeBuffer, class Material > class render_object_manager
    {
    public:
        
        struct render_group
        {
            Material material;
            gl::index_buffer indices;
            
            render_group() = default;
            render_group( const render_group& o ) = delete;
            
            render_group(
                Material        && m,
                gl::index_buffer&& i
            ) :
                material( std::move( m ) ),
                indices(  std::move( i ) )
            {}
            
            render_group( render_group&& o ) :
                material( std::move( o.material ) ),
                indices(  std::move( o.indices  ) )
            {}
        };
        
        struct render_object
        {
            std::vector< render_group > render_groups;
            AttributeBuffer vertices;
            
            render_object() = default;
            render_object( const render_object& o ) = delete;
            
            render_object(
                std::vector< render_group >&& rg,
                AttributeBuffer            && v
            ) :
                render_groups( std::move( rg ) ),
                vertices(      std::move( v  ) )
            {}
            
            render_object( render_object&& o ) :
                render_groups( std::move( o.render_groups ) ),
                vertices(      std::move( o.vertices      ) )
            {}
        };
        
        using object_list = std::vector< render_object >;
        
    protected:
        // Possibly different in the future
        std::mutex   read_mutex;
        std::mutex& write_mutex = read_mutex;
        
        std::vector< render_object > objects;
        
    public:
        class read_reference
        {
            friend class render_object_manager;
            
        protected:
            render_object_manager& parent;
            bool should_unlock = false;
            
            read_reference( render_object_manager& parent ) :
                parent( parent ),
                should_unlock( true )
            {
                parent.read_mutex.lock();
            }
            
        public:
            
            read_reference( const read_reference& ) = delete;
            read_reference( read_reference&& o ) : parent( o.parent )
            {
                std::swap( should_unlock, o.should_unlock );
            }
            
            ~read_reference()
            {
                if( should_unlock )
                    parent.read_mutex.unlock();
            }
            
            const object_list& operator *()
            {
                return parent.objects;
            }
            const object_list* operator ->()
            {
                return &parent.objects;
            }
        };
        
        class write_reference
        {
            friend class render_object_manager;
            
        protected:
            render_object_manager& parent;
            bool should_unlock = false;
            
            write_reference( render_object_manager& parent ) :
                parent( parent ),
                should_unlock( true )
            {
                parent.write_mutex.lock();
            }
            
        public:
            
            write_reference( const write_reference& ) = delete;
            write_reference( write_reference&& o ) : parent( o.parent )
            {
                std::swap( should_unlock, o.should_unlock );
            }
            
            ~write_reference()
            {
                if( should_unlock )
                    parent.write_mutex.unlock();
            }
            
            object_list& operator *()
            {
                return parent.objects;
            }
            object_list* operator ->()
            {
                return &parent.objects;
            }
        };
        
        read_reference read()
        {
            return read_reference( *this );
        }
        
        write_reference write()
        {
            return write_reference( *this );
        }
    };
}


#endif
