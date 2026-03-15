#include "enpch.h"
#include "renderer.h"
#include <array>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "vertex_array.h"
#include "shader.h"
#include "uniform_buffer.h"
#include "core/assets/asset_database.h"
#include "core/os/time.h"

namespace engine {
    struct QuadVertex {
        glm::vec2 position;
        glm::vec4 color;
        glm::vec2 tex_coord;
        int tex_index;
        float time_offset;
        float time_speed;
    };

    struct RendererData {
        static constexpr unsigned int max_quads = 16384;
        static constexpr unsigned int max_vertices = max_quads * 4;
        static constexpr unsigned int max_indices = max_quads * 6;
        static int max_texture_slots;

        Renderer::Statistics stats;

        Ref<VertexArray> quad_vertex_array;
        Ref<VertexBuffer> quad_vertex_buffer;
        Ref<Shader> quad_shader;
        unsigned int quad_index_count = 0;
        QuadVertex* quad_vertex_buffer_base = nullptr;
        QuadVertex* quad_vertex_buffer_ptr = nullptr;
        glm::vec4 quad_vertex_positions[4] = {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            { 0.5f, -0.5f, 0.0f, 1.0f },
            { 0.5f, 0.5f, 0.0f, 1.0f },
            { -0.5f, 0.5f, 0.0f, 1.0f },
        };

        std::vector<Ref<Texture>> texture_slots;
        Ref<Texture> white_texture;
        unsigned int texture_slot_index = 1;

        struct CameraData {
            glm::mat4 view_projection = glm::mat4();
        };
        CameraData camera_data;
        Ref<UniformBuffer> camera_uniform_buffer;
        struct TimeData {
            TimeValue time;
            TimeValue delta_time;
        };
        TimeData time_data;
        Ref<UniformBuffer> time_uniform_buffer;
    };

    int RendererData::max_texture_slots;
    static RendererData _data;

    void Renderer::init() {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_data.max_texture_slots);
        if (_data.max_texture_slots > 32) // 32 is the maximum glsl array size
            _data.max_texture_slots = 32;

        _data.texture_slots.resize(_data.max_texture_slots);

        _data.quad_vertex_array = create_ref<VertexArray>();

        _data.quad_vertex_buffer = create_ref<VertexBuffer>(_data.max_vertices * static_cast<unsigned int>(sizeof(QuadVertex)));
        _data.quad_vertex_buffer->set_layout({
            { UniformType::Vec2, "a_Position" },
            { UniformType::Vec4, "a_Color" },
            { UniformType::Vec2, "a_TexCoord" },
            { UniformType::Int, "a_TexIndex" },
            { UniformType::Float, "a_TimeOffset" },
            { UniformType::Float, "a_TimeSpeed" },
            });
        _data.quad_vertex_array->add_vertex_buffer(_data.quad_vertex_buffer);

        _data.quad_vertex_buffer_base = new QuadVertex[_data.max_vertices];

        unsigned int* quad_indices = new unsigned int[_data.max_indices];
        unsigned int offset = 0;
        for (unsigned int i = 0; i < _data.max_indices; i += 6) {
            quad_indices[i + 0] = offset + 0;
            quad_indices[i + 1] = offset + 1;
            quad_indices[i + 2] = offset + 2;

            quad_indices[i + 3] = offset + 2;
            quad_indices[i + 4] = offset + 3;
            quad_indices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quad_index_buffer = create_ref<IndexBuffer>(quad_indices, _data.max_indices);
        _data.quad_vertex_array->set_index_buffer(quad_index_buffer);
        delete[] quad_indices;

        _data.white_texture = Texture::GenFlatTexture(1, 1, 0xffffffff);

        std::vector<int> samplers;
        samplers.resize(_data.max_texture_slots * sizeof(int));
        for (unsigned int i = 0; i < _data.max_texture_slots; i++)
            samplers[i] = i;

        _data.quad_shader = AssetDatabase::load_shader("res/shaders/quad.glsl");
        _data.quad_shader->bind();
        _data.quad_shader->set_uniform_int_buffer("u_Textures", _data.max_texture_slots, samplers.data());

        _data.texture_slots[0] = _data.white_texture;

        _data.camera_uniform_buffer = create_ref<UniformBuffer>(static_cast<unsigned int>(sizeof(RendererData::CameraData)), 0);

        _data.time_uniform_buffer = create_ref<UniformBuffer>(static_cast<unsigned int>(sizeof(RendererData::TimeData)), 1);
    }

