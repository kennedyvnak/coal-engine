#pragma once 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/base.h"

namespace engine {
    class Texture;

    class Framebuffer {
    public:
        Framebuffer(unsigned int width, unsigned int height);
        ~Framebuffer();

        void bind() const;
        void unbind() const;

        void resize(unsigned int width, unsigned int height);

        inline unsigned int get_frame_buffer_id() const { return _id; }
        inline  int get_width() const { return _width; }
        inline  int get_height() const { return _height; }
        Texture& get_texture() const { return *_texture; }
    private:
        unsigned int _id;
        int _width;
        int _height;
        Scope<Texture> _texture;
    };
}