/// 2024. 04. 05 김상준
/// C++ Coroutine 을 작성하기 위한 promise_type 객체를 내부에 보유한 객체입니다.
/// 내부에 std::coroutine_handle 을 보유하고 있어 Coroutine 에 대한
/// 조작이 가능합니다.

#pragma once

#include <coroutine>
#include <memory>

namespace application
{
	template <typename T>
	class CoroutineObject
	{
	public:
		struct promise_type_impl;
		using coro_Handle = std::coroutine_handle<promise_type_impl>;

		class Coroutine_Handler;

		struct promise_base
		{
			auto initial_suspend()
			{
				return std::suspend_always();
			}

			auto final_suspend() noexcept
			{
				return std::suspend_always();
			}

			void unhandled_exception()
			{
				throw std::exception("Coroutine Unhandled Exception.");
			}
		};

		struct promise_type_impl
			: public promise_base
		{
			promise_type_impl() = default;

			promise_type_impl(const T& value)
				: value(value)
			{

			}

			CoroutineObject get_return_object()
			{
				return CoroutineObject(std::shared_ptr<Coroutine_Handler>(coro_Handle::from_promise(*this)));
			}

			auto yield_value(const T& value)
			{
				this->value = value;
				return std::suspend_always();
			}

			void return_value(const T& value)
			{
				this->value = value;
				return;
			}

			T value;
		};

		CoroutineObject()
			: handle()
		{

		}

		CoroutineObject(const std::shared_ptr<Coroutine_Handler>& handle)
			: handle(handle)
		{

		}

		CoroutineObject(const CoroutineObject& obj)
			: handle(obj.handle)
		{

		}

		struct Iterator
		{
			Iterator(CoroutineObject* coroutine)
				: coroutine(coroutine)
			{

			}

			const T& operator*() const
			{
				return coroutine->handle->primitive_handle.promise().value;
			}

			Iterator& operator++()
			{
				(*coroutine)();
				isDone = coroutine->Done();
				return *this;
			}

			bool operator==(std::default_sentinel_t) const
			{
				if (isDone == true && coroutine->Done())
				{
					return true;
				}

				return false;
			}

		private:
			CoroutineObject* coroutine = nullptr;
			bool isDone = false;
		};

		Iterator begin()
		{
			if (!handle)
			{
				return Iterator(nullptr);
			}

			return Iterator(this);
		}

		std::default_sentinel_t end()
		{
			return std::default_sentinel_t();
		}

		bool Done()
		{
			return handle->primitive_handle.done();
		}

		void operator()()
		{
			handle->primitive_handle();
		}

		class Coroutine_Handler
		{
		public:
			Coroutine_Handler(const coro_Handle& handle)
				: primitive_handle(handle)
			{

			}

			~Coroutine_Handler()
			{
				if (primitive_handle)
				{
					primitive_handle.destroy();
				}
			}

			coro_Handle primitive_handle;
		};

	private:
		std::shared_ptr<Coroutine_Handler> handle;
	};

	template <>
	class CoroutineObject<void>
	{
	public:
		struct promise_type_impl;
		using coro_Handle = std::coroutine_handle<promise_type_impl>;

		class Coroutine_Handler;

		struct promise_base
		{
			auto initial_suspend()
			{
				return std::suspend_always();
			}

			auto final_suspend() noexcept
			{
				return std::suspend_always();
			}

			void unhandled_exception()
			{
				throw std::exception("Coroutine Unhandled Exception.");
			}
		};

		struct promise_type_impl
			: public promise_base
		{
			CoroutineObject get_return_object()
			{
				return CoroutineObject(std::make_shared<Coroutine_Handler>(coro_Handle::from_promise(*this)));
			}

			void return_void()
			{
				return;
			}
		};

		CoroutineObject()
			: handle(nullptr)
		{

		}

		CoroutineObject(const std::shared_ptr<Coroutine_Handler>& handle)
			: handle(handle)
		{

		}

		CoroutineObject(const CoroutineObject& obj)
			: handle(obj.handle)
		{

		}

		struct Iterator
		{
			Iterator(CoroutineObject* coroutine)
				: coroutine(coroutine)
			{

			}

			/// Range-based for 에 사용하기 위하여
			/// 임의로 false 반환하도록 함
			const bool operator*() const
			{
				return false;
			}

			Iterator& operator++()
			{
				(*coroutine)();
				isDone = coroutine->Done();
				return *this;
			}

			bool operator==(std::default_sentinel_t) const
			{
				if (isDone == true && coroutine->Done())
				{
					return true;
				}

				return false;
			}

		private:
			CoroutineObject* coroutine = nullptr;
			bool isDone = false;
		};

		Iterator begin()
		{
			if (!handle)
			{
				return Iterator(nullptr);
			}

			return Iterator(this);
		}

		std::default_sentinel_t end()
		{
			return std::default_sentinel_t();
		}

		bool Done()
		{
			return handle->primitive_handle.done();
		}

		void operator()()
		{
			handle->primitive_handle();
		}

		class Coroutine_Handler
		{
		public:
			Coroutine_Handler(const coro_Handle& handle)
				: primitive_handle(handle)
			{

			}

			~Coroutine_Handler()
			{
				if (primitive_handle)
				{
					primitive_handle.destroy();
				}
			}

			coro_Handle primitive_handle;
		};

	private:
		std::shared_ptr<Coroutine_Handler> handle;
	};
}

namespace std
{
	template <typename T, typename... Args>
	struct coroutine_traits<application::CoroutineObject<T>, Args...>
	{
		using promise_type = application::CoroutineObject<T>::promise_type_impl;
	};
}
