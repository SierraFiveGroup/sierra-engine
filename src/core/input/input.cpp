#include "input.hpp"

namespace Sierra {
    //Base definitions
    bool InputHandler::mKeys[GLFW_KEY_LAST] = {};
    bool InputHandler::mMouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};
    double InputHandler::mMouseX = 0.0;
    double InputHandler::mMouseY = 0.0;
    glm::ivec2 InputHandler::mMousePos = glm::ivec2(0, 0);
    double InputHandler::mScrollOffset = 0.0;
    
    InputHandler::InputHandler(ResourceRef<Window> window) {
        GLFWwindow *ref = window->getGLFWWindow();
        
        glfwSetKeyCallback(ref, keyCallback);
        glfwSetMouseButtonCallback(ref, mouseButtonCallback);
        glfwSetCursorPosCallback(ref, cursorPositionCallback);
        glfwSetScrollCallback(ref, scrollCallback);

        glfwSetWindowUserPointer(ref, this);
    }

    void InputHandler::keyCallback(GLFWwindow*, int key, int, int action, int) {
        if (key >= 0 && key < GLFW_KEY_LAST)
            mKeys[key] = action != GLFW_RELEASE;
    }

    void InputHandler::mouseButtonCallback(GLFWwindow*, int button, int action, int) {
        if (button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
            mMouseButtons[button] = action; 
    }

    void InputHandler::cursorPositionCallback(GLFWwindow*, double xpos, double ypos) {
        mMousePos = glm::ivec2(xpos, ypos);
    }

    void InputHandler::scrollCallback(GLFWwindow*, double, double yoffset) {
        mScrollOffset = yoffset;
    }

    bool InputHandler::isKeyPressed(int key) {
        return key >= 0 && key < GLFW_KEY_LAST && mKeys[key];
    }

    bool InputHandler::isMouseButtonPressed(int button) {
        return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && mMouseButtons[button];
    }

    glm::ivec2 InputHandler::getMousePosition() {
        return mMousePos;
    }

    double InputHandler::getScrollOffset() {
        return mScrollOffset;
    }

    void InputHandler::endFrame() {
        mScrollOffset = 0.0;
    }
}