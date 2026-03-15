#include <coal_engine.h>
#include "log_window.h"
#include <core/logging/logger.h>

namespace engine {
    void LogWindow::add_log(const LogData& log) {
        _logs.push_back(Log{ engine::format("[{:%R:%OS}]: {}\n", log.time, log.message.c_str()), (char)log.level });
    }

    void LogWindow::clear() {
        _logs.clear();
    }

    void LogWindow::render() {
        if (!ImGui::Begin("Console")) {
            ImGui::End();
            return;
        }

        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &_auto_scroll);
            ImGui::Checkbox("Show Traces", &_filter_mask[0]);
            ImGui::Checkbox("Show Infos", &_filter_mask[1]);
            ImGui::Checkbox("Show Warnings", &_filter_mask[2]);
            ImGui::Checkbox("Show Errors", &_filter_mask[3]);
            ImGui::Checkbox("Show Critical", &_filter_mask[4]);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        _filter.Draw("Filter", -100.0f);

        ImGui::Separator();

        if (ImGui::BeginChild("scrolling", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            if (clear)
                this->clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));

            std::vector<Log> _filtered_logs;
            for (const auto& log : _logs) {
                if (_filter_mask[log.level])
                    _filtered_logs.push_back(log);
            }

            if (_filter.IsActive()) {
                for (const auto& log : _filtered_logs) {
                    const char* msg = log.message.c_str();
                    if (_filter.PassFilter(msg, msg + log.message.size()))
                        draw_log(log);
                }
            } else {
                ImGuiListClipper clipper;
                clipper.Begin(_filtered_logs.size());
                while (clipper.Step()) {
                    for (int log_no = clipper.DisplayStart; log_no < clipper.DisplayEnd; log_no++) {
                        draw_log(_filtered_logs[log_no]);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            if (_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }

    void LogWindow::draw_log(const Log& log) {
        ImVec4 color;
        switch (log.level) {
        case 0: // TRACE
            color = ImVec4{ 0.6f, 0.6f, 0.6f, 1.0f };
            break;
        case 1: // INFO
            color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            break;
        case 2: // WARNING
            color = ImVec4{ 0.95f, 0.835f, 0.12f, 1.0f };
            break;
        case 3: // ERROR
            color = ImVec4{ 0.95f, 0.335f, 0.2f, 1.0f };
            break;
        case 4: // CRITICAL
            color = ImVec4{ 1.0f, 0.1f, 0.05f, 1.0f };
            break;
        default:
            color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            break;
        }
        ImGui::TextColored(color, log.message.c_str());
    }
}
