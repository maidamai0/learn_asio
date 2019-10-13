#include <asio/defer.hpp>
#include <asio/executor.hpp>
#include <asio/post.hpp>
#include <asio/strand.hpp>
#include <asio/system_executor.hpp>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <typeinfo>
#include <vector>

#include <fmt/core.h>
#include <fmt/ostream.h>

using asio::defer;
using asio::executor;
using asio::post;
using asio::strand;
using asio::system_executor;

//------------------------------------------------------------------------------
// A tiny actor framework
// ~~~~~~~~~~~~~~~~~~~~~~

class actor;

// Used to identify the sender and recipient of messages.
typedef actor* actor_address;

// Base class for all registered message handlers.
class message_handler_base {
 public:
  virtual ~message_handler_base() {}

  // Used to determine which message handlers receive an incoming message.
  virtual const std::type_info& message_id() const = 0;
};

// Base class for a handler for a specific message type.
template <class Message> class message_handler : public message_handler_base {
 public:
  // Handle an incoming message.
  virtual void handle_message(Message msg, actor_address from) = 0;
};

// Concrete message handler for a specific message type.
// c++ has no explicit language support for reflection, This is a way to store
// the handlers of Actor,like a
// map<message,Actor_member_function_to_handle_message>
template <class Actor, class Message>
class mf_message_handler : public message_handler<Message> {
 public:
  // Construct a message handler to invoke the specified member function.
  mf_message_handler(void (Actor::*mf)(Message, actor_address), Actor* a)
      : handler_(mf), actor_(a) {}

  // Used to determine which message handlers receive an incoming message.
  virtual const std::type_info& message_id() const { return typeid(Message); }

  // Handle an incoming message.
  virtual void handle_message(Message msg, actor_address from) {
    (actor_->*handler_)(std::move(msg), from);
  }

  // Determine whether the message handler represents the specified function.
  bool is_function(void (Actor::*mf)(Message, actor_address)) const {
    return mf == handler_;
  }

 private:
  void (Actor::*handler_)(Message, actor_address);  // Actor member funciton
  Actor* actor_;  // used to call member funtions of class Actor
};

// Base class for all actors.
class actor {
 public:
  virtual ~actor() {}

  // Obtain the actor's address for use as a message sender or recipient.
  actor_address address() { return this; }

  // Send a message from one actor to another.
  template <class Message>
  friend void send(Message msg, actor_address from, actor_address to) {
    // Execute the message handler in the context of the target's executor.
    post(to->executor_, [=]() { to->call_handler(std::move(msg), from); });
  }

  virtual size_t get_id() const { return 0; }

 protected:
  // Construct the actor to use the specified executor for all message handlers.
  actor(executor e) : executor_(std::move(e)) {}

  // Register a handler for a specific message type. Duplicates are permitted.
  template <class Actor, class Message>
  void register_handler(void (Actor::*mf)(Message, actor_address)) {
    handlers_.push_back(std::make_shared<mf_message_handler<Actor, Message>>(
        mf, static_cast<Actor*>(this)));
  }

  // Deregister a handler. Removes only the first matching handler.
  // type is same and address is same -> match
  template <class Actor, class Message>
  void deregister_handler(void (Actor::*mf)(Message, actor_address)) {
    const std::type_info& id = typeid(message_handler<Message>);
    for (auto iter = handlers_.begin(); iter != handlers_.end(); ++iter) {
      if ((*iter)->message_id() == id) {
        auto mh = static_cast<mf_message_handler<Actor, Message>*>(iter->get());
        if (mh->is_function(mf)) {
          handlers_.erase(iter);
          return;
        }
      }
    }
  }

  // Send a message from within a message handler.
  template <class Message> void tail_send(Message msg, actor_address to) {
    // Execute the message handler in the context of the target's executor.
    actor* from = this;
    // same as post
    defer(to->executor_, [=] { to->call_handler(std::move(msg), from); });
  }

 private:
  // Find the matching message handlers, if any, and call them.
  template <class Message> void call_handler(Message msg, actor_address from) {
    const std::type_info& message_id = typeid(Message);
    for (auto& h : handlers_) {
      if (h->message_id() == message_id) {
        auto mh = static_cast<message_handler<Message>*>(h.get());
        mh->handle_message(msg, from);
      }
    }
  }

