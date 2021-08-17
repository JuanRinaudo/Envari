#ifndef TEMPLATES_H
#define TEMPLATES_H

template<class T, std::size_t n>
constexpr std::size_t ArraySize(T (&)[n])
{
    return n;
}

#endif