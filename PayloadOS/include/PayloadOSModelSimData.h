#pragma once
#include "PayloadOSGeneral.h"
#include <array>

namespace PayloadOS{
    namespace Simulation{
        namespace Model{
            struct Parameter{
                const char* name;
                const bool positive;
                const float_t defaultValue;
                float_t value;
                const char* units;
            };
            enum class ModifiableDataNames{
                launchSiteTemp, initialPower, GPSFixPeriod, consumptionRate, MotorStrength, VisibleSatalites,initialLat, initialLon, 
                alt1Noise, alt2Noise, IMU0Noise, IMU1Noise, IMU2Noise, IMU3Noise, IMU4Noise, powerNoise, initialAltitude, 
                launchTimeAfterArm, launchRailAngle, dragCoefficient, ejectionPower, ejectionDelay, ejectionT, landingDeltaT, 
                drogueSpeed, mainSpeed, driftSpeed, oscilationPeriod, oscilationAmplitude, 
                GPSAltitudeNoise, GPSPositionNoise, Temperature1Noise, Temperature2Noise,
                LinearPowerRatio, AngularPowerRatio, GravityPowerRatio, AltitudePowerRatio, motorBurnTime, SENTINAL_COUNT
            };

            #define PayloadOS_NumberOfModifiables static_cast<std::size_t>(ModifiableDataNames::SENTINAL_COUNT)
            using  ModifiableData = std::array<Parameter, PayloadOS_NumberOfModifiables>; 

            class ModelSimData{
                ModifiableData external;
            public:
                void makeDefault();
                void printData() const;
                Parameter* getDataWithName(const char*);
                const Parameter* getDataWithName(const char*) const;
                Parameter* getData(ModifiableDataNames);
                const Parameter* getData(ModifiableDataNames) const;
            private:
            //helpers
                static int_t getIndex(ModifiableDataNames);
            //singleton implementation
            private:
                ModelSimData();
                constexpr static ModifiableData defaultExternal();
            public:
                static ModelSimData* get();
                ModelSimData(const ModelSimData&) = delete;
                void operator=(const ModelSimData&) = delete;
            };
        }
    }
}
