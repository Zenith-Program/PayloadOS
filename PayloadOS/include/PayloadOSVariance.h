#pragma once
#include "PayloadOSGeneral.h"
#include <array>

namespace PayloadOS{
    namespace FlightData{
        class RunningVariance{
            std::array<float_t, PayloadOS_VarianceBufferSize> values, sorted;
            uint_t size;
            float_t* current;
            bool filled;
        public:
            RunningVariance(uint_t);
            void clear();
            error_t setSize(uint_t);
            uint_t getSize() const;
            void push(float_t);
            float_t getVariance(uint_t=0);
            float_t getStandardDeviation(uint_t=0);
        private:
            void sort(uint_t, uint_t);
        };

        class AltimeterVariances{
        public:
            static RunningVariance* getAltimeter1();
            static RunningVariance* getAltimeter2();
        };
    }
}