    void Renderer::finalize() {
        delete[] _data.quad_vertex_buffer_base;
    }

    void Renderer::begin_scene(const Ref<Camera>& camera) {
        _data.camera_data.view_projection = camera->get_view_projection();
        _data.camera_uniform_buffer->set_data(&_data.camera_data, sizeof(RendererData::CameraData));

        _data.time_data.time = Time::get_time();
        _data.time_data.delta_time = Time::get_delta_time();
        _data.time_uniform_buffer->set_data(&_data.time_data, sizeof(RendererData::TimeData));

        start_batch();
    }

    void Renderer::end_scene() {
        end_batch();
    }

    void Renderer::start_batch() {
        _data.quad_index_count = 0;
        _data.quad_vertex_buffer_ptr = _data.quad_vertex_buffer_base;

        _data.texture_slot_index = 1;
    }

    void Renderer::end_batch() {
        if (_data.quad_index_count) {
            unsigned int data_size = (unsigned int)((unsigned char*)_data.quad_vertex_buffer_ptr - (unsigned char*)_data.quad_vertex_buffer_base);
            _data.quad_vertex_buffer->set_data(data_size, _data.quad_vertex_buffer_base);

            for (unsigned int i = 0; i < _data.texture_slot_index; i++)
                _data.texture_slots[i]->bind(i);

            _data.quad_shader->bind();
            _data.quad_vertex_array->bind();
            glDrawElements(GL_TRIANGLES, _data.quad_index_count, GL_UNSIGNED_INT, nullptr);
            _data.stats.draw_calls++;
        }
    }

    void Renderer::step_batch() {
        end_batch();
        start_batch();
    }

    void Renderer::draw_quad(const Transform& transform) {
        draw_quad(transform, _data.white_texture, glm::vec4(1.0f));
    }

    void Renderer::draw_quad(const Transform& transform, Ref<Texture> texture) {
        draw_quad(transform, texture, glm::vec4(1.0f));
    }

    void Renderer::draw_quad(const Transform& transform, glm::vec4 color) {
        draw_quad(transform, _data.white_texture, color);
    }

    void Renderer::draw_quad(const Transform& transform, Ref<Texture> texture, glm::vec4 color, float time_offset, float time_speed) {
        constexpr unsigned int quad_vertex_count = 4;
        constexpr glm::vec2 texture_coords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

        if (_data.quad_index_count + 6 >= RendererData::max_indices)
            step_batch();

        int texture_index = 0;
        bool found_texture = false;
        for (unsigned int i = 0; i < _data.texture_slot_index; i++) {
            if (_data.texture_slots[i]->get_id() == texture->get_id()) {
                texture_index = i;
                found_texture = true;
                break;
            }
        }

        if (!found_texture) {
            if (_data.texture_slot_index >= RendererData::max_texture_slots) {
                step_batch();
                draw_quad(transform, texture, color);
                return;
            }

            texture_index = _data.texture_slot_index;
            _data.texture_slots[_data.texture_slot_index] = texture;
            _data.texture_slot_index++;
        }

        for (unsigned int i = 0; i < quad_vertex_count; i++) {
            _data.quad_vertex_buffer_ptr->position = transform.get_matrix() * _data.quad_vertex_positions[i];
            _data.quad_vertex_buffer_ptr->color = color;
            _data.quad_vertex_buffer_ptr->tex_coord = texture_coords[i];
            _data.quad_vertex_buffer_ptr->tex_index = texture_index;
            _data.quad_vertex_buffer_ptr->time_offset = time_offset;
            _data.quad_vertex_buffer_ptr->time_speed = time_speed;
            _data.quad_vertex_buffer_ptr++;
        }

        _data.quad_index_count += 6;
        _data.stats.quad_count++;
    }

    void Renderer::reset_statistics() {
        memset(&_data.stats, 0, sizeof(Renderer::Statistics));
    }

    Renderer::Statistics Renderer::get_stats() {
        return _data.stats;
    }
}