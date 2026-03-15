#include "enpch.h"
#include "viewport.h"
#include "engine.h"

namespace engine {
    Viewport::Viewport(unsigned int width, unsigned int height) {
        _framebuffer = create_scope<Framebuffer>(width, height);
        resize(width, height);
    }

    Viewport::~Viewport() = default;

    void Viewport::resize(unsigned int width, unsigned int height) {
        _framebuffer->resize(width, height);
        glViewport(0, 0, width, height);
    }
}