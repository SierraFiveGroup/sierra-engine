#include "render_state_manager.hpp"

namespace Sierra {

    RenderStateManager::RenderStateManager(): nextPacketIndex() {

    }

    RenderStateManager::RenderStateManager(uint32_t framebufferCount): renderPackets(framebufferCount), nextPacketIndex() {

    }

    std::optional<RenderStateManager::RenderPacket*> RenderStateManager::getEmptyPacket() {
        std::pair<RenderPacket, std::atomic_bool>& packet = renderPackets[(++nextPacketIndex) % renderPackets.size()];

        if(packet.second) {
            if(!nextPacketIndex) nextPacketIndex = renderPackets.size() - 1;
            else nextPacketIndex--;

            return std::nullopt;
        }

        return std::optional<RenderPacket*>(&packet.first);
    }
    
    std::optional<RenderStateManager::RenderPacket*> RenderStateManager::getFullPacket() {
        int i = (nextPacketIndex + 1) % renderPackets.size();
        for(; i != nextPacketIndex || renderPackets[i].second; (++i) % renderPackets.size());

        std::pair<RenderPacket, std::atomic_bool>& packet = renderPackets[i];

        if(packet.second) {
            packet.second = false;
            return std::optional<RenderPacket*>(&packet.first);
        }

        return std::nullopt;
    }
}