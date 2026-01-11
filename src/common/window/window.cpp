#include "window.hpp"

#if !defined(__APPLE__)
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {

    if (severity <= GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
           LOG(message); 
           break;
        case GL_DEBUG_SEVERITY_MEDIUM:
           WARN(message); 
           break;
        case GL_DEBUG_SEVERITY_HIGH:
           ERROR(message); 
           throw std::runtime_error("problem izkusenj");
           break;
    }
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    //glViewport(0, 0, width, height);
}
#endif // !__APPLE__


// During init, enable debug output

namespace Sierra {
    size_t Window::instanceCount = 0;

    Window::Window(std::string name, Resolution resolution, API api): 
     resolution(resolution), glfwWin(nullptr), api(api) {
    
        if (instanceCount == 0) {
            initGLFW();
        }
    
        createWindow(name);
        //init_glad();
    
        instanceCount++;
    }
    
    void Window::createWindow(std::string name) {
        glfwWin = glfwCreateWindow(resolution.w, resolution.h, name.c_str(), nullptr, nullptr);
    
        if ( glfwWin == nullptr ) {
            throw std::runtime_error("Failed to create new GLFW window");
        }

        // Only bind GL context and callbacks when using OpenGL and not on macOS
    #if !defined(__APPLE__)
        if (api == API::gl) {
            glfwMakeContextCurrent(glfwWin);
            glfwSetWindowSizeCallback(glfwWin, framebufferSizeCallback);
        }
    #endif
        //glfwSwapInterval(1);
    }
    
    void Window::initGLFW() {
#ifdef __linux__
#ifdef DEBUG
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
#endif

        if ( glfwInit() == GLFW_FALSE ) {
            throw std::runtime_error("Failed to init GLFW");
        }
    
        if(api == API::gl) {
    #if defined(__APPLE__)
            // Disable OpenGL on macOS builds entirely
            throw std::runtime_error("OpenGL is disabled on macOS build");
    #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            // Forward-compat is not needed when OpenGL is disabled on macOS
    #endif
        } else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //TODO make interchangable
        }
    }
    
/*    void Window::initGLAD() {
        if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ) {
            throw std::runtime_error("Failed to init GLAD, womp womp");
        }

        glViewport(0, 0, resolution.w, resolution.h);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEBUG_OUTPUT);

#ifdef DEBUG
        glDebugMessageCallback(MessageCallback, 0);
#endif

    }*/

    bool Window::shouldClose() {
        return glfwWindowShouldClose(glfwWin);
    }

    void Window::update() {
        glfwSwapBuffers(glfwWin);
        glfwPollEvents();
    }
    
    GLFWwindow* Window::getGLFWWindow() {
        return glfwWin;
    }
    
    Resolution Window::getResolution() {
        return resolution;
    }
    
    Window::Window(Window&& other) noexcept{
        resolution = other.resolution;
        glfwWin = other.glfwWin;
    
        other.glfwWin = nullptr;
    }
    
    void Window::operator=(Window&& other) noexcept {
        resolution = other.resolution;
        glfwWin = other.glfwWin;
    
        other.glfwWin = nullptr;
    }
    
    Window::~Window() {
        if ( glfwWin == nullptr ) {
            return;
        }
    
        glfwDestroyWindow(glfwWin);
    
        if ( instanceCount == 1 ) {
            glfwTerminate();
        }
    
        instanceCount--;
    }
}
