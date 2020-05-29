#ifndef __LOCKFREE_THREADPOOL__
#define __LOCKFREE_THREADPOOL__

#include <thread>
#include <vector>
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <atomic>
#include <type_traits>
#include <future>

#include "func_wrap.hpp"

class thread_pool {
public:
	size_t amount;
	std::atomic<bool> status{false};
	std::vector<std::thread> threadz;
	boost::lockfree::queue<function_wrapper*> jobs{0};


	thread_pool(size_t amount) : amount(amount) {
		status.store(true, std::memory_order_relaxed);
		for (int i = 0; i < amount; ++i) {
			threadz.emplace_back(std::bind(&thread_pool::entry, this, i));
		}

	}

	void entry(int id) {
		function_wrapper* task;
		while (status.load(std::memory_order_relaxed)) {
			if (jobs.pop(task)) {
				(*task)();
				delete task;
			}
			else {
				std::this_thread::yield();
			}
		}
	}

	template<typename Fn, typename... Args>
	decltype(auto) push(Fn&& fn, Args&&... args) {
		typedef typename std::result_of<Fn(Args...)>::type rettype;
		std::packaged_task<rettype()> task(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
		std::future<rettype> future = task.get_future();
		auto ptask = new function_wrapper(std::move(task));
		while (!jobs.push(ptask));
		return future;
	}

	~thread_pool() {
		status.store(false, std::memory_order_relaxed);

		for (auto&& thread : threadz) {
			thread.join();
		}

		while(!jobs.empty()) {
			function_wrapper* ptr;
			while(!jobs.pop(ptr));
			if (ptr) delete ptr;
		}
	}
};

#endif