/*#include "physics_manager2d.hpp"

namespace Sierra {
    PhysicsManager2D::PhysicsManager2D(): quadTree(glm::vec2(0.0f), glm::vec2(1000.0f, 1000.0f)) {
        physicsConstants = std::make_shared<PhysicsConstants>();
    }

    void PhysicsManager2D::addBody(ResourceRef<Component::Body2D> body) {
        if (body.isEmpty()) {
            ERROR("Tried to add null body to the physics manager");
            return;
        }

        body->setPhysicsConstants(physicsConstants);

        bodies.insert(body);

        quadTree.addBody(body);
    }

    void PhysicsManager2D::removeBody(ResourceRef<Component::Body2D> body) {
        bodies.erase(body);
    }

    void PhysicsManager2D::update() {
        quadTree.update();
        quadTree.draw();

        for (ResourceRef<Component::Body2D> b : bodies) {
            b->mIsGrounded = false;
        }

    }

    void PhysicsManager2D::setConstants(PhysicsConstants& constants) {
        *physicsConstants = constants;
    }

    PhysicsConstants PhysicsManager2D::getConstants() {
        return *physicsConstants;
    }
}*/