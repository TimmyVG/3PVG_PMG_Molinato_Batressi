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
  JobSystem(JobSystem&&);
  JobSystem& operator=(JobSystem&);
  ~JobSystem();

  template<typename T> typename std::future<std::invoke_result_t<T>>
  add(T&& f) {
    typedef typename std::invoke_result_t<T> result;
    std::packaged_task<result()> task(std::move(f));
    auto future = task.get_future();

    add_implementation(std::move(task));
    return future;
  };

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
