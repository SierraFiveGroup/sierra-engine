#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <cmath>


//iterators will only stay valid on insertion, not erasure
//but iterator isnt guaranteed to iterate through newly inserted elements
//idk for erasure Id have to think about it actually lol
//maybe i could make it but so far it isnt insertion or erasure safe
namespace Sierra {
    template<typename T>
    class Hive {
        struct OpaqueT {
            uint8_t buf[sizeof(T)]; // this is done so vector can be used and indexed normally without auto initialization of it's contents, we create a "dummy" obj of the same size 
        };

        struct Block {
            std::vector<uint8_t> skipField;
            std::vector<OpaqueT> objects; 

            uint32_t objCount; // to more easily check whether we need to delete it
        };

        public:
        class iterator { 
            friend class Sierra::Hive<T>;
            public:
                iterator(): blockIndex(), objectIndex(), blocks(), bCount(), oCount() {

                }

                iterator(Hive& hive) {
                    iterator(hive, 0, 0);
                }

                bool operator==(const iterator& other) {
                    return blockIndex == other.blockIndex &&
                        objectIndex == other.objectIndex &&
                        blocks == other.blocks;
                }

                bool operator!=(const iterator& other) {
                    return !(*this == other);
                }

                T& operator*() {
                    return *(T*)&blocks[blockIndex]->objects[objectIndex];
                }

                T* operator->() {
                    return (T*)&blocks[blockIndex]->objects[objectIndex];
                }                

                iterator& operator++() {
                    if(blockIndex == bCount) return *this;
                    std::cout << objectIndex << " " << oCount << " " << blockIndex << " " << bCount << "hello\n";
                    for(objectIndex++; objectIndex < oCount && !blocks[blockIndex].skipField[objectIndex]; objectIndex++);

                    if(objectIndex >= oCount) {
                        blockIndex++;
                        objectIndex = -1; 

                        ++*this;

                        return *this;
                    }

                    return *this;
                }

                iterator& operator++(int) {
                    auto& old = *this;
                    ++*this;
                    return old;
                }

                iterator& operator--() {
                    if(!blockIndex) return *this;
                    for(objectIndex--; objectIndex >= 0 && !blocks[blockIndex]->skipField[objectIndex]; objectIndex--);

                    if((objectIndex - 1) < 0) {
                        objectIndex = oCount - 1;
                        blockIndex--;
                        --*this;
                        return *this;
                    }

                    return *this;
                }

                iterator& operator--(int) {
                    auto& old = *this;
                    ++*this;
                    return old;
                }

                friend void swap(iterator& a, iterator& b) {
                    std::swap(a.blockIndex, b.blockIndex);
                    std::swap(a.objectIndex, b.objectIndex);
                    std::swap(a.blocks, b.blocks);
                    std::swap(a.bCount, b.bCount);
                    std::swap(a.oCount, b.oCount);
                }
            protected:
                uint32_t blockIndex, objectIndex;
                Block* blocks;
                uint32_t bCount;
                uint32_t oCount;

                iterator(Hive& hive, uint32_t blockIndex, uint32_t objectIndex): blocks(hive.blocks.data()), objectIndex(objectIndex), blockIndex(blockIndex),
                 bCount(hive.blocks.size()), oCount(hive.blockSize) {
                }

                iterator(Block* blocks, uint32_t bCount, uint32_t oCount): blocks(blocks), bCount(bCount), oCount(oCount) {

                }
                
        };

        protected:

        uint32_t blockSize;

        std::vector<uint8_t> bSkipField;
        uint32_t freeBlockCount;

        std::vector<Block> blocks;

        public:
            Hive(): blockSize(32), freeBlockCount(0), blocks(), bSkipField() {
                init();
            }

            Hive(uint8_t blockSize): blockSize(blockSize), freeBlockCount(0), blocks(), bSkipField() {
                init();
            }

            T* insert(T obj) { 
                uint32_t blockIndex = getNextFreeBlock();
                return blockInsert(blockIndex);
            }

            void erase(T* obj) {
                iterator it = find(obj);
                eraseInternal(it);
            }

