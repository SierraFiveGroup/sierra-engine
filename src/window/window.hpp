#pragma once

#include <string>
#include <stdexcept>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "io/logging/logger.hpp"
#include "api.hpp"

namespace Sierra {
    struct Resolution {
        uint16_t w;
        uint16_t h;
    };
    
    class Window {
        public:
            Window(std::string name, Resolution resolution, API api);
    
            Window(Window&& other) noexcept;
            void operator=(Window&& other) noexcept;
    
            Window(Window& other) = delete;
            void operator=(Window& other) = delete;
    
            ~Window();
    
            bool shouldClose();
            void update();

            Resolution getResolution();
            GLFWwindow* getGLFWWindow();
    
        private:
            void initGLFW();
            void createWindow(std::string name);
            void initGLAD();
    
            Resolution resolution;
            API api;
    
            GLFWwindow* glfwWin;
    
    
            static size_t instanceCount;
    };
}