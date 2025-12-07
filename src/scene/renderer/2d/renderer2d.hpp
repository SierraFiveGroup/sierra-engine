#pragma once


#include <unordered_map>
#include <map>
#include <algorithm>
#include <vector>
#include <list>
#include <cstring>
#include <thread>
#include <mutex>
#include <stdfloat>
#include <memory>
#include <functional>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../object/2d/object2d.hpp"
#include "../../object/camera/camera.hpp"
#include "gl/util/buffer/gl_buffer.hpp"
#include "gl/util/vao/gl_vao.hpp"
#include "util/utilities.hpp"
#include "batch2d/batch2d.hpp"

typedef uint32_t bool32_t;

namespace Sierra {

    class Renderer2D {
        public:
            Renderer2D();

            Renderer2D(Renderer2D&&) = delete;
            Renderer2D(Renderer2D&) = delete;

            void render(Camera& camera);
            void update();

            void addObject(ResourceRef<Object2D> object);
            ObjectMap::iterator removeObject2d(ResourceRef<Object2D> object, GLuint oldTex = 0);

            void freeUnusedMemory();
        private:
            void createGLBuffers();

            void cleanupBatch(RenderBatch2D& batch);
            void cleanupBatchLarge(RenderBatch2D& batch);
            void cleanupBatchSmall(RenderBatch2D& batch);

            void initBatch(RenderBatch2D& batch);

            void updateInstanceData();

            GLBuffer ebo;
            GLBuffer vbo;
            
            std::unordered_map<GLuint, std::vector<RenderBatch2D>> draw_batches_2d;

            std::list<std::thread> cleanupThreads;

    };
}