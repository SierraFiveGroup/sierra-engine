#include "delta_time.hpp"

namespace Sierra {

    DeltaTime::DeltaTime(): mLast(0), mDt(0) {

    }

    void DeltaTime::update() {
        if (!mLast)
            mLast = CLOCK_NS;

        mDt = (CLOCK_NS - mLast) / 1000000000.0f;

        mDt = std::min(mDt, MAX_DT);

        mLast = CLOCK_NS;
    }

    float DeltaTime::get() {
        return mDt;
    }

}