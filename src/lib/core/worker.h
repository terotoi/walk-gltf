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
#ifndef _CX_CORE_WORKER_H
#define _CX_CORE_WORKER_H

#include "shared_queue.h"
#include "task.h"

#include <iostream>
#include <thread>

namespace cst {

using namespace std::chrono_literals;

enum WorkerState {
  WORKER_STATE_NOT_STARTED,
  WORKER_STATE_STARTING,
  WORKER_STATE_PROCESSING,
  WORKER_STATE_FINISHING,
  WORKER_STATE_JOINED
};

/**
 * Worker starts a thread and processes all tasks that are available
 * in its task queue.
 */
class Worker {
public:
  /**
   * Creates a new worker.
   *
   * @param name Name of the worker.
   */
  Worker(std::string const &name, bool gated)
      : name(name), gated(gated), timeout(250ms) {}

  /// Destroys the worker
  ~Worker() { join(); }

  /// Returns the name of the worker
  std::string const &getName() const { return name; }

  bool isGated() const { return gated; }

  /// Returns the ID of the thread used by the worker
  std::thread::id getThreadId() const { return threadId; }

  /// Set a new timeout
  void setTimeout(std::chrono::milliseconds const ms) { timeout = ms; }

  /// Purge all pending tasks of the worker. The task currently in progress will
  /// be finished normally.
  void purge() {
    std::cout << prefix() << ": purge locking user\n";
    std::scoped_lock lock(user_mux);
    tasks.purge();
  }

  WorkerState getState() const { return state; }

  /// Joins the worker thread.
  void join() {
    if (state == WORKER_STATE_JOINED)
      return;

    //std::cout << prefix() << ": joining\n";

    if (thread.joinable())
      thread.join();
    setState(WORKER_STATE_JOINED);
  }

  /// Waits for the worker to finish.
  void wait() {
    if (state == WORKER_STATE_JOINED)
      return;

    while (state != WORKER_STATE_JOINED && state != WORKER_STATE_FINISHING) {
      {
        std::unique_lock lock(state_mux);
        stateCV.wait(lock);
      }

      if (state == WORKER_STATE_FINISHING && thread.joinable()) {
        thread.join();
        state = WORKER_STATE_JOINED;
      }
    }
  }

  /// Adds a task to the queue of the worker. Starts a new thread if the worker
  /// is not running.
  void add(taskptr task, size_t maxQueueLen) {
    if (maxQueueLen > 0) {
      std::scoped_lock lock(user_mux);
      if (tasks.size() >= maxQueueLen) {
        tasks.purge();
        throw std::runtime_error(prefix() + ": queue is full");
      }
    }

    std::scoped_lock lock(user_mux);

    tasks.push(task);

    bool start_new = false;

    if (state == WORKER_STATE_FINISHING) {
      join();
      start_new = true;
    } else if (state == WORKER_STATE_NOT_STARTED ||
               state == WORKER_STATE_JOINED)
      start_new = true;

    if (start_new) {
      //std::cout << prefix() << ": thread not started, starting a new thread\n";
      state = WORKER_STATE_STARTING;
      thread = std::thread(&Worker::run, this);
    }
  }

private:
  /// Returns a debug print prefix.
  std::string prefix() const { return "[" + name + "]"; }

  void setState(WorkerState state) {
    std::scoped_lock lock(state_mux);
    if (state != this->state) {
      this->state = state;
      stateCV.notify_all();
    }
  }

  // Runs the worker thread.
  void run() {
    threadId = std::this_thread::get_id();
    setState(WORKER_STATE_PROCESSING);
    bool running = true;

    while (running) {
      taskptr task = tasks.popTimeout(timeout);

      if (task != nullptr) {
        task->run();
      } else {
        running = !tasks.empty();
      }
    }

    setState(WORKER_STATE_FINISHING);

    if (!tasks.empty())
      std::cerr << prefix() << ": tasks was not empty on exit.\n";
  }

  std::string const name;
  bool gated;
  SharedQueue<taskptr> tasks;
  mutable std::mutex user_mux, int_mux, state_mux;
  std::thread thread;
  std::thread::id threadId;
  std::chrono::milliseconds timeout;
  volatile WorkerState state = WORKER_STATE_NOT_STARTED;
  std::condition_variable stateCV;

  friend std::string to_string(Worker *worker);
};

template <typename T> std::shared_ptr<Worker> worker_ptr;

inline std::string to_string(Worker *worker) { return worker->prefix(); }

} // namespace cst

#endif // _CST_LIB_CORE_WORKER_H
