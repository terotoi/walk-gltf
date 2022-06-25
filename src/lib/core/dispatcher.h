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
#ifndef _CX_CORE_DISPATCHER_H
#define _CX_CORE_DISPATCHER_H

#include "worker.h"

#include <iostream>
#include <map>

namespace cst {

static constexpr auto DEFAULT_WORKERS = 15;
static constexpr auto DEFAULT_MAX_QUEUE_LEN = 20;

/**
 * Dispatcher dispathes tasks to available workes and starts
 * new workers when needed and possible.
 */
class Dispatcher {
public:
  Dispatcher() { workers.resize(DEFAULT_WORKERS, nullptr); }

  ~Dispatcher() { waitAll(); }

  /**
   * Adds a task with no parameters.
   */
  void add(std::function<void()> f, std::string const &name = "") {
    add(std::make_shared<VoidTask>(f, name));
  }

  /// Wait and join all workers.
  void waitAll() {
    bool waiting = true;
    while (waiting) {
      //std::cout << "{dispatcher} Waiting for workers to finish..." << std::endl;
      bool waited = false;
      for (size_t i = 0; i < workers.size(); ++i) {
        if (workers[i] != nullptr) {
          workers[i]->wait();
          delete workers[i];
          workers[i] = nullptr;
          waited = true;
        }
      }
      waiting = waited;
    }

    workers.clear();
  }

  /**
   * Adds a task to a worker. May start a new worker.
   * @param task The task to add.
   */
  void add(taskptr task) {
    std::scoped_lock lock(mux);

    Worker *worker;
    size_t slot;
    if (findEmptySlot(slot, 0)) {
      worker = new Worker("Worker " + std::to_string(num_workers++), false);
      workers[slot] = worker;
    } else {
      worker = findWorkerNoGate();
      if (worker == nullptr)
        throw std::runtime_error("No worker available");
    }

    //std::cout << "{dispatcher}: adding task " << to_string(task)
    //          << " to worker " << to_string(worker) << "\n";
    worker->add(task, DEFAULT_MAX_QUEUE_LEN);
  }

private:
  bool findEmptySlot(size_t &slot, size_t start=0) {
    for (size_t i = start; i < workers.size(); i++) {
      if (workers[i] == nullptr) {
        slot = i;
        return true;
      }
    }
    return false;
  }

  Worker *findWorkerNoGate() {
    std::thread::id my_id = std::this_thread::get_id();

    for (size_t i = 0; i < workers.size(); i++) {
      Worker *w = workers[i];
      if (w != nullptr && (w->getThreadId() != my_id) && !w->isGated())
        return w;
    }
    return nullptr;
  }

  std::mutex mux;
  std::vector<Worker *> workers;

  size_t num_workers = 0;
};

} // namespace cst

#endif // _CX_CORE_DISPATCHER_H
