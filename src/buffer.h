#include <cassert>
#include <cstdint>
#include <memory>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

template <typename T, int S> struct Observable;
template <typename T, int S> struct Subscription;
template <typename T, int S> struct Subscriber {
  Observable<T, S> &b; // to the buffer
  uint64_t reader;     // reader index

  std::optional<T> popper() {
    return !b.empty() ? std::optional<T>{b.shift(reader)} : std::nullopt;
  };
  Subscription<T, S> getSubscription() { return Subscription<T, S>{*this}; }
};

template <typename T, int S> struct Subscription {
  explicit Subscription(Subscriber<T, S> &s) : s(s) {}
  auto pop() { return s.popper(); };
  void unsubscribe() { s.completed = true; }

private:
  Subscriber<T, S> &s;
};

template <typename T, int S> struct Observable {
  Observable() : data(std::make_unique<T[]>(S)) {}

  inline void onNext(T val) {
    read = minread();
    assert(!full());
    data[mask(write++)] = val;
  }

  Subscription<T, S> subscribe() {
    // if no value.. point to writer minus 1
    auto [it, success] = subscribers.insert(
        {++subscriber_ids, Subscriber<T, S>{*this, this->write}});

    assert(success);

    return it->second.getSubscription();
  }

  // private:
  static_assert(S && !(S & (S - 1)));
  uint64_t read = 0;
  uint64_t write = 0;
  std::unique_ptr<T[]> data;

  inline uint64_t size() { return write - read; }
  inline bool empty() { return read == write; }
  inline bool full() { return size() == S; }
  inline uint64_t mask(uint64_t idx) { return idx & (S - 1); }

  // inline const T &shift() {
  //   assert(!empty());
  //   return data[mask(read++)];
  // }

  inline const T &shift(uint64_t &reader) {
    assert(!(reader == write));

    return data[mask(reader++)];
  }

  /**
   * @brief find the slowest consumer
   *
   * @return uint64_t
   */
  uint64_t minread() {
    if (subscribers.size() > 0) {
      return std::accumulate(subscribers.begin(), subscribers.begin(),
                             subscribers.begin()->second.reader,
                             [](uint64_t min, const auto &r) {
                               return std::min(min, r.second.reader);
                             });
    } else {
      return read;
    }
  }

  std::unordered_map<uint8_t, Subscriber<T, S>> subscribers;

  uint8_t subscriber_ids = 0;
};

// template <typename T, int S> struct Observable : public Ringbuffer<T, S> {
//   uint64_t read() {
//     return std::accumulate(
//         subscribers.begin(), subscribers.begin(), uint64_t(-1),
//         [](uint64_t min, uint64_t r) { return std::min(min, r); });
//   }

//   std::unordered_map<uint8_t, Subscriber<T, S>> subscribers;

//   // if no value.. point to writer minus 1
//   Subscription<T, S> subscribe() {
//     auto [it, success] = subscribers.insert(
//         {++subscriber_ids, Subscriber<T, S>{*this, this->read}});

//     assert(success);

//     return it->second.getSubscription();
//   }

//   uint8_t subscriber_ids = 0;
// }
// ;
