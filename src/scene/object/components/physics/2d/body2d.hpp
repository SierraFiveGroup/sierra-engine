#pragma once

#include <memory>
#include <unordered_map>
#include <cassert>
#include <limits>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../component.hpp"
#include "audio/al_buffer.hpp"
#include "audio/al_sound.hpp"
#include "util/utilities.hpp"
#include "shape/shape.hpp"
#include "scene/object/components/transform/2d/transform2d.hpp"
#include "scene/physics_manager/physics_constants.hpp"

namespace Sierra {
    class PhysicsManager2D;
    class QuadTree;

    namespace Component {
        class Body2D : public Component {
            friend class Sierra::PhysicsManager2D;
            friend class Sierra::QuadTree;

            public:
                struct Info {
                    float mass = 1.0f;
                    float elasticity = 0.1f;
                    float friction = 0.1f;
                };

                Body2D();
                Body2D(Info& info);
                DISABLE_COPY_AND_MOVE(Body2D);

                glm::vec2 checkIntersection(ResourceRef<Body2D> other);

                void applyImpulse(glm::vec3 f);

                ResourceRef<Shape> getShape();

                Info getInfo();
                void setInfo(Info info);

                void setPhysicsConstants(ResourceRef<PhysicsConstants> constants);

                bool getIsSimulated();
                void setIsSimulated(bool isSimulated);

                bool getIsAffectedByGravity();
                void setIsAffectedByGravity(bool isAffectedByGravity);

                virtual void start() { return; }
                virtual void earlyUpdate(const StateInfo& stateInfo) { return; }
                virtual void update(const StateInfo& stateInfo) { return; }
                virtual void lateUpdate(const StateInfo& stateInfo);
                virtual void fixedUpdate(const StateInfo& stateInfo);
                virtual void frameCleanup() { return; }
                virtual bool hasChanged() { return false; }
            
                virtual std::vector<size_t> getRequiredComponentHashes() const override;
                virtual void setRequiredComponents(std::vector<ResourceRef<Component>> components) override;
            protected:
                bool mIsSimulated = true;
                bool misAffectedByGravity = true;
                bool mIsResting = false;
                bool mIsGrounded = false;

                bool resolveCollision(ResourceRef<Body2D> other);
            private:
                Info mInfo;
                glm::vec3 mVel;
                
            
                std::shared_ptr<Shape> mShape;


                static glm::vec2 getIntersectingNormal(std::vector<glm::vec2> vertices,  glm::vec2 p);

                ResourceRef<Transform2D> mTransform;
                ResourceRef<PhysicsConstants> physicsConstants;
        };
    }
}