            iterator erase(iterator it) {
                bool blockErased = eraseInternal(it);

                if(blockErased) {
                    it.blocks = blocks.data();
                    it.bCount = blocks.size();

                    if(it.blockIndex >= blocks.size()) {
                        it.blockIndex--;
                        it.objectIndex = -1; // basically set it before the start of the previous block so when we iterate on return it goes to 0
                        //also adjust the number of blocks as we removed one ofc
                    }
                }

                return ++it;
            }

            iterator begin() {
                return ++iterator(*this, 0, -1);
            }

            iterator end() {
                return iterator(*this, blocks.size(), 0);
            }

            iterator find(T* obj) {
                iterator it = begin();
                while(it != end() && &(*it) != obj)
                    it++;

                return it;
            }
        private:
            void init() {
                addBlock();
            }

            void addBlock() {
                freeBlockCount++;
                blocks.emplace_back();
                bSkipField.push_back(0);

                blocks.back().skipField.resize(blockSize);
                blocks.back().objects.resize(blockSize);
                blocks.back().objCount = 0;
            }

            void removeBlock(uint32_t index) {
                blocks.erase(blocks.begin() + index);
                removeSkipEntry(bSkipField, index);
            }

            uint32_t getNextFreeBlock() {
                return bSkipField.front();
            }

            T* blockInsert(uint32_t blockIndex) {
                uint32_t index = addNextSkipBlock(blocks[blockIndex].skipField);

                if(blocks[blockIndex].skipField[0] == blockSize) { // block is full
                    freeBlockCount--;
                    addNextSkipBlock(bSkipField);
                    addBlock();
                }

                blocks[blockIndex].objCount++;
                return new((T*)&blocks[blockIndex].objects[index]) T();
            }

            bool eraseInternal(iterator it) {
                std::cerr << it.blockIndex << " " << it.bCount << " " << it.objectIndex << " " << it.oCount << "\n";
                std::cerr << blocks.size() << "\n";
                Block& block = blocks[it.blockIndex];

                if constexpr (std::is_destructible_v<T>) { 
                    ((T*)&block.objects[it.objectIndex])->~T();
                }
                
                removeSkipEntry(block.skipField, it.objectIndex);
                block.objCount--;

                if(block.objCount <= 0 && blocks.size() > 1) { // we have to assume there is always at least one spare block present when we insert
                    removeBlock(it.blockIndex);
                    return true;
                }

                return false;
            }

            uint32_t addNextSkipBlock(std::vector<uint8_t>& skipField) {
                uint32_t insertIndex = skipField[0];
                uint32_t leftNode = insertIndex ? skipField[0] : 0;
                uint32_t rightNode = (insertIndex + 1 >= skipField.size()) ? 0 : skipField[insertIndex+1]; // these basically booleans determine whether its skipfield adjacent

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
                    skipField[insertIndex] = 1; //ensure its never 0 to allow for checks during iteration
                }

                return insertIndex;
            }
            
            void removeSkipEntry(std::vector<uint8_t>& skipField, uint32_t index) {
                int i = 0; // index is 8 sometimess
                for(i = index; i >= 0 && skipField[i]; i--);
                //i = std::max(i, 0); // fix in iterator too and is this even right?
                i++;

                uint32_t skipBlockSize = skipField[i];


                if(skipBlockSize == 1) { // easy case, it's just alone
                    // how do I make dis nice without shitting on the performance and having an empty if
                } else if(!index || !skipField[index-1]) { // left boundary
                    skipField[index+1] = skipField[index] - 1;
                    skipField[index] = 0;
                } else if(index == (blockSize - 1) || !skipField[index+1]) {// right boundary
                    skipField[index-1] = skipField[index] - 1;
                    skipField[index] = 0;
                } else { // in between
                    skipField[i + skipBlockSize - 1] = skipField[index + 1] = (i + skipBlockSize - 1) - index;
                    skipField[i] = skipField[index - 1] = index - i;
                }
                skipField[index] = 0; 
            }

    };
}
