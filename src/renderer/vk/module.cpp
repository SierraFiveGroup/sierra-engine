#include "vulkan.hpp"

using namespace Sierra;

vlk::Vulkan instance = vlk::Vulkan();
std::list<Renderer::Error> errors;

#define ERR_CATCH(x) try{ x; } catch(std::runtime_error e) {errors.emplace_back(-1, e.what()); return -1;} // TODO custom exception with err code

extern "C" int init(Renderer::Configuration configuration, ResourceManager& resourceManager) {
    ERR_CATCH(new (&instance) vlk::Vulkan(*configuration.window));

    return 0;
}

extern "C" int update() {
    return 0;
}

extern "C" int cleanup() {
    ERR_CATCH(instance.cleanup());
    return 0;
}

extern "C" int setConfiguration(Renderer::Configuration configuration) {
    return 0;
}

extern "C" Renderer::Configuration getConfiguration() {
    return {};
}

extern "C" Renderer::Error getError(/*int?*/) {
    if(errors.empty())
        return {};

    Renderer::Error oldest = errors.front();
    errors.pop_front();
    return oldest;
}

extern "C" std::vector<Res::ResourceAny> loadResources(ResourceManager::LoadPacket loadInfo) {
    return instance.loadResources(loadInfo);
}

