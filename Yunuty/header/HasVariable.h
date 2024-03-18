#pragma once
#include <type_traits>

template <typename T, typename U = int>
struct Has_first : std::false_type {};
// SFINAE 를 활용한 템플릿 메타 프로그래밍
template <typename T>
struct Has_first<T, decltype((void)T::first, 0)> : std::true_type {};
