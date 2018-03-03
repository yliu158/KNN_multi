#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace std;

class ThreadPool {
public:
  explicit ThreadPool(size_t size) : work_(io_service_) {
    for (size_t i = 0; i < size; ++i) {
      workers_.create_thread(
          boost::bind(&boost::asio::io_service::run, &io_service_));
    }
  }

  ~ThreadPool() {
    io_service_.stop();
    workers_.join_all();
  }

  // Add new work item to the pool.
  template<class F>
  void Enqueue(F f) {
    io_service_.post(f);
  }

private:
  boost::thread_group workers_;
  boost::asio::io_service io_service_;
  boost::asio::io_service::work work_;
};
