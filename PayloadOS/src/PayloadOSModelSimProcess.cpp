#include "PayloadOSModelSimProcess.h"
#include <Arduino.h>
#include <cmath>

using namespace PayloadOS;
using namespace Simulation;
using namespace Model;

RandomProcess::RandomProcess(float_t power, float_t durration) : period(durration){
    for(uint_t i=0; i<coefficients.size(); i++){
        int32_t randomNum = random();
        coefficients[i] = power*(2*(static_cast<float_t>(randomNum)/static_cast<float_t>(__INT32_MAX__))-1);
    }  
}

float_t RandomProcess::at(float_t time) const{
    float_t accum = 0;
    for(uint_t i = 0; i<coefficients.size(); i++){
        accum += coefficients[i] * std::sin((i+1)*PI*time/period);
    }
    return accum;
}

const float_t* RandomProcess::getCoefficients() const{
    return coefficients.data();
}

uint_t RandomProcess::order() const{
    return coefficients.size();
}