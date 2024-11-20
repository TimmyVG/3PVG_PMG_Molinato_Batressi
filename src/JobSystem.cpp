#include "mew/JobSystem.hpp"


  JobSystem::JobSystem() {
    auto threads = std::thread::hardware_concurrency();
    if (threads == 0) threads = 1;
    for (auto it = 0; it != threads; it++) {
      workers_.push_back(std::thread{ [this]() { worker(); } });
    }
  }

  JobSystem::~JobSystem() {
    quit_ = true;
    worker_update_.notify_all();
    for (auto& t : workers_) {
      t.join();
    }
  }

  JobSystem::JobSystem(JobSystem&&)
  {
  }

  JobSystem& JobSystem::operator=(JobSystem&)
  {
    return *this;
  }

  void JobSystem::worker() {
    while (true) {
      std::move_only_function<void()> f;
      {
        //CRITICAL SECTION START
        std::unique_lock<std::mutex> lock{ mutex_ };
        worker_update_.wait(lock, [this]() { return !tasks_.empty() || quit_; });
        if (quit_) break;
        f = std::move(tasks_.front());
        tasks_.pop();
        //CRITICAL SECTION END
      }
      f();
    }
  }

  void JobSystem::add_implementation(std::move_only_function<void()> f)
  {
    {
      //CRITICAL SECTION START
      std::lock_guard<std::mutex> lock{ mutex_ };
      tasks_.emplace(std::move(f));
      //CRITICAL SECTION END
    }
    worker_update_.notify_one();
  }