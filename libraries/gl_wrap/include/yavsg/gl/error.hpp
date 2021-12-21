#pragma once


#include <stdexcept>    // runtime_error


namespace JadeMatrix::yavsg::gl
{
    class error : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };
}
