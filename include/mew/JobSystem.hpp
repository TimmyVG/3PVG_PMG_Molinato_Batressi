#include<atomic>
#include<chrono>
#include<condition_variable>
#include<functional>
#include<future>
#include<iostream>
#include<memory>
#include<mutex>
#include<numeric>
#include<optional>
#include<queue>
#include<string>
#include<thread>
#include<tuple>
#include<utility>
#include<vector>

class JobSystem {
public:
  JobSystem();
  ~JobSystem();

  //TODO: Implement move constructor and assignment
  JobSystem(JobSystem&&);
  JobSystem& operator=(JobSystem&);

  template<typename T, typename... Args>
  std::future<std::invoke_result_t<T, Args&&...>>
    add(T&& f, Args... args);

private:
  void worker();
  void add_implementation(std::move_only_function<void()> f);
  //Worker shared context
  std::mutex mutex_;
  std::queue<std::move_only_function<void()>> tasks_;
  std::condition_variable worker_update_;
  std::atomic<bool> quit_;

  std::vector<std::thread> workers_;
};