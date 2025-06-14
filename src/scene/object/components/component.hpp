#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "io/logging/logger.hpp"
#include "scene/ref/resource_ref.hpp"
#include "scene/state_info.hpp"

namespace Sierra {
    class Object;

    namespace Component {
        class Component {
            friend class Sierra::Object;

            public:
                std::string getComponentName();

                Component() = default;
                Component(std::weak_ptr<Object> ref);
                virtual ~Component() = default;

                virtual void start() = 0;
                virtual void earlyUpdate(const StateInfo& stateInfo) = 0;
                virtual void update(const StateInfo& stateInfo) = 0;
                virtual void lateUpdate(const StateInfo& stateInfo) = 0;
                virtual void fixedUpdate(const StateInfo& stateInfo) = 0;
                virtual void frameCleanup() = 0;
                virtual bool hasChanged() = 0;

                virtual std::vector<size_t> getRequiredComponentHashes() const;
                virtual void setRequiredComponents(std::vector<ResourceRef<Component>> components);
            protected:
                ResourceRef<Object> mObject;

                std::string mComponentName;

            private:
        };
    }
}