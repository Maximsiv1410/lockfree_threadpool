#include <iostream>
#include <future>
#include <boost/lockfree/queue.hpp>


#include "thread_pool.hpp"

int plus(int num) {
	return num + 1;
}

int main() {
	thread_pool pool(4);
	std::vector<std::future<int>> futures;

	for (int i = 0; i < 20; ++i) {
		futures.push_back(pool.push(plus, i*i));
	}

	for (auto&& ft : futures) {
		std::cout << ft.get() << "\n";
	}
	

}

