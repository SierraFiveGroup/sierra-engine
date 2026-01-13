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
                iterator(): blockIndex(), bWidth(), objectIndex(), blocks(), bCount(), oCount() {

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
                    return *(T*)&blocks[blockIndex].objects[objectIndex * bWidth];
                }

                T* operator->() {
                    return (T*)&blocks[blockIndex].objects[objectIndex * bWidth];
                }                

                iterator& operator++() {
                    if(blockIndex == bCount) return *this;
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
                    for(objectIndex--; objectIndex >= 0 && !blocks[blockIndex].skipField[objectIndex]; objectIndex--);

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

                iterator& operator+(int n) { // kind of shite but I do avoid checking it every iteration
                    if(n > 0) {
                        for(int i = 0; i < abs(n); i++) {
                            ++*this;
                        }
                        return *this;
                    }

                    for(int i = 0; i < abs(n); i++) {
                        --*this;
                    }

                    return *this;
                }

                iterator& operator-(int n) {
                    if(n < 0) {
                        for(int i = 0; i < abs(n); i++) {
                            ++*this;
                        }

                        return *this;
                    }

                    for(int i = 0; i < abs(n); i++) {
                        --*this;
                    }

                    return *this;
                }


                bool operator>(iterator& b) {
                    return len(*this) > len(b);
                }

                bool operator<(iterator& b) {
                    return len(*this) < len(b);
                }

                bool operator>=(iterator& b) {
                    return len(*this) >= len(b);
                }

                bool operator<=(iterator& b) {
                    return len(*this) <= len(b);
                }

                iterator& operator+=(int n) {
                    *this = *this + n;
                }

                iterator& operator-=(int n) {
                    *this = *this - n;
                }

                T* operator[](size_t offset) {
                    return *(*this + offset);
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
                uint32_t bWidth;
                uint32_t oCount;

                iterator(Hive& hive, uint32_t blockIndex, uint32_t objectIndex): blocks(hive.blocks.data()), objectIndex(objectIndex), blockIndex(blockIndex),
                 bCount(hive.blocks.size()), oCount(hive.blockSize), bWidth(hive.blockWidth) {
                }

                iterator(Block* blocks, uint32_t bCount, uint32_t oCount): blocks(blocks), bCount(bCount), oCount(oCount) {

                } // fix this constructor goofyness when did I make this bruh

                inline uint32_t len() {
                    return blockIndex * oCount + objectIndex;
                }
                
        };

        protected:

        uint32_t blockSize;
        uint32_t blockWidth;

        std::vector<uint8_t> bSkipField;
        uint32_t freeBlockCount;

        std::vector<Block> blocks;

        public:
            Hive(): blockSize(32), blockWidth(1), freeBlockCount(0), blocks(), bSkipField() {
                init();
            }

            Hive(uint32_t blockSize, uint32_t blockWidth): blockSize(blockSize), blockWidth(blockWidth), freeBlockCount(0), blocks(), bSkipField() {
                init();
            }

            T* insert(T&& obj) { 
                uint32_t blockIndex = getNextFreeBlock();
                return blockInsert(blockIndex, &obj);
            }

            void erase(T* obj) {
                iterator it = find(obj); // TODO optimize by looking at block addresses to find the right one
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
                return iterator(*this, blocks.size(), -1);
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
                blocks.back().objects.resize(blockSize * blockWidth);
                blocks.back().objCount = 0;
            }

            void removeBlock(uint32_t index) {
                blocks.erase(blocks.begin() + index);
                removeSkipEntry(bSkipField, index);
            }

            uint32_t getNextFreeBlock() {
                return bSkipField.front();
            }

            T* blockInsert(uint32_t blockIndex, T* obj) {
                uint32_t index = addNextSkipBlock(blocks[blockIndex].skipField);

                if(blocks[blockIndex].skipField[0] == blockSize) { // block is full
                    freeBlockCount--;
                    addNextSkipBlock(bSkipField);
                    addBlock();
                }

                blocks[blockIndex].objCount++;

                T* first = (T*)&blocks[blockIndex].objects[index * blockWidth];
                for(size_t i = 0; i < blockWidth; i++) {
                    if(obj)
                        new((T*)&blocks[blockIndex].objects[index * blockWidth + i]) T(*obj);
                    else
                        new((T*)&blocks[blockIndex].objects[index * blockWidth + i]) T(); // TODO is this if necessary / is there any case where we would want to add without explicitly passing in an object
                }
                return first;
            }

            bool eraseInternal(iterator it) {
                Block& block = blocks[it.blockIndex];

                if constexpr (std::is_destructible_v<T>) { 
                    for(size_t i = 0; i < blockWidth; i++)
                        ((T*)&block.objects[it.objectIndex * blockWidth + i])->~T();
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
