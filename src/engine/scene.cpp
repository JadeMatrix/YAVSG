#include "../../include/engine/scene.hpp"


namespace yavsg
{
    scene::scene() :
        main_camera(
            { 2.2f, 0.0f, 2.2f },
            0.1f,
            1.0f,
            10.0f
        )
    {}
}