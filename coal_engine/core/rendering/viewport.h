#pragma once

#include "framebuffer.h"

namespace engine {
    class Viewport {
    public:
        Viewport(int width, int height);
        ~Viewport() = default;

        void resize(int width, int height);

        inline int get_width() const { return _framebuffer->get_width(); }
        inline int get_height() const { return _framebuffer->get_height(); }

        inline Framebuffer& get_framebuffer() const { return *_framebuffer; };
    private:
        Scope<Framebuffer> _framebuffer;
    };
}