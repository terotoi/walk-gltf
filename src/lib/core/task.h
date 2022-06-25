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
#ifndef _CX_CORE_TASK_H
#define _CX_CORE_TASK_H

#include <functional>
#include <memory>
#include <string>

namespace cst {
/**
 * Task is an abstract class for tasks runnable by a worker.
 */
class Task {
public:
  Task(std::string const &name) : name(name) {}
  virtual ~Task() {}

  std::string const &getName() const { return name; }

  virtual void run() = 0;

private:
  std::string name;
};

typedef std::shared_ptr<Task> taskptr;

inline std::string to_string(taskptr task) {
  return "<" + task->getName() + ">";
}

/**
 * VoidTask is a task that does not take parameters.
 */
class VoidTask : public Task {
public:
  VoidTask(std::function<void()> func, std::string const &name = "")
      : Task(name), func(func) {}
  virtual ~VoidTask() {}

  virtual void run() { func(); }

private:
  std::function<void()> func;
};

/**
 * ParamTask is a task with a templated parameter and a function
 * taking that parameter.
 */
template <typename T> class ParamTask : public Task {
public:
  ParamTask(std::function<void(T)> f, T p, std::string const &name = "")
      : Task(name), p(p), f(f) {}
  ~ParamTask() {}

  void run() { f(p); }

private:
  T p;
  std::function<void(T)> f;
};
} // namespace cx

#endif // _CX_CORE_TASK_H
