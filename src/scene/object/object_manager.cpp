#include "object_manager.hpp"

namespace Sierra{

    ObjectManager::ObjectManager(ComponentLoader componentLoader): componentLoader(std::move(componentLoader)), rowCount(0) {

    }
}