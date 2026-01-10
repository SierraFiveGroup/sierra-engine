#pragma once

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <utility>
#include <span>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include "io/logging/logger.hpp"
#include "util/utilities.hpp"

//This mempool is not meant for efficient allocation, rather it is meant
//for unfragmented allocation. Do not use this allocator for heap that you
//require to be efficient with storage.
namespace Sierra::Memory {
	class Mempool {
		public:
			Mempool() = default;
			Mempool(uint32_t sz);
			DISABLE_COPY_AND_MOVE(Mempool);
			~Mempool();

			uint8_t* allocate(uint32_t sz);
			bool free(uint8_t* ptr, bool overwrite = false);
			
			//Technically, you can access/write to the allocated
			//memory manually, but you have to implement your own
			//guards for that. These handle it for you.
			std::span<uint8_t> access(uint8_t* ptr);
			bool write(uint8_t* ptr, const uint8_t* src, size_t sz);
		private:
			uint8_t *mPool;
			uint8_t *mAvailPtr;
			
			uint32_t mCurrentSize;
			uint32_t mMaxSize;

			//std::vector<std::pair<uint8_t*, uint8_t*>> mBoundPtrs;
			//std::vector<std::pair<uint8_t*, uint8_t*>> mFreedBounds;

			std::unordered_map<uint8_t*, uint8_t*> mBoundPtrs;
			std::unordered_map<uint8_t*, uint8_t*> mFreedBounds;
	};
}
