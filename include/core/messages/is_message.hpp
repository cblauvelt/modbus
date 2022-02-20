#pragma once

#include <type_traits>

#include "types.hpp"

namespace modbus {

namespace detail {

// See http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf.
template <typename...> using void_t = void;

// Primary template handles all types not supporting the operation.
template <class T, template <typename> class, typename = void_t<>>
struct detect : std::false_type {};

// Specialization recognizes/validates only types supporting the archetype.
template <class T, template <typename> class Op>
struct detect<T, Op, void_t<Op<T>>> : std::true_type {};

template <typename T> using member_size_t = decltype(std::declval<T>().size());

template <typename T>
using member_serialize_t =
    decltype(std::declval<T>().serialize(std::declval<buffer_iterator>()));

template <typename T> using has_size = detect<T, member_size_t>;

template <typename T> using has_serialize = detect<T, member_serialize_t>;

template <typename T>
using is_message =
    std::integral_constant<bool, has_size<T>::value && has_serialize<T>::value>;

} // namespace detail
} // namespace modbus