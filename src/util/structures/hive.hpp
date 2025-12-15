#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <unordered_map>
#include <limits>
#include <algorithm>


namespace Sierra {
    template<typename T>
    class Hive {
        struct Block{
            std::vector<uint8_t> skipField;
            bool firstEntrySkip;
            uint64_t id;

            T* objects;
        };

        public:
            Hive(): nextBlockID(0), blocks(), freeBlocks(), blockSize(32) {

            }

            Hive(uint8_t blockSize): nextBlockID(0), blocks(), freeBlocks(), blockSize(blockSize) {

            }

            uint64_t insert(T obj) {
                return insertPtr(&obj);
            }

            //uint64_t insert(T& obj) {
            //    return insertPtr(&obj);
            //}

            template<typename... Ts>
            void emplace(Ts... args);

            T& operator[](uint64_t id) {
                uint64_t blockID = id >> 16;
                uint16_t objIndex = id & (std::numeric_limits<uint16_t>::max());

                return blocks[blockID]->objects[objIndex];
            }
        private:

            uint64_t insertPtr(T* obj) {
                if(freeBlocks.empty()) allocBlock();

                if(*obj == 127) {
                    DBG("here we come");
                }

                Block& block = *freeBlocks.front();
                uint16_t objIdx = 0;
                if(block.firstEntrySkip) {
                    objIdx = block.skipField[0]-1;
                    block.objects[objIdx] = *obj;
                    block.skipField[0]--;
                    block.skipField[1]++;

                    if(block.skipField[0] <= 0) {
                        block.skipField[0] = block.skipField[1];
                        std::shift_left(block.skipField.begin() + 1, block.skipField.end(), 1); // potential optimization by not shifting ALL of them
                    }

                    return (block.id << 16) + objIdx;
                }
               
                objIdx = block.skipField[0];
                block.objects[objIdx] = *obj;
                block.skipField[0]++;
                block.skipField[1]--;

                if(block.skipField[0] <= 0) 
                    std::shift_left(block.skipField.begin() + 1, block.skipField.end(), 1);

                if(block.skipField[0] == blockSize)
                    freeBlocks.pop_front();
                
                return (block.id << 16) + objIdx;
            }

            void allocBlock() {
                Block* block = new Block;
                block->skipField.resize(128); //TEST WITH AND WITHOUT
                block->skipField[0] = blockSize;
                block->firstEntrySkip = true;
                block->objects = new T[blockSize];
                block->id = nextBlockID;

                blocks[nextBlockID] = block;
                freeBlocks.push_front(block);

                nextBlockID++;
            }

            std::unordered_map<uint64_t, Block*> blocks;
            std::list<Block*> freeBlocks;
            uint8_t blockSize;

            uint64_t nextBlockID;
    };
}