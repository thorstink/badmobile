#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

template <typename T, int S> struct Ringbuffer {
  static_assert(S && !(S & (S - 1)));
  Ringbuffer() : data(std::make_unique<T[]>(S)) {}
  uint64_t read = 0;
  uint64_t write = 0;
  std::unique_ptr<T[]> data;

  inline uint64_t size() { return write - read; }
  inline bool empty() { return read == write; }
  inline bool full() { return size() == S; }
  inline uint64_t mask(uint64_t idx) { return idx & (S - 1); }

  inline void onNext(T val) {
    assert(!full());
    data[mask(write++)] = val;
  }

  inline const T &shift() {
    assert(!empty());

    return data[mask(read++)];
  }
};

template <typename T, int S> struct Subscription;
template <typename T, int S> struct Subscriber {
  Ringbuffer<T, S> &b;
  uint64_t reader;
  bool completed = false;

  bool popper(T &t) {
    t = b.shift();
    return true;
  };
  Subscription<T, S> getSubscription() { return Subscription<T, S>{*this}; }
};

template <typename T, int S> struct Subscription {
  T p() {
    T val;
    s.popper(val);
    return val;
  };

  void unsubscribe() { s.completed = true; }

  Subscriber<T, S> &s;
};

template <typename T, int S> struct Observable : public Ringbuffer<T, S> {
  std::unordered_map<uint8_t, Subscriber<T, S>> subscribers;

  /**
   * @brief create a subscription (with unique id)
   *
   * @return Subscription<T, S>
   */
  Subscription<T, S> subscribe() {
    auto [it, success] = subscribers.insert(
        {++subscriber_ids, Subscriber<T, S>{*this, this->write - 1}});

    assert(success);

    return it->second.getSubscription();
  }

  uint8_t subscriber_ids = 0;
};
