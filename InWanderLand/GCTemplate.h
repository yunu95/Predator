/// 2024. 03. 12 김상준
/// 템플릿 특수화 처리를 위한 Global Constant Template Data 구조체 작성

#pragma once

template <typename T>
struct GC
{
	GC() = default;
	GC(const GC& input)
	{
		this->data = input.data;
	}
	GC& operator=(const GC& input)
	{
		this->data = input.data;
		return *this;
	}
	GC(const T& input)
	{
		this->data = input;
	}
	GC& operator=(const T& input)
	{
		this->data = input;
		return *this;
	}
	operator T() const
	{
		return data;
	}

	T data = T();
};

template <class>
constexpr bool is_GC_v = false;

template <class T>
constexpr bool is_GC_v<GC<T>> = true;

template <class T>
constexpr bool is_GC_v<const GC<T>> = true;
