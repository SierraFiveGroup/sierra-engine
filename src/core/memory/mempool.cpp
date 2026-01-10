#include "mempool.hpp"

namespace Sierra::Memory {
	Mempool::Mempool(uint32_t sz) {
		mPool = new uint8_t[sz];
		mCurrentSize = 0;
		mMaxSize = sz;
	}

	Mempool::~Mempool() {
		if (mPool) delete[] mPool;
	}

	uint8_t* Mempool::allocate(uint32_t sz) {
		//Check if we can reuse a bound
		for (auto it = mFreedBounds.begin(); it != mFreedBounds.end(); ++it) {
			if (it->second - it->first >= sz) {
				//We can reuse
				auto sec = it->first + sz;
				mBoundPtrs[it->first] = sec;
				mFreedBounds.erase(it);
				return it->first;
			}
		}

		
		if (mCurrentSize + sz > mMaxSize) {
			ERROR("This allocation would exceed the max mempool size");
			return nullptr;
		}

		uint8_t *setPtr = mAvailPtr;
		mAvailPtr += sz;
		
		mBoundPtrs[setPtr] = mAvailPtr;
		return setPtr;
	}

	bool Mempool::free(uint8_t* ptr, bool overwrite) {
		if (mBoundPtrs.find(ptr) != mBoundPtrs.end()) {
			mFreedBounds[ptr] = mBoundPtrs[ptr];
			mBoundPtrs.erase(ptr);

			if (overwrite) {
				size_t len = ptr - mFreedBounds[ptr];
				std::memset(ptr, 0x00, len);
			}

			//Combine multiple unallocated (freed) bounds into one (if sequential)
			for (const auto & [ key, value ] : mFreedBounds) {
				if (value == ptr - 1) { // -1 because previous address (I think)
					mFreedBounds[key] = mFreedBounds[ptr];
					mFreedBounds.erase(ptr); //Get rid of the ref
					break;
				}

				if (key - 1 == mFreedBounds[ptr]) { //Next free
					mFreedBounds[ptr] = mFreedBounds[key];
					break;
				}
			}

			return true;
		} else {
			WARN("Pointer not found in allocations");
			return false;
		}
	}

	std::span<uint8_t> Mempool::access(uint8_t* ptr) {
		if (mBoundPtrs.find(ptr) != mBoundPtrs.end()) {
			return std::span<uint8_t>(ptr, mBoundPtrs[ptr]);
		} else {
			WARN("Pointer not found in allocations");
			return std::span<uint8_t>(); //Return empty span
		}
	}

	bool Mempool::write(uint8_t* ptr, const uint8_t* src, size_t sz) {
		if (!ptr || !src || mBoundPtrs[ptr] - ptr > sz) return false;
		
		if (mBoundPtrs.find(ptr) != mBoundPtrs.end()) {
			size_t maxWritable = mBoundPtrs[ptr] - ptr;
			size_t copySize = std::min(sz, maxWritable);

			std::memcpy(ptr, src, copySize);
			return true;
		}

		return false;
	}
}
