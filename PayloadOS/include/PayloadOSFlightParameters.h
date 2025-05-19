#pragma once
#include <array>
#include "PayloadOSGeneral.h"
#include "PayloadOSConsoleInterpreter.h"

namespace PayloadOS{
    namespace FlightData{
        struct FlightParameter{
            const char* name;
            const float_t defaultValue;
            float_t value;
            const char* unit;
        };

        enum class FlightParameterNames{
            CovarianceWindowSize, UpwardMotionThreshold, DownwardMotionThreshold, MinimumAscentTime, MinimumDescentTime, MinimumLandingTime, OutlierCount, NumTransmissions, SENTINAL_COUNT
        };

        #define PayloadOS_NumberOfFlightParameters static_cast<uint_t>(FlightParameterNames::SENTINAL_COUNT)
        using ParameterData = std::array<FlightParameter, PayloadOS_NumberOfFlightParameters>;

        class FlightParameters{
            ParameterData data;
        public:
            void makeDefault();
            void printData() const;
            FlightParameter* getDataWithName(const char*);
            const FlightParameter* getDataWithName(const char*) const;
            FlightParameter* getData(FlightParameterNames);
            const FlightParameter* getData(FlightParameterNames) const;
            //commands
            static void print_c(const Interpreter::Token*);
            static void setParam_c(const Interpreter::Token*);
        private:
            //helpers
            static int_t getIndex(FlightParameterNames);
            static constexpr ParameterData defaultInit();
            //sningleton implementation
        private:
            FlightParameters();
        public:
            static FlightParameters* get();
            FlightParameters(const FlightParameters&) = delete;
            void operator=(const FlightParameters&) = delete;
        };
    }
}