#include "Game.h" // has to be included before Window.h
#include "Window.h"

#include <utility>
#include <stdexcept>

namespace eng {

	Window::Window(Game* const game, int width, int height, const char* title) :
			handle(nullptr), game(game) {
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW.");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // create a debug opengl context for debug builds
#endif
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); // resize the window automatically when moved between monitors with different content scales
		//glfwWindowHint(GLFW_SAMPLES, 4);
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		//glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		handle = glfwCreateWindow(width, height, title, NULL, NULL);
		if (!handle) {
			throw std::runtime_error("Failed to create GLFW window.");
		}

		glfwMakeContextCurrent(handle);

		glfwSetWindowUserPointer(handle, this);

		glfwSwapInterval(vSync ? 1 : 0);

		glfwSetWindowCloseCallback(handle, [](GLFWwindow*) { Game::instance().quit(); });

		/*glfwSetWindowRefreshCallback(handle, [](GLFWwindow* windowHandle) {
			Window* const thisWindow = (Window* const) glfwGetWindowUserPointer(windowHandle);
			// TODO: redraw?
			//thisWindow->swapBuffers();
		});*/
		glfwSetFramebufferSizeCallback(handle, [](GLFWwindow*, int width, int height) {
			Game::instance().renderer.handleWindowResize(width, height);
			Game::instance().rendererResize(static_cast<size_t>(width), static_cast<size_t>(height));
		});
		glfwSetWindowContentScaleCallback(handle, [](GLFWwindow*, float scaleX, float scaleY) {
			Game::instance().renderer.handleWindowRescale(scaleX, scaleY);
			Game::instance().rendererRescale(scaleX, scaleY);
		});
		glfwSetWindowMaximizeCallback(handle, [](GLFWwindow* windowHandle, int maximized) {
			Window* const thisWindow = fromWindowHandle(windowHandle);
			const auto size = thisWindow->getSize();
			const auto scale = thisWindow->getContentScale();
			Game::instance().renderer.handleWindowResize(size.x, size.y, scale.x, scale.y);
			Game::instance().windowMaximizeCallback(static_cast<bool>(maximized));
		});
		glfwSetWindowIconifyCallback(handle, [](GLFWwindow* windowHandle, int minimized) {
			Window* const thisWindow = fromWindowHandle(windowHandle);
			const auto size = thisWindow->getSize();
			const auto scale = thisWindow->getContentScale();
			Game::instance().renderer.handleWindowResize(size.x, size.y, scale.x, scale.y);
			Game::instance().windowMinimizeCallback(static_cast<bool>(minimized));
		});
		glfwSetWindowFocusCallback(handle, [](GLFWwindow*, int focused) {
			Game::instance().windowFocusCallback(static_cast<bool>(focused));
		});

		// set window icon (not the icon shown in the task bar/dock)
		/*try {
			setIcon({ loadImage<4>("icon.png") });
		} catch (...) {
			std::cerr << "Failed to load window icon\n";
		}*/

		glfwSetWindowSizeLimits(handle, 212, 120, GLFW_DONT_CARE, GLFW_DONT_CARE);
	}

	Window::~Window() {
		if (handle) {
			glfwDestroyWindow(handle);
			glfwTerminate();
		}
	}

	Window::Window(Window&& b) :
			handle(std::exchange(b.handle, nullptr)), game(b.game) {}

	Window* Window::fromWindowHandle(GLFWwindow* windowHandle) {
		return static_cast<Window*>(glfwGetWindowUserPointer(windowHandle));
	}

	int Window::getWidth() const {
		int w;
		glfwGetFramebufferSize(handle, &w, nullptr);
		return w;
	}
	int Window::getHeight() const {
		int h;
		glfwGetFramebufferSize(handle, nullptr, &h);
		return h;
	}
	glm::ivec2 Window::getSize() const {
		glm::ivec2 size;
		glfwGetFramebufferSize(handle, &size.x, &size.y);
		return size;
	}

	glm::vec2 Window::getContentScale() const {
		glm::vec2 scale;
		glfwGetWindowContentScale(handle, &scale.x, &scale.y);
		return scale;
	}

	bool Window::isVSync() const {
		return vSync;
	}
	void Window::setVSync(bool vSync) const {
		Window::vSync = vSync;
		glfwSwapInterval(vSync ? 1 : 0);
	}

	void Window::setTitle(const char* title) const {
		glfwSetWindowTitle(handle, title);
	}
	void Window::setTitle(const std::string& title) const {
		glfwSetWindowTitle(handle, title.c_str());
	}

	void Window::setIcon(std::vector<Image4> iconImages) const {
		if (iconImages.size() > 0) {
			std::vector<GLFWimage> icons {};
			for (auto& img : iconImages) {
				if (img) icons.emplace_back(
					GLFWimage {
						static_cast<int>(img.getWidth()),
						static_cast<int>(img.getHeight()),
						img.get()
					}
				);
			}
			glfwSetWindowIcon(handle, static_cast<int>(icons.size()), icons.data());
		} else {
			glfwSetWindowIcon(handle, 0, nullptr);
		}
	}

	void Window::minimize() const {
		glfwIconifyWindow(handle);
	}
	void Window::maximize() const {
		glfwMaximizeWindow(handle);
	}
	void Window::restore() const {
		glfwRestoreWindow(handle);
	}
	bool Window::isMinimized() const {
		return glfwGetWindowAttrib(handle, GLFW_ICONIFIED);
	}
	bool Window::isMaximized() const {
		return glfwGetWindowAttrib(handle, GLFW_MAXIMIZED);
	}

	void Window::hide() const {
		glfwHideWindow(handle);
	}
	void Window::show() const {
		glfwShowWindow(handle);
	}
	bool Window::isVisible() const {
		return glfwGetWindowAttrib(handle, GLFW_VISIBLE);
	}


	bool Window::hasFocus() const {
		return glfwGetWindowAttrib(handle, GLFW_FOCUSED);
	}

	void Window::requestAttention() const {
		glfwRequestWindowAttention(handle);
	}

	void Window::swapBuffers() const {
		glfwSwapBuffers(handle);
	}

}