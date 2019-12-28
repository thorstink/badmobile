#include <boost/lockfree/queue.hpp>
#include <boost/fiber/all.hpp>
#include <thread>
#include <atomic>
#include <iostream>

using namespace boost::lockfree;
using namespace boost::fibers;

queue<int, fixed_sized<true>> q{10000};
std::atomic<int> sum{0};

void produce()
{
  for (int i = 1; i <= 10000; ++i)
    q.push(i);
}

void consume()
{
  int i;
  while (q.pop(i))
    sum += i;
}

int main()
{
  boost::fibers::fiber f1{produce};
  boost::fibers::fiber f2{consume};
  boost::fibers::fiber f3{consume};
  f1.join();
  f2.join();
  f3.join();
  consume();
  std::cout << sum << '\n';
}