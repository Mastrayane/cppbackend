#include "logger.h"

 void init_logging() {
    logging::add_common_attributes();

    auto core = logging::core::get();

    auto console_sink = logging::add_console_log(std::cout, keywords::format = expr::stream
        << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%dT%H:%M:%S.%f")
        << " " << expr::attr<std::string>("Message")
        << " " << expr::attr<json::value>("AdditionalData"));

    console_sink->set_formatter([](logging::record_view const& rec, logging::formatting_ostream& strm) {
        json::object log_entry;
        log_entry["timestamp"] = boost::posix_time::to_iso_extended_string(rec[timestamp].get());
        log_entry["message"] = rec[message].get();
        log_entry["data"] = rec[additional_data].get();

        strm << json::serialize(log_entry);
        });
}

