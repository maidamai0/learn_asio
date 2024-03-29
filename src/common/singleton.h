#pragma once

/**
 * @file singleton.h
 * @author tonghao.yuan (yuantonghao@gmail.com)
 * @brief   common functions related to singleton
 * @version 0.1
 * @date 2021-05-12
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "common/macro.hpp"

/**
 * @brief make a sigleton
 * @details make consturctor of T private and let this function be friends of T
 * @tparam T    singleton type
 * @return      T
 */
template <typename T>
auto make_singleton() -> T& {
  static T instance;
  return instance;
}

#define enable_singleton(T)            \
  friend auto make_singleton<T>()->T&; \
                                       \
 public:                               \
  no_copy(T);                          \
  no_move(T);                          \
                                       \
 private:
