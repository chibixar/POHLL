#pragma once
#include <cstddef>
#include <utility>

// Собственные алгоритмы: требуют от итератора только ++, *, !=, копирование.

template <typename It, typename Pred>
It find_if_own(It first, It last, Pred pred)
{
    for (; first != last; ++first)
        if (pred(*first))
            return first;
    return last;
}

template <typename It, typename Pred>
size_t count_if_own(It first, It last, Pred pred)
{
    size_t n = 0;
    for (; first != last; ++first)
        if (pred(*first))
            ++n;
    return n;
}

// Сортировка выбором; less(a, b) == true, если a должен стоять раньше b
template <typename It, typename Less>
void sort_own(It first, It last, Less less)
{
    for (It i = first; i != last; ++i)
    {
        It smallest = i;
        It j = i;
        ++j;
        for (; j != last; ++j)
            if (less(*j, *smallest))
                smallest = j;
        if (smallest != i)
            std::swap(*i, *smallest);
    }
}
