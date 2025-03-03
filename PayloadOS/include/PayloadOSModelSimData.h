#pragma once
#include "PayloadOSGeneral.h"

namespace PayloadOS{
    namespace Simulation{
        namespace Model{
            struct InternalData{
                float_t launchSiteTemp, initialPower, GPSFixPeriod, consumptionRate, MotorStrength, VisibleSatalites,initialLat, initialLon;
            };

            struct ModifiableData{
                float_t alt1Noise, alt2Noise, IMU0Noise, IMU1Noise, IMU2Noise, IMU3Noise, IMU4Noise, powerNoise;
                float_t initialAltitude, launchTimeAfterArm, launchRailAngle;
                float_t dragCoefficient, ejectionPower, ejectionDelay, ejectionT, landingDeltaT, drogueSpeed, mainSpeed, driftSpeed, oscilationPeriod, oscilationAmplitude;
            };

            class ModelSimData{
                const InternalData internal;
                ModifiableData external;
            public:
                const InternalData& getInternal() const;
                ModifiableData& getModifiable();
                void makeDefault();
            //singleton implementation
            private:
                ModelSimData();
                constexpr static InternalData defaultInternal();
                constexpr static ModifiableData defaultExternal();
            public:
                static ModelSimData* get();
                ModelSimData(const ModelSimData&) = delete;
                void operator=(const ModelSimData&) = delete;
            };
        }
    }
}
