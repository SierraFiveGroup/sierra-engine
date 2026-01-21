#include "render_state_manager.hpp"

namespace Sierra {

    RenderStateManager::RenderStateManager(): nextPacketIndex() {

    }

    RenderStateManager::RenderStateManager(uint32_t framebufferCount): renderPackets(framebufferCount), nextPacketIndex() {

    }

    std::optional<RenderStateManager::RenderPacket*> RenderStateManager::getEmptyPacket() {
        nextPacketIndex = (nextPacketIndex + 1) % renderPackets.size();
       std::pair<RenderPacket, std::atomic_bool>& packet = renderPackets[nextPacketIndex];

        if(packet.second) {
            return std::nullopt;
        }

        return std::optional<RenderPacket*>(&packet.first);
    }
    
    std::optional<RenderStateManager::RenderPacket*> RenderStateManager::getFullPacket() {
        int i = (nextPacketIndex + 1) % renderPackets.size();
        for(; i != nextPacketIndex && renderPackets[i].second; i = (i + 1) % renderPackets.size());

        std::pair<RenderPacket, std::atomic_bool>& packet = renderPackets[i];

        if(!packet.second) {
            return std::nullopt;
        }

        nextPacketIndex = i;
        return std::optional<RenderPacket*>(&packet.first);

    }

    void RenderStateManager::releaseOwnership(RenderPacket* packet) {
        
        auto it = renderPackets.begin();
        for(; it != renderPackets.end(); it++) {
            std::pair<RenderPacket, std::atomic_bool>& currPacket = *it;
            if(&currPacket.first != packet) continue;

            currPacket.second = !currPacket.second;
            break;
        }

        if(it == renderPackets.end()) {
            throw new std::runtime_error("Failed to release the render packet: packet not found");
        }
    }
}