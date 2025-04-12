#pragma once
#include "PayloadOSGeneral.h"
#include "PayloadOSConsoleInterpreter.h"
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
            float_t getIdentityCovariance(uint_t=0);
        private:
            void sort(uint_t, uint_t);
            void sortDifferenceFromMean(uint_t, uint_t);
            const float_t* circularAt(uint_t) const;
            uint_t circularIndex(const float_t*) const;
            float_t interpolate(const float_t*) const;
        };

        class AltimeterVariances{
            static float_t zero1, zero2;
        public:
            static RunningVariance* getAltimeter1();
            static void setAltimeter1Zero(float_t);
            static void zeroAltimeter1(const Interpreter::Token*);
            static uint_t getAltimeter1Zero();

            static RunningVariance* getAltimeter2();
            static void setAltimeter2Zero(float_t);
            static void zeroAltimeter2(const Interpreter::Token*);
            static uint_t getAltimeter2Zero();
        };
    }
}