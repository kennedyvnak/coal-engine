#include "enpch.h"
#include "window.h"
#include "engine.h"
#include "core/events/window_event.h"
#include "core/events/key_event.h"
#include "core/events/mouse_event.h"

namespace engine {
	const int Window::min_width = 640;
	const int Window::min_height = 360;

	Window::Window(const std::string title) {
		_data.title = title;
	}

	Window::~Window() {
		glfwSetWindowSizeCallback(_ptr, nullptr);
		glfwDestroyWindow(_ptr);

		glfwTerminate();
	}

	void Window::init() {
#ifdef ENGINE_PLATFORM_LINUX
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
		int sucess = glfwInit();
		EN_ASSERT(sucess, "Can't initialize GLFW.");

#ifdef ENGINE_PLATFORM_MACOS
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#ifdef ENGINE_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

		if (_data.width <= 0)
			_data.width = vidmode->width;
		if (_data.height <= 0)
			_data.height = vidmode->height;

		_ptr = glfwCreateWindow(_data.width, _data.height, _data.title.c_str(), nullptr, nullptr);

		if (!_ptr) {
			glfwTerminate();
			EN_ASSERT(false, "Can't create GLFW window.");
		}

		glfwGetWindowPos(_ptr, nullptr, &start_y);

		if (_data.fullscreen)
			glfwSetWindowMonitor(_ptr, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);

		glfwSetWindowUserPointer(_ptr, &_data);
		glfwSetWindowSizeLimits(_ptr, Window::min_width, Window::min_height, GLFW_DONT_CARE, GLFW_DONT_CARE);

		glfwSetWindowSizeCallback(_ptr, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.width = width;
		data.height = height;

		data.minimized = width <= 0 && height <= 0;

		WindowResizeEvent event(width, height);
		data.event_callback(event);
			});

		glfwSetWindowCloseCallback(_ptr, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.event_callback(event);
			});

		glfwSetKeyCallback(_ptr, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS:
		{
			KeyPressedEvent event(key, 0);
			data.event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(key);
			data.event_callback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(key, true);
			data.event_callback(event);
			break;
		}
		}});

		glfwSetCharCallback(_ptr, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		KeyTypedEvent event(keycode);
		data.event_callback(event);
			});

		glfwSetMouseButtonCallback(_ptr, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(button);
			data.event_callback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(button);
			data.event_callback(event);
			break;
		}
		}});

		glfwSetScrollCallback(_ptr, [](GLFWwindow* window, double x_offset, double y_offset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)x_offset, (float)y_offset);
		data.event_callback(event);
			});

		glfwSetCursorPosCallback(_ptr, [](GLFWwindow* window, double pos_x, double pos_y) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)pos_x, (float)pos_y);
		data.event_callback(event);
			});

		glfwMakeContextCurrent(_ptr);

		glfwSwapInterval(_data.vsync);
	}

	void Window::set_width(int width) {
		_data.width = width;
		if (_ptr)
			glfwSetWindowSize(_ptr, width, _data.height);
	}

	void Window::set_height(int height) {
		_data.height = height;
		if (_ptr)
			glfwSetWindowSize(_ptr, _data.width, height);
	}

	void Window::set_fullscreen(bool fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(_ptr, fullscreen ? monitor : NULL, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
		_data.fullscreen = fullscreen;

		glfwSetWindowPos(_ptr, 0, start_y);
	}

	void Window::set_vsync(bool enabled) {
		_data.vsync = enabled;
		if (_ptr)
			glfwSwapInterval(enabled);
	}

	void Window::swap_buffers() {
		glfwSwapBuffers(_ptr);
	}

	void Window::poll_events() {
		glfwPollEvents();
	}

	bool Window::should_close() {
		return glfwWindowShouldClose(_ptr);
	}
}