  // All messages associated with a single actor object should be processed
  // non-concurrently. We use a strand to ensure non-concurrent execution even
  // if the underlying executor may use multiple threads.
  strand<executor> executor_;

  std::vector<std::shared_ptr<message_handler_base>> handlers_;
};

// A concrete actor that allows synchronous message retrieval.
template <class Message> class receiver : public actor {
 public:
  receiver() : actor(system_executor()) {
    register_handler(&receiver::message_handler);
  }

  // Block until a message has been received.
  Message wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (condition_.wait_for(lock, std::chrono::seconds(1),
                            [this] { return !message_queue_.empty(); })) {
      Message msg(std::move(message_queue_.front()));
      message_queue_.pop_front();
      fmt::print("{} [{}] message is {:<3}\n", __FUNCTION__,
                 std::this_thread::get_id(), msg);
      return msg;
    } else {
      fmt::print("{} [{}] time out\n", __FUNCTION__,
                 std::this_thread::get_id());
      return Message{};
    }
  }

 private:
  // Handle a new message by adding it to the queue and waking a waiter.
  void message_handler(Message msg, actor_address /* from */) {
    std::lock_guard<std::mutex> lock(mutex_);
    message_queue_.push_back(std::move(msg));
    //    fmt::print("{} [{}] message is {}\n", __FUNCTION__,
    //               std::this_thread::get_id(), msg);
    condition_.notify_one();
  }

  std::mutex mutex_;
  std::condition_variable condition_;
  std::deque<Message> message_queue_;
};

//------------------------------------------------------------------------------

#include <asio/thread_pool.hpp>
#include <iostream>

using asio::thread_pool;

class member : public actor {
 public:
  explicit member(executor e) : actor(std::move(e)), serial_number_{number_++} {
    register_handler(&member::init_handler);
  }

  size_t get_id() const override { return serial_number_; }

 private:
  void init_handler(actor_address next, actor_address from) {
    next_ = next;
    caller_ = from;

    register_handler(&member::token_handler);
    deregister_handler(&member::init_handler);
  }

  // if token is 0, send to caller
  // else send token-1 to next
  void token_handler(int token, actor_address /*from*/) {
    int msg(token);
    actor_address to(caller_);
    //    fmt::print("{} [{}] [{}] message is {}\n", __FUNCTION__,
    //    serial_number_,
    //               std::this_thread::get_id(), msg);

    if (token > 0) {
      msg = token - 1;
      to = next_;
    }

    tail_send(msg, to);
  }

  actor_address next_{nullptr};
  actor_address caller_{nullptr};
  size_t serial_number_{0};
  static size_t number_;
};

size_t member::number_ = 0;

int main() {
  const std::size_t num_threads = 16;
  const std::size_t num_actors = 100;

  // thread pool
  struct single_thread_pool : thread_pool {
    single_thread_pool() : thread_pool(1) {}
  };
  single_thread_pool pools[num_threads];

  // Create the member actors.
  const std::size_t actors_per_thread = num_actors / num_threads;
  std::vector<std::shared_ptr<member>> members(num_actors);
  for (std::size_t i = 0; i < num_actors; ++i) {
    members[i] = std::make_shared<member>(
        pools[(i / actors_per_thread) % num_threads].get_executor());
  }

  // link the members to make them a loop link
  receiver<int> rcvr;
  for (std::size_t i = num_actors, next_i = 0; i > 0; next_i = --i) {
    // i    next_i    i-1
    // 503    0       502
    // 502    502     501
    // 1      1       0
    send(members[next_i]->address(), rcvr.address(), members[i - 1]->address());
  }

  // Send exactly one token to each actor, all with the same initial value,
  // rounding up if required.
  const int num_hops = 50000;
  const int token_value = (num_hops + num_actors - 1) / num_actors;
  for (std::size_t i = 0; i < 5; ++i) {
    send(token_value, rcvr.address(), members[i]->address());
  }

  // Wait for all signal messages, indicating the tokens have all reached zero.
  for (std::size_t i = 0; i < num_actors; ++i) {
    fmt::print("[{:<3}]rcvr waiting...", i);
    rcvr.wait();
  }
}
