#include "enpch.h"
#include "rendering_api.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer.h"
#include "engine.h"
#include "texture.h"
#include "shader.h"
#include "vertex_array.h"
#include "core/components/transform.h"
#include "core/assets/asset_database.h"

namespace engine {
    RenderingAPI* RenderingAPI::_current_api;

    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) {
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         EN_LOG_CRITICAL("[OpenGL]: {}", message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:       EN_LOG_ERROR("[OpenGL]: {}", message); return;
        case GL_DEBUG_SEVERITY_LOW:          EN_LOG_WARNING("[OpenGL]: {}", message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: EN_LOG_INFO("[OpenGL]: {}", message); return;
        }

        EN_ASSERT(false, "Unknown severity level!");
    }

    void glfw_error_callback(int error_code, const char* description) {
        EN_LOG_ERROR("[GLFW Error] ({}): {}.", error_code, description);
        EN_DEBUG_BREAK();
    }

    void RenderingAPI::init() {
        if (glfwGetCurrentContext() == nullptr) {
            EN_LOG_ERROR("No current OpenGL context found before initializing GLEW!");
            EN_DEBUG_BREAK();
        }

#ifdef ENGINE_PLATFORM_LINUX
        if (glfwGetPlatform() != GLFW_PLATFORM_X11) {
            EN_LOG_WARNING("GLFW is not using X11 platform. GLEW initialization might fail (No GLX display).");
        }
#endif

        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            EN_LOG_ERROR("Glew initialization failed: {}", (const char*)glewGetErrorString(err));
            EN_DEBUG_BREAK();
        }

        _current_api = new RenderingAPI();

#ifndef ENGINE_PLATFORM_MACOS
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)opengl_message_callback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
        glfwSetErrorCallback(glfw_error_callback);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        EN_LOG_INFO("Initialize OpenGL | Version: {} | Renderer: {}", (char*)glGetString(GL_VERSION), (char*)glGetString(GL_RENDERER));
    }

    void RenderingAPI::iset_clear_color(glm::vec4 color) {
        _clear_color = color;
    }

    void RenderingAPI::iclear() {
        glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderingAPI::ienable_depth_test() {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderingAPI::idisable_depth_test() {
        glDisable(GL_DEPTH_TEST);
    }
}
