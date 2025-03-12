#pragma once
#include "PayloadOSGeneral.h"
#include <array>

namespace PayloadOS{
    namespace Simulation{
        namespace Model{
            class RandomProcess{
                std::array<float_t, PayloadOS_ModelSim_ProcessOrder> coefficients;
                float_t period;
            public:
                RandomProcess(float_t, float_t);
                float_t at(float_t) const;
                const float_t* getCoefficients() const;
                uint_t order() const;
            };
        }
    }
}