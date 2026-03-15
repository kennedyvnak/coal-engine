#pragma once

#include <imgui.h>
#include <vector>
#include <string>

namespace engine {
    struct LogData;

    class LogWindow {
    public:
        struct Log {
            std::string message;
            char level;
        };

        LogWindow() = default;
        ~LogWindow() = default;

        void add_log(const LogData& log);
        void clear();
        void render();
    private:
        std::vector<Log> _logs;
        ImGuiTextFilter _filter;
        bool _auto_scroll = true;
        bool _filter_mask[5] = { 1, 1, 1, 1, 1 };

        void draw_log(const Log& log);
    };
}
