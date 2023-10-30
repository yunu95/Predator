/// 2023. 10. 23 김상준
/// STL 의 Queue 자료구조를 응용하여 CircularQueue 를 구현함
/// 꽉 차있을 때, 아무 동작 하지 않는 것이 아니라
/// 끝에 하나를 넣고 앞의 하나를 없애는 방식으로 동작하도록 함

#pragma once

#include <vector>
#include <cassert>

template <typename T, size_t N = 30>
class CircularQueue
{
public:
	CircularQueue(size_t capacity = N)
	{
		container.resize(capacity);
	}

	bool Empty() const noexcept
	{
		return size == 0;
	}

	bool Full() const noexcept
	{
		return size == N;
	}

	size_t Size() const noexcept
	{
		return size;
	}

	T& Front() const noexcept
	{
		assert(size != 0 && "The Container is Empty...");
		return const_cast<T&>(container[front]);
	}

	T& Back() const noexcept
	{
		assert(size != 0 && "The Container is Empty...");
		return const_cast<T&>(container[(rear - 1 + N) % N]);
	}

	void Push(const T& val)
	{
		if (size == N)
		{
			Pop();
		}

		container[rear] = val;
		size++;
		rear++;
		rear %= N;
	}

	void Push(T&& val)
	{
		if (size == N)
		{
			Pop();
		}

		container[rear] = val;
		size++;
		rear++;
		rear %= N;
	}

	template <typename... Types>
	void Emplace(Types&&... vals)
	{
		if (size == N)
		{
			Pop();
		}

		container[rear] = T(vals...);
		size++;
		rear++;
		rear %= N;
	}

	void Pop() noexcept
	{
		if (size == 0)
		{
			return;
		}

		front++;
		front %= N;
		size--;
	}

	void Swap(CircularQueue& swap_container) noexcept
	{
		container.swap(swap_container.container);
		size_t temp_f = container.front;
		size_t temp_r = container.rear;
		size_t temp_s = container.size;

		container.front = swap_container.front;
		swap_container.front = temp_f;
		container.rear = swap_container.rear;
		swap_container.rear = temp_r;
		container.size = swap_container.size;
		swap_container.size = temp_s;
	}

private:
	size_t front = 0;
	size_t rear = 0;
	size_t size = 0;
	std::vector<T> container;
};

