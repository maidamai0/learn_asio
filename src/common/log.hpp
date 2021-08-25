#pragma once

/**
 * @file log.hpp
 * @author tonghao.yuan (tonghao.yuan@csdental.com)
 * @brief thin warpper of spdlog
 * @version 0.1
 * @date 2021-06-30
 *
 * Copyright (c) 2021 Carestream Dental,LLC. All Rights Reserved
 *
 */

#include <string_view>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"

#include "common/singleton.h"

#define LOGD SPDLOG_DEBUG
#define LOGI SPDLOG_INFO
#define LOGW SPDLOG_WARN
#define LOGE SPDLOG_ERROR

#define TRACE log_details::ScopeTrace trace(__FUNCTION__)

namespace log_details {

class Log {
  enable_singleton(Log)

 public:
  Log() {
    auto file_sink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(fmt::format("logs/{}_log.txt", APP_NAME), true);
    auto std_cout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    spdlog::set_default_logger(
        std::make_shared<spdlog::logger>("LearnAsio", spdlog::sinks_init_list{std_cout_sink, file_sink}));

    spdlog::set_level(spdlog::level::debug);
    // spdlog::enable_backtrace(10);
    spdlog::flush_on(spdlog::level::debug);
    spdlog::set_pattern("%Y-%m-%d %T.%e %L %^%v%$ @%s:%#");

    LOGI("Application {} started", APP_NAME);
  }

  ~Log() { LOGI("Application {} terminated", APP_NAME); }
};

class ScopeTrace {
 public:
  ScopeTrace(std::string_view scope_name) : name_(scope_name) { LOGI(">>> {}", name_); }
  ~ScopeTrace() { LOGI("<<< {}", name_); }

  std::string_view name_;
};

}  // namespace log_details
