#include "buffer.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

int main() {
  Observable<int, 4> o;
  // initial value
  o.onNext(1);
  o.onNext(2);
  o.onNext(3);
  o.onNext(4);

  std::cout << std::setw(16) << std::this_thread::get_id() << " thread id main"
            << std::endl;

  // subscription is now a handle to access the buffer
  auto subscription = o.subscribe();

  std::cout << subscription.p() << std::endl;
  std::cout << subscription.p() << std::endl;
  std::cout << subscription.p() << std::endl;

  uint64_t N = 1000L * 1000L * 100L;
  // initial value counter
  uint64_t c = 0;

  auto end = std::chrono::high_resolution_clock::now();

  // std::thread t0([&] {
  //   std::cout << std::setw(16) << std::this_thread::get_id() << " thread 1
  //   id"
  //             << std::endl;
  //   while (c < N) {
  //     b.shift();
  //     c++;
  //   }
  //   end = std::chrono::high_resolution_clock::now();
  // });

  // b.shift();
}
