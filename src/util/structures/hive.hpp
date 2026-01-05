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
            std::vector<T> objects;

        };

        uint32_t blockSize;

        std::vector<uint8_t> bSkipField;
        std::vector<Block> blocks;

        public:
            Hive(): blockSize(32) {
                init();
            }

            Hive(uint8_t blockSize): blockSize(blockSize) {
                init();
            }

            T* insert(T obj) { 
                Block* block = getNextFreeBlock();
            }

        private:
            void init() {
                addBlock();
                bSkipField.push_back(0);
            }

            void addBlock() {
                blocks.emplace_back();
                blocks.back().skipField.resize(blockSize);
                blocks.back().objects.resize(blockSize);
            }

            Block* getNextFreeBlock() {
                return &blocks[bSkipField.begin()];
            }

            T* insertBlock(Block* block) {
                Block* block = getNextFreeBlock();
                uint32_t index = addNextSkipBlock(block);

                return block->objects[index];
            }

            uint32_t addNextSkipBlock(std::vector<uint8_t>& skipField) {
                uint32_t insertIndex = skipField[0];
                uint32_t leftNode = !insertIndex ? 0 : skipField[0];
                uint32_t rightNode = (insertIndex + 1 >= skipField.size()) ? 0 : skipField[insertIndex+1];

                if(insertIndex == skipField.size()) return insertIndex;

                if(!leftNode && !rightNode) {
                    skipField[insertIndex] = 1;
                }
                else if(leftNode && !rightNode) {
                    skipField[0]++;
                    skipField[insertIndex] = skipField[0];
                }
                else if(!leftNode && rightNode) {
                    skipField[insertIndex+1]++;
                    skipField[insertIndex] = skipField[insertIndex+1];
                } else {
                    skipField[0] += skipField[insertIndex+1] + 1;
                    skipField[insertIndex+1] = skipField[0];
                }

                return insertIndex;
            }
    };
}