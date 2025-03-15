#include "PayloadOSVariance.h"
#include "PayloadOSPeripheralSelector.h"
#include <algorithm>
#include <Arduino.h>

using namespace PayloadOS;
using namespace FlightData;

RunningVariance::RunningVariance(uint_t size) : size(size), current(values.data()), filled(false){}

void RunningVariance::clear(){
    current = values.data();
    filled = false;
}

error_t RunningVariance::setSize(uint_t newSize){
    if(newSize <= values.size()){
        size = newSize;
        return PayloadOS::GOOD;
    } 
    return PayloadOS::ERROR;
}

uint_t RunningVariance::getSize() const{
    return size;
}

void RunningVariance::push(float_t value){
    *current = value;
    current++;
    if(current == values.data() + size){
        current = values.data();
        filled = true;
    } 
}

float_t RunningVariance::getVariance(uint_t numOutliers){
    uint_t end = size;
    if(!filled) end = static_cast<uint_t>(current - values.data());
    sort(0, end);
    if(2*numOutliers >= end) numOutliers = 0;
    float_t mean = 0;
    for(uint_t i=0+numOutliers; i<end - numOutliers; i++)
        mean += sorted[i];
    mean /= (end - 2*numOutliers);
    float_t var = 0;
    for(uint_t i=0+numOutliers; i<end-numOutliers; i++)
        var += (sorted[i] - mean)*(sorted[i] - mean);
    var /= (end - 2*numOutliers);
    return var;
}

float_t RunningVariance::getStandardDeviation(uint_t numOutliers){
    return std::sqrt(getVariance(numOutliers));
}

void RunningVariance::sort(uint_t start, uint_t end){
    std::copy(values.begin(), values.end(), sorted.begin());
    for(uint_t i=start; i<end; i++){
        uint_t smallest = i;
        for(uint_t j=i; j<end; j++)
            if(sorted[j] < sorted[smallest]) smallest = j;
        float_t temp = sorted[i];
        sorted[i] = sorted[smallest];
        sorted[smallest] = temp;
    }
}


//altimeter selectors
RunningVariance* AltimeterVariances::getAltimeter1(){
    static RunningVariance instance(1); 
    return &instance;
}

RunningVariance* AltimeterVariances::getAltimeter2(){
    static RunningVariance instance(1); 
    return &instance;
}

void AltimeterVariances::zeroAltimeter1(const Interpreter::Token* args){
    zero1 = Peripherals::PeripheralSelector::get()->getPayloadAltimeter()->getAltitude_ft();
}

uint_t AltimeterVariances::getAltimeter1Zero(){
    return zero1;
}

void AltimeterVariances::zeroAltimeter2(const Interpreter::Token*){
    zero2 = Peripherals::PeripheralSelector::get()->getLightAPRSAltimeter()->getAltitude_ft();
}

uint_t AltimeterVariances::getAltimeter2Zero(){
    return zero2;
}

float_t AltimeterVariances::zero1 = 0;
float_t AltimeterVariances::zero2 = 0;
