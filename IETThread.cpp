#include "IETThread.h"

IETThread::IETThread() {

}

IETThread::~IETThread() {

}

void IETThread::start() {
	std::thread(&IETThread::run, this).detach();
}

void IETThread::sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}