#include "enpch.h"
#include "engine.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "core/assets/asset_database.h"
#include "core/rendering/window.h"
#include "core/rendering/rendering_api.h"
#include "core/rendering/layers/layer.h"
#include "core/os/time.h"
#include "entities/rendering/texture_renderer.h"

namespace engine {
	Engine* Engine::_instance;
	const double EngineMetrics::frame_check_interval = 1.0 / 30.0;

	Engine::Engine(const EngineSpecification& specification)
		: _specs(specification) {
		_instance = this;

		AssetDatabase::load_database();

		_window = create_scope<Window>(specification.name);
		EN_ASSERT(_window->init() != -1, "Window creation failed");

		rendering::RenderingAPI::init();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
		_framebuffer = create_scope<Framebuffer>(vidmode->width, vidmode->height);
		glViewport(0, 0, vidmode->width, vidmode->height);

		_imgui_layer = new imgui::ImGuiLayer();
		_editor_layer = new editor::EditorLayer();
		push_overlay(_imgui_layer);
		push_layer(_editor_layer);
	}

	void Engine::run() {
		Ref<Camera> cam = create_ref<Camera>(0.0f, float(_window->get_width()), 0.0f, float(_window->get_height()), -1.0f, 1.0f);

		Ref<Texture> texture = AssetDatabase::load_texture("res/textures/hearth.png");

		Ref<entities::TextureRenderer> tex_renderer = create_ref<entities::TextureRenderer>(texture, Transform(glm::vec2(_window->get_width() * 0.5f, _window->get_height() * 0.5f), 0.0f, glm::vec2(100.0f)));

		rendering::RenderingAPI::set_clear_color(glm::vec4(0.07f, 0.13f, 0.17f, 1.0f));

		while (!_window->should_close()) {
			calculate_fps();

			_framebuffer->bind();

			for (Layer* layer : _layer_stack)
				layer->on_update();

			rendering::RenderingAPI::clear();

			float time = Time::get_time_since_startup();
			tex_renderer->get_material()->set_vector4("u_Color", glm::vec4(0.5f + sinf(time) * 0.5f, 0.5f + cosf(time) * 0.5f, 0.5f + sinf(time * 3.14f) * 0.5f, 1.0f));

			tex_renderer->draw(cam);

			_framebuffer->unbind();

			_imgui_layer->begin();

			for (Layer* layer : _layer_stack)
				layer->on_imgui_render();

			_imgui_layer->end();

			_window->swap_buffers();
			_window->poll_events();
		}
	}

	Engine* Engine::create_engine(EngineCommandLineArgs args) {
		EngineSpecification specification;
		specification.name = "Coal Engine";
		specification.command_line_args = args;
		return new Engine(specification);
	}

	void Engine::push_layer(Layer* layer) {
		_layer_stack.push_layer(layer);
		layer->on_attach();
	}

	void Engine::push_overlay(Layer* layer) {
		_layer_stack.push_overlay(layer);
		layer->on_attach();
	}

	void Engine::calculate_fps() {
		double current_time = Time::get_time_since_startup_as_double();
		double time_diff = current_time - _fps_previous_time;
		_frame_count++;
		_metrics.total_frame_count++;
		if (time_diff >= EngineMetrics::frame_check_interval) {
			_metrics.fps = _metrics.fps_as_double = 1.0 / time_diff * _frame_count;
			_metrics.ms = _metrics.ms_as_double = time_diff / _frame_count * 1000.0;
			_fps_previous_time = current_time;
			_frame_count = 0;
		}
	}
}
