#pragma once

#include <functional>

#include <glm/glm.hpp>

#include "window/window.hpp"
#include "io/logging/logger.hpp"
#include "scene/ref/resource_ref.hpp"

namespace Sierra {
    class InputHandler {
        friend class Scene;
        public:

            InputHandler(ResourceRef<Window> window);

            InputHandler(const InputHandler&) = delete;
            InputHandler(const InputHandler&&) = delete;

            static bool isKeyPressed(int key);
            static bool isMouseButtonPressed(int button);

            glm::ivec2 getMousePosition();

            double getScrollOffset();
        
            static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
            static void mouseButtonCallback(GLFWwindow*, int button, int action, int mods);
            static void cursorPositionCallback(GLFWwindow*, double xpos, double ypos);
            static void scrollCallback(GLFWwindow*, double xoffset, double yoffset);
        protected:
            void endFrame();

        private:
            static bool mKeys[GLFW_KEY_LAST];
            static bool mMouseButtons[GLFW_MOUSE_BUTTON_LAST];
            static double mMouseX, mMouseY;
            static glm::ivec2 mMousePos;
            static double mScrollOffset;
    };
}