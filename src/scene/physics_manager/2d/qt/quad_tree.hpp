/*#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "scene/renderer/dbg/debug_renderer.hpp"
#include "scene/object/components/physics/2d/body2d.hpp"


namespace Sierra {
    class QuadTree {
        
        struct ChildObject {
            std::shared_ptr<bool> updated;
            bool affected;
        };

        struct Node {
            std::unordered_map<ResourceRef<Component::Body2D>, ChildObject> bodies;
            std::vector<Node> children;
            
            uint32_t depth;
            
            glm::vec2 pos;
            glm::vec2 size;
        };
        
        public:

            QuadTree(glm::vec2 pos, glm::vec2 size);

            QuadTree(const QuadTree&) = delete;
            QuadTree(QuadTree&&) = delete;

            void update();
            void draw();

            void addBody(ResourceRef<Component::Body2D> body);

        private:
            void updateNode(Node& node);
            void segmentNode(Node& node);
            void drawNode(const Node& node);
            void resolveCollisionsOnNode(Node& node);

            bool isInNode(Node& node, ResourceRef<Component::Body2D> body);
            bool addBodyToNode(Node& node, ChildObject obj, ResourceRef<Component::Body2D> body);
            void resetNodeState(Node& node);

            Node mRootNode;

            const size_t NODE_OBJECT_TRESHOLD = 5;
            const size_t MAX_DEPTH = 10;
    };
}*/