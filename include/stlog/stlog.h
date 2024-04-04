#pragma once

#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "fmtlog.h"

namespace st {
namespace log {

#define LOG_DEBUG(format_str, ...) logd(format_str, ##__VA_ARGS__)
#define LOG_INFO(format_str, ...) logi(format_str, ##__VA_ARGS__)
#define LOG_WARN(format_str, ...) logw(format_str, ##__VA_ARGS__)
#define LOG_ERROR(format_str, ...) loge(format_str, ##__VA_ARGS__)

class Logger {
  public:
    static void Init(const std::filesystem::path& log_dir, const std::string& app_name,
                     const std::string& log_level = "DEBUG", const int32_t date = GetCurrentDate()) {
        // 设置日志文件路径
        char buffer[256] = {0};
        sprintf(buffer, "%s/%s_%d.log", log_dir.c_str(), app_name.c_str(), date);
        fmtlog::setLogFile(buffer);

        // 设置日志级别
        SetLogLevel(log_level);
        // 设置日志头格式
        fmtlog::setHeaderPattern("{YmdHMSF} {l} [{t:<6}] {s:<16} $");
        // 预分配线程队列
        fmtlog::preallocate();
        // 设置日志刷新级别
        fmtlog::flushOn(fmtlog::WRN);
        // 设置日志刷新时间间隔
        fmtlog::setFlushDelay(1e6);
        // 设置日志队列满回调函数
        fmtlog::setLogQFullCB(LogQFullCB, NULL);
        // 分配后台轮询线程
        fmtlog::startPollingThread(1e6);
    }

    static void SetThreadName(const std::string& thread_name) { fmtlog::setThreadName(thread_name.c_str()); }

  protected:
    static void LogQFullCB(void* userData) { FMTLOG(fmtlog::ERR, "Log Queue full."); }

  private:
    Logger();
    Logger& operator=(const Logger&);
    Logger(const Logger&);

    static int32_t GetCurrentDate() {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        return (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
    }

    static void SetLogLevel(const std::string& log_level) {
        if (log_level == "INFO") {
            fmtlog::setLogLevel(fmtlog::INF);
        } else if (log_level == "WARNING") {
            fmtlog::setLogLevel(fmtlog::WRN);
        } else if (log_level == "ERROR") {
            fmtlog::setLogLevel(fmtlog::ERR);
        } else if (log_level == "DEBUG") {
            fmtlog::setLogLevel(fmtlog::DBG);
        } else {
            throw std::runtime_error("Invalid log level. Please use DEBUG, INFO, WARNING, or ERROR.");
        }
    }
};
}  // namespace log
}  // namespace st