#pragma once

#include <memory>
#include <unordered_set>

#include "qt/quad_tree.hpp"

#include "../physics_constants.hpp"
#include "scene/ref/resource_ref.hpp"
#include "scene/object/components/physics/2d/body2d.hpp"


namespace Sierra {
    class PhysicsManager2D {
        friend class Scene;

        public:
            PhysicsManager2D();

            void addBody(ResourceRef<Component::Body2D> body);
            void removeBody(ResourceRef<Component::Body2D> body);

            void setConstants(PhysicsConstants& constants);
            PhysicsConstants getConstants();

        protected:
            void update();

        private:
            QuadTree quadTree;

            std::unordered_set<ResourceRef<Component::Body2D>> bodies;

            std::shared_ptr<PhysicsConstants> physicsConstants;
    }; 
}