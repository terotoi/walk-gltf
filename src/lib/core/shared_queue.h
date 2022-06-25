/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#ifndef _CST_LIB_CORE_SHARED_QUEUE_H
#define _CST_LIB_CORE_SHARED_QUEUE_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace cst {
/**
 * SharedQueue is a thread-safe queue.
 */
template <typename T> class SharedQueue {
public:
  SharedQueue() {}
  ~SharedQueue() {}

  /**
   * Returns true if the queue is empty.
   */
  bool empty() const { return objs.empty(); }

  /**
   * Pushes an object to the queue and signals all the waiters.
   */
  void push(T obj) {
    std::lock_guard lock(mux);
    objs.push(obj);
    cv.notify_all();
  }

  /**
   * If the queue is empty, waits for the condition variable and then pops
   * and returns an object. If the queue is not empty, pops an object
   * immediately.
   */
  T pop() {
    std::unique_lock lock(mux);
    if (objs.empty())
      cv.wait(lock);
    T obj = objs.front();
    objs.pop();
    return obj;
  }

  /**
   * If the queue is empty, waits for the condition variable and then pops
   * and returns an object. If the queue is not empty, pops an object
   * immediately. This function also exits if a given timeout is reached. In
   * this case a nullptr is returned.
   */
  T popTimeout(auto timeout) {
    using namespace std::literals::chrono_literals;

    std::unique_lock lock(mux);
    if (objs.empty())
      cv.wait_for(lock, timeout);

    if (objs.empty())
      return nullptr;

    T obj = objs.front();
    objs.pop();
    return obj;
  }

  /**
   * Purgers all tasks from the queue. Does not issue a signal.
   * Does nothing if the queue is empty.
   */
  void purge() {
    std::unique_lock lock(mux);
    std::queue<T> empty;
    std::swap(objs, empty);
  }

  /**
   * Signals all waiters.
   */
  void signal() { cv.notify_all(); }

  /**
   * Returns the size of the queue.
   */
  size_t size() {
    std::scoped_lock lock(mux);
    return objs.size();
  }

private:
  std::queue<T> objs;
  std::mutex mux;
  std::condition_variable cv;
};

} // namespace cst

#endif // _CST_LIB_CORE_SHARED_QUEUE_H
