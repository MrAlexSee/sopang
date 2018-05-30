#ifndef REPEAT_HPP
#define REPEAT_HPP

#include <functional>

void repeat(int count, std::function<void()> fun)
{
    for (int i = 0; i < count; ++i)
    {
        fun();
    }
}

#endif // REPEAT_HPP
