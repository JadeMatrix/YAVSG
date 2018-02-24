#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#import <OpenGL/glu.h>  // #include <GL/glu.h>

#include <iostream>
#include <sstream>

#include "vector.hpp"
#include "real.hpp"


std::ostream& operator<<( std::ostream& out, __int128 v )
{
    if( v == 0 )
        out << "0";
    else if( v == ( __int128 )0x01 << 127 )
        out << "-170141183460469231731687303715884105728";
    // else if( v == ~( ( __int128 )0x01 << 127 ) )
    //     out << "170141183460469231731687303715884105727";
    else
    {
        if( v < 0 )
        {
            out << "-";
            v *= -1;
        }
        
        std::string val_str;
        while( v > 0 )
        {
            val_str = std::to_string( ( int )( v % 10 ) ) + val_str;
            v /= 10;
        }
        
        out << val_str;
    }
    return out;
}


namespace
{
    const int SCREEN_WIDTH  = 1024;
    const int SCREEN_HEIGHT =  720;
    
    template< typename T, unsigned int D > std::string vector_string(
        const yavsg::vector< T, D >& v
    )
    {
        std::stringstream s;
        s << "<";
        for( unsigned int i = 0; i < D; ++i )
        {
            s << v[ i ];
            if( i + 1 < D )
                s << ",";
        }
        s << ">";
        return s.str();
    }
}


int main( int argc, char* argv[] )
{
    // SDL_Window*  sdl_window  = nullptr;
    // SDL_Surface* sdl_surface = nullptr;
    
    // if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    // {
    //     std::cout
    //         << "could not initialize SDL2: "
    //         << SDL_GetError()
    //         << std::endl
    //     ;
    //     return -1;
    // }
    
    // sdl_window = SDL_CreateWindow(
    //     "hello world",
    //     SDL_WINDOWPOS_UNDEFINED,
    //     SDL_WINDOWPOS_UNDEFINED,
    //     SCREEN_WIDTH,
    //     SCREEN_HEIGHT,
    //     SDL_WINDOW_SHOWN
    // );
    
    // if( !sdl_window )
    // {
    //     std::cout
    //         << "could not create SDL2 window: "
    //         << SDL_GetError()
    //         << std::endl
    //     ;
    //     return -1;
    // }
    
    // SDL_UpdateWindowSurface( sdl_window );
    
    // sdl_surface = SDL_GetWindowSurface( sdl_window );
    // SDL_FillRect(
    //     sdl_surface,
    //     NULL,
    //     SDL_MapRGB( sdl_surface -> format, 0x00, 0x00, 0x00 )
    // );
    // SDL_UpdateWindowSurface( sdl_window );
    // SDL_Delay( 2000 );
    
    // SDL_DestroyWindow( sdl_window );
    
    // SDL_Quit();
    
    
    
    
    
    
    
    
    
    
    
    // yavsg::vector< double, 3 > v1( { 3.0f, 1.2f, 2.7f } );
    // yavsg::vector< double, 3 > v2( { 0.5f, 0.2f, 1.0f } );
    
    // // yavsg::vector< yavsg::real, 3 > v3( { 92233720368547758ll, 92233720368547758ll, 92233720368547758ll } );
    // // yavsg::vector< int_fast64_t, 3 > v3( { 23455234, 6543561, 567 } );
    // yavsg::vector< __int128, 3 > v3( { 23455234ll, 6543561ll, 567ll } );
    
    // std::cout
    //     << vector_string( v1 )
    //     << " => "
    //     << vector_string( v1.unit() )
    //     << ","
    //     << v1.magnitude()
    //     << std::endl
        
    //     << vector_string( v2 )
    //     << " => "
    //     << vector_string( v2.unit() )
    //     << ","
    //     << v2.magnitude()
    //     << std::endl
        
    //     << vector_string( v3 )
    //     << " => "
    //     << vector_string( v3.unit() )
    //     << ","
    //     << v3.magnitude()
    //     << std::endl
        
    //     << vector_string( yavsg::cross_product( v1, v2 ) )
    //     << std::endl
    // ;
    
    // long long test_value_64 = /*92233720368547758ll*/ 23455234ll;
    // __int128 test_value_128 = /*92233720368547758ll*/ 23455234ll;
    
    // std::cout
    //     << "test_value_64 = "
    //     << test_value_64
    //     << std::endl
    //     << "sqrt = "
    //     << yavsg::newtons_method< long long >( test_value_64 )
    //     << std::endl
    //     << "squared = "
    //     << test_value_64 * test_value_64
    //     << std::endl
        
    //     << "test_value_128 = "
    //     << test_value_128
    //     << std::endl
    //     << "sqrt = "
    //     << yavsg::newtons_method< __int128 >( test_value_128 )
    //     << std::endl
    //     << "squared = "
    //     << test_value_128 * test_value_128
    //     << std::endl
    // ;
    
    // yavsg::vector<      double, 1 > vec1_f( { 23455234.0f } );
    // yavsg::vector<    __int128, 1 > vec1_i( { 23455234ll  } );
    // // yavsg::vector< yavsg::real, 1 > vec1_r( { 23455234ll  } );
    
    // std::cout
    //     << "vec1_f: "
    //     << vector_string( vec1_f )
    //     << " => "
    //     << vec1_f.dot( vec1_f )
    //     << " => "
    //     << vector_string( vec1_f.unit() )
    //     << ","
    //     << vec1_f.magnitude()
    //     << std::endl
        
    //     << "vec1_i: "
    //     << vector_string( vec1_i )
    //     << " => "
    //     << vec1_i.dot( vec1_i )
    //     << " => "
    //     << vector_string( vec1_i.unit() )
    //     << ","
    //     << vec1_i.magnitude()
    //     << std::endl
        
    //     // << "vec1_r: "
    //     // << vector_string( vec1_r )
    //     // << " => "
    //     // << vec1_r.dot( vec1_r )
    //     // << " => "
    //     // << vector_string( vec1_r.unit() )
    //     // << ","
    //     // << vec1_r.magnitude()
    //     // << std::endl
        
    //     << "23455234ll^2 = "
    //     << ( ( __int128 )23455234ll * ( __int128 )23455234ll )
    //     << std::endl
    // ;
    
    __int128    test_value_2_128(  23455235ll );
    yavsg::real test_value_2_real( 23455235ll );
    
    std::cout
        << "test_value_2_128 = "
        << test_value_2_128
        << ", squared = "
        << ( test_value_2_128 * test_value_2_128 )
        << ", sqrt = "
        << yavsg::sqrt( test_value_2_128 )
        << ", /2 = "
        << test_value_2_128 / 2
        << std::endl
        
        << "test_value_2_real = "
        << test_value_2_real
        << ", squared = "
        << ( test_value_2_real * test_value_2_real )
        << ", sqrt = "
        << yavsg::sqrt( test_value_2_real )
        << ", /2 = "
        << test_value_2_real / 2
        << std::endl
    ;
    
    // 550148001994756
    // 42818190560721
    // 321489
    
    // 592966192876966
    
    // 24350897.167804023232528
    
    return 0;
}
