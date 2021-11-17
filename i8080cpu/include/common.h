#pragma once

template<typename P, typename V>
P* padd(const P* pointer, V offset)
{
    return reinterpret_cast<P*>(reinterpret_cast<size_t>(pointer) + offset);
}