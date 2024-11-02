#include "my_logger.h"

void Logger::SetTimestamp(std::chrono::system_clock::time_point ts) {
    std::lock_guard<std::mutex> lock(mtx_);
    manual_ts_ = ts;
    OpenLogFile();
}

void Logger::OpenLogFile() {
    std::string new_date = GetFileTimeStamp();
    if (new_date != current_date_) {
        if (log_file_.is_open()) {
            log_file_.close();
        }
        current_date_ = new_date;
        log_file_.open("/var/log/sample_log_" + current_date_ + ".log", std::ios::app);
    }
}