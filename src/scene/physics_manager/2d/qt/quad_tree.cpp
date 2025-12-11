/*#include "quad_tree.hpp"
                
namespace Sierra {

    QuadTree::QuadTree(glm::vec2 pos, glm::vec2 size) {
        mRootNode.pos = pos;
        mRootNode.size = size;
        mRootNode.depth = 1;
    }

    void QuadTree::update() {
        resetNodeState(mRootNode);

        updateNode(mRootNode);
        resolveCollisionsOnNode(mRootNode);
    }

    void QuadTree::draw() {
        drawNode(mRootNode);
    }

    void QuadTree::addBody(ResourceRef<Component::Body2D> body) {
        ChildObject obj;
        obj.updated = std::make_shared<bool>(false);
        obj.affected = true;

        addBodyToNode(mRootNode, obj, body);
    }

    bool QuadTree::addBodyToNode(Node& node, ChildObject obj, ResourceRef<Component::Body2D> body) {
        if (!isInNode(node, body))
            return false;

            
        if (node.children.empty()) {
            *obj.updated = true;
            obj.affected = true;

            node.bodies[body] = obj;

            return true;
        }
            
        bool inNode = false;
        for (Node& child : node.children) {
            if (addBodyToNode(child, obj, body))
                inNode = true;
        }

        return inNode;
    }

    void QuadTree::resetNodeState(Node& node) {
        for (Node& child : node.children) {
            resetNodeState(child);
        }

        for (auto& obj : node.bodies) {
            obj.second.affected = false;
            *obj.second.updated = false;
        }
    }

    void QuadTree::updateNode(Node& node) { // recursion actually being usefu???? unheard of
        if (node.bodies.size() > NODE_OBJECT_TRESHOLD && node.depth <= MAX_DEPTH) {
            segmentNode(node);
        }
        
        for(Node& child : node.children) {
            updateNode(child);
        }


        for (auto it = node.bodies.begin(); it != node.bodies.end();) {
            if (isInNode(node, it->first)) {
                it++;
                continue;
            }

            if(!*it->second.updated) 
                addBodyToNode(mRootNode, it->second, it->first);
            

            if (*it->second.updated && !it->second.affected)
                it = node.bodies.erase(it);
            else
                it++;
        }

        bool childrenEmpty = true;
        for(Node& child : node.children) {
            if (!child.bodies.empty() || !child.children.empty()) {
                childrenEmpty = false;
                break;
            }
        }

        if (childrenEmpty)
            node.children.clear();
    }

    void QuadTree::segmentNode(Node& node) {
        if (!node.children.empty())
            return;

        node.children.resize(4);
            
        for (Node& child : node.children) {
            child.size = node.size / 2;
            child.depth = node.depth + 1;
        }

        node.children[0].pos = glm::vec2(node.pos.x - node.size.x / 4, node.pos.y - node.size.y / 4);
        node.children[1].pos = glm::vec2(node.pos.x + node.size.x / 4, node.pos.y - node.size.x / 4);
        node.children[2].pos = glm::vec2(node.pos.x - node.size.x / 4, node.pos.y + node.size.y / 4);
        node.children[3].pos = glm::vec2(node.pos.x + node.size.x / 4, node.pos.y + node.size.y / 4);

        for (auto& pair : node.bodies) {
            addBodyToNode(mRootNode, pair.second, pair.first);
        }

        node.bodies.clear();
    }
    
    void QuadTree::drawNode(const Node& node) {
        Debug::Renderer::drawRect(glm::vec3(node.pos, 0.0), node.size, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), false, true);

        for (const Node& child : node.children) {
            drawNode(child);
        }
    }

    bool QuadTree::isInNode(Node& node, ResourceRef<Component::Body2D> body) {
        glm::vec2 bSize = body->mTransform->getScale();
        glm::vec2 bPos = (glm::vec2)body->mTransform->getPosition();

        if (
           bPos.x + bSize.x / 2 > node.pos.x - node.size.x / 2 && node.pos.x + node.size.x / 2 > bPos.x - bSize.x / 2 &&
           bPos.y + bSize.y / 2 > node.pos.y - node.size.y / 2 && node.pos.y + node.size.y / 2 > bPos.y - bSize.y / 2 
        ) {
            return true;
        }

        return false;
    }

    void QuadTree::resolveCollisionsOnNode(Node& node) {
        if (!node.children.empty()) {
            for (Node& child : node.children) {
                resolveCollisionsOnNode(child);
            }
        }

        for(auto& b1 : node.bodies) {
            for(auto& b2 : node.bodies) {
                if (b1.first.getHash());
                    continue;

                b1.first->resolveCollision(b2.first);
            }
        }
    }
}
*/