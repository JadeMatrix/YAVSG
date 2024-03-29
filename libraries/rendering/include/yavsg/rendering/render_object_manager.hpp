#pragma once


#include <yavsg/gl_wrap.hpp>
#include <yavsg/gl/attribute_buffer.hpp>
#include <yavsg/math/basic_transforms.hpp>
#include <yavsg/math/matrix.hpp>
#include <yavsg/math/quaternion.hpp>
#include <yavsg/math/vector.hpp>

#include <mutex>
#include <vector>


namespace JadeMatrix::yavsg
{
    // TODO: More abstract for different implementations? Or just make more
    // efficient?
    template<
        typename AttributeBuffer,
        typename Material
    > class render_object_manager
    {
    public:
        struct render_group
        {
            Material material;
            gl::index_buffer indices;
            
            render_group() = default;
            render_group( render_group const& o ) = delete;
            
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
            
            // TODO: Subclassing for customizing these
            // Basic transforms; applied scale then rotation then position
            vector< GLfloat, 3 > position;
            vector< GLfloat, 3 > scale;
            versor< GLfloat > rotation;
            
            // render_object() = default;
            render_object( render_object const& o ) = delete;
            
            render_object(
                std::vector< render_group >&& rg,
                AttributeBuffer            && v,
                vector< GLfloat, 3 >   const& p,
                vector< GLfloat, 3 >   const& s,
                versor< GLfloat >      const& r
            ) :
                render_groups( std::move( rg ) ),
                vertices(      std::move( v  ) ),
                position( p ),
                scale(    s ),
                rotation( r )
            {}
            
            render_object( render_object&& o ) :
                render_groups( std::move( o.render_groups ) ),
                vertices(      std::move( o.vertices      ) ),
                position(      std::move( o.position      ) ),
                scale(         std::move( o.scale         ) ),
                rotation(      std::move( o.rotation      ) )
            {}
            
            square_matrix< GLfloat, 4 > transform_model() const
            {
                return (
                      yavsg::translation< GLfloat >( position )
                    * yavsg::    scaling< GLfloat >( scale    )
                    * yavsg::   rotation< GLfloat >( rotation )
                );
            }
        };
        
        using object_list = std::vector< render_object >;
        
        class read_reference
        {
            friend class render_object_manager;
            
        public:
            read_reference( read_reference const& ) = delete;
            read_reference( read_reference&& o ) : parent( o.parent )
            {
                std::swap( should_unlock, o.should_unlock );
            }
            
            ~read_reference()
            {
                if( should_unlock )
                {
                    parent.read_mutex.unlock();
                }
            }
            
            object_list const& operator *()
            {
                return parent.objects;
            }
            object_list const* operator ->()
            {
                return &parent.objects;
            }
            
        protected:
            render_object_manager& parent;
            bool should_unlock = false;
            
            read_reference( render_object_manager& parent ) :
                parent( parent ),
                should_unlock( true )
            {
                parent.read_mutex.lock();
            }
        };
        
        class write_reference
        {
            friend class render_object_manager;
            
        public:
            
            write_reference( write_reference const& ) = delete;
            write_reference( write_reference&& o ) : parent( o.parent )
            {
                std::swap( should_unlock, o.should_unlock );
            }
            
            ~write_reference()
            {
                if( should_unlock )
                {
                    parent.write_mutex.unlock();
                }
            }
            
            object_list& operator *()
            {
                return parent.objects;
            }
            object_list* operator ->()
            {
                return &parent.objects;
            }
            
        protected:
            render_object_manager& parent;
            bool should_unlock = false;
            
            write_reference( render_object_manager& parent ) :
                parent( parent ),
                should_unlock( true )
            {
                parent.write_mutex.lock();
            }
        };
        
        read_reference read() const
        {
            return read_reference{ const_cast< render_object_manager<
                AttributeBuffer,
                Material
            >& >( *this ) };
        }
        
        write_reference write()
        {
            return write_reference( *this );
        }
        
    protected:
        // Possibly different in the future
        std::mutex mutable read_mutex;
        std::mutex       & write_mutex = read_mutex;
        
        std::vector< render_object > objects;
    };
}
