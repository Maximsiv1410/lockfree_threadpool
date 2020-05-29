#ifndef __MY_FUNC_WRAPPER__
#define __MY_FUNC_WRAPPER__

#include <utility>
#include <memory>
class function_wrapper {
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() {}
	};

	template<typename F>
	struct impl_type : impl_base {
		F f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call() { f(); }
	};

	std::unique_ptr<impl_base> impl;

public:

	function_wrapper() {}

	template<typename F>
	function_wrapper(F&& f) :
		impl(new impl_type<F>(std::move(f))) {
	}

	void operator()() {
		impl->call();
	}

	void call() { impl->call(); }

	function_wrapper(function_wrapper&& other) :
		impl(std::move(other.impl)) {
	}

	/*function_wrapper(function_wrapper* other) :impl(std::move(other->impl)) {

	}*/

	function_wrapper& operator=(function_wrapper&& other) {
		impl = std::move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;

	function_wrapper& operator=(const function_wrapper&) = delete;

	
};

#endif