#include "enpch.h"
#include "logger.h"
#if ENGINE_EDITOR
#include <editor_gui/log_window.h>
#endif
#ifdef ENGINE_PLATFORM_WINDOWS
#include "console_color.h"
#endif // ENGINE_PLATFORM_WINDOWS

namespace engine {
	Logger* Logger::_instance;

	Logger::Logger() {}

	Logger::~Logger() {}

	void Logger::initialize_logger() {
		_instance = new Logger();
#if ENGINE_EDITOR
		_instance->_log_window = new LogWindow();
#endif
	}

#ifdef ENGINE_PLATFORM_WINDOWS
#define COUT_TEXT(color, abbreviation) std::cout << dye::color(engine::format(#abbreviation "({:%T}): {}", log.time, log.message)) << std::endl
#else
#define COUT_TEXT(color, abbreviation) std::cout << engine::format(#abbreviation "({:%T}): {}", log.time, log.message) << std::endl
#endif

	void Logger::ilog(const Log& log) {
#if ENGINE_EDITOR
		_log_window->add_log(log);
#endif

		switch (log.level) {
		case 0: // TRACE
			COUT_TEXT(grey, TRC);
			break;
		case 1: // INFO
			COUT_TEXT(vanilla, INF);
			break;
		case 2: // WARNING
			COUT_TEXT(yellow, WRN);
			break;
		case 3: // ERROR
			COUT_TEXT(red, ERR);
			break;
		case 4: // CRITICAL
			COUT_TEXT(red_on_black, CRT);
			break;
		}
	}
}
