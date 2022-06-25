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
#ifndef _CX_CORE_GATED_DISPATCHER_H
#define _CX_CORE_GATED_DISPATCHER_H

#include "worker.h"

#include <iostream>
#include <map>

namespace cst {

static constexpr auto DEFAULT_GATED_WORKERS = 10;
static constexpr auto DEFAULT_GATED_MAX_QUEUE_LEN = 20;

/**
 * GatedDispatcher dispathes tasks to available workes and starts
 * new workers when needed and possible. All works are bound
 * to a gate object. Only one worker is responsible for one gate.
 */
template <typename T> class GatedDispatcher {
public:
  GatedDispatcher() { workers.resize(DEFAULT_GATED_WORKERS, nullptr); }
  ~GatedDispatcher() { waitAll(); }

  /**
   * Adds a gated task with no parameters.
   */
  void add(std::function<void()> f, T gate, std::string const &name = "") {
    add(std::make_shared<VoidTask>(f, name), gate);
  }

  /// Wait for the worker assigned to the given gate to finish.
  void wait(T gate, bool remove = false) {
    Worker *w = nullptr;

    {
      std::scoped_lock lock(mux);
      auto it = gates.find(gate);
      if (it != gates.end()) {
        w = it->second;
      }
    }

    if (w != nullptr) {
      w->wait();

      if (remove) {
        std::scoped_lock lock(mux);
        gates.erase(gate);
      }
    }
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
    gates.clear();
  }

  /**
   * Adds a gated task to a worker. May start a new worker.
   * @param task The task to add.
   * @param gate The gate object.
   */
  void add(taskptr task, T gate) {
    std::lock_guard lock(mux);

    Worker *worker = nullptr;

    if (gates.find(gate) != gates.end()) {
      worker = gates[gate];
      //if (worker != nullptr)
      //  std::cout << "{dispatcher}: found worker for gate " << to_string(gate)
      //            << ": " << to_string(worker) << "\n";
    }

    if (worker == nullptr) {
      size_t slot;
      if (!findEmptySlot(slot))
        throw std::runtime_error("{dispatcher}: no slot found for gated task " + task->getName());

      worker = new Worker("Worker " + std::to_string(num_workers++), true);
      workers[slot] = worker;
      gates[gate] = worker;
    }

    //std::cout << "{dispatcher}: adding task " << to_string(task)
    //          << " to worker " << to_string(worker) << "\n";
    worker->add(task, DEFAULT_GATED_MAX_QUEUE_LEN);
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

  std::mutex mux;
  std::vector<Worker *> workers;
  std::map<T, Worker *> gates;
  size_t num_workers = 0;
};

} // namespace cst

#endif // _CX_CORE_GATED_DISPATCHER_H
