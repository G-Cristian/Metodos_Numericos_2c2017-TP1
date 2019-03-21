#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <condition_variable>
#include <mutex>

namespace K9 {
	class Semaphore {
	public:
		Semaphore(int value);
		~Semaphore();

		void signal();
		void wait();
	private:
		int _value;
		int _wakeUps;
		std::mutex _mutex;
		std::condition_variable_any _cond;
	};
}

#endif // !_SEMAPHORE_H_
