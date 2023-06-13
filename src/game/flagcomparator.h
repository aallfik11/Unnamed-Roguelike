#ifndef FLAGCOMPARATOR_H
#define FLAGCOMPARATOR_H
#include <type_traits>

/*
 * @brief
 * Will not compile if used with Non-enum types, if the enumerated types are not
 * bitflags, the behavior is undefined. If used with a max-sized unsigned
 * integer it might not work depending on the amount of flags, in that case,
 * using the u_ version is recommended
 *
 * @return true if the target contains any of the flags, false otherwise
 */
template <class T>
constexpr bool compareFlags(T target, std::initializer_list<T> flags)
{
    static_assert(std::is_enum<T>());
    auto     target_cast = static_cast<intmax_t>(target);
    intmax_t flags_cast{};
    for (auto flag : flags)
    {
        flags_cast |= static_cast<intmax_t>(flag);
    }
    return ((target_cast & flags_cast) != 0);
}

/*
 * @brief
 * Will not compile if used with Non-enum types, if the enumerated types are not
 * bitflags, the behavior is undefined.
 *
 * @return true if the target contains any of the flags, false otherwise
 */
template <class T>
constexpr bool u_compareFlags(T target, std::initializer_list<T> flags)
{
    static_assert(std::is_enum<T>());
    auto     target_cast = static_cast<uintmax_t>(target);
    uintmax_t flags_cast{};
    for (auto flag : flags)
    {
        flags_cast |= static_cast<uintmax_t>(flag);
    }
    return ((target_cast & flags_cast) != 0);
}

#endif /*FLAGCOMPARATOR_H*/
