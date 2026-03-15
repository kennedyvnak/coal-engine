#pragma once

#include <string>
#include <chrono>
#include "utility/string_utility.h"

#define EN_LOG_TRACE(...) engine::Logger::get_instance()->log_trace(__VA_ARGS__)
#define EN_LOG_INFO(...) engine::Logger::get_instance()->log_info(__VA_ARGS__)
#define EN_LOG_WARNING(...) engine::Logger::get_instance()->log_warning(__VA_ARGS__)
#define EN_LOG_ERROR(...) engine::Logger::get_instance()->log_error(__VA_ARGS__)
#define EN_LOG_CRITICAL(...) engine::Logger::get_instance()->log_critical(__VA_ARGS__)

namespace engine {
#if ENGINE_EDITOR
	class LogWindow;
#endif

	struct LogData {
		int level;
		std::string message;
		std::chrono::local_time<std::chrono::system_clock::duration> time;
	};

	class Logger {
	public:
		using Log = LogData;

		Logger(const Logger&) = delete;

		static void initialize_logger();

		static inline Logger* get_instance() { return _instance; }

#if ENGINE_EDITOR
		static LogWindow* get_log_window() { return _instance->_log_window; }
#endif

#define LOG_FUNC(type, value)\
		template<typename ... Args>\
		void log_##type(const std::string_view& format_str, Args&&... args) {\
			ilog({ value, engine::format(format_str, args...), get_current_time() });\
		}\

		LOG_FUNC(trace, 0);
		LOG_FUNC(info, 1);
		LOG_FUNC(warning, 2);
		LOG_FUNC(error, 3);
		LOG_FUNC(critical, 4);
#undef LOG_FUNC
	private:
		Logger();
		~Logger();

#if ENGINE_EDITOR
		LogWindow* _log_window;
#endif

		static Logger* _instance;

		std::chrono::local_time<std::chrono::system_clock::duration> get_current_time() { return std::chrono::current_zone()->to_local(std::chrono::system_clock::now()); }

		void ilog(const Log& log);
	};
}
