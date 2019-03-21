#include "..\Include\Semaphore.h"

namespace K9 {

	Semaphore::Semaphore(int value):
		_value(value),
		_wakeUps(0){
	}

	Semaphore::~Semaphore() {

	}

	void Semaphore::signal() {
		_mutex.lock();
		_value++;
		if (_value <= 0) {
			_wakeUps++;
			_cond.notify_one();
		}
		_mutex.unlock();
	}

	void Semaphore::wait() {
		_mutex.lock();
		_value--;
		if (_value < 0) {
			do {
				_cond.wait(_mutex);
			} while (_wakeUps < 1);

			_wakeUps--;
		}
		_mutex.unlock();
	}
}