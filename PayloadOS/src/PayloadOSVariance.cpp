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
    std::copy(values.begin()+start, values.begin()+end, sorted.begin());
    for(uint_t i=start; i<end; i++){
        uint_t smallest = i;
        for(uint_t j=i; j<end; j++)
            if(sorted[j] < sorted[smallest]) smallest = j;
        float_t temp = sorted[i];
        sorted[i] = sorted[smallest];
        sorted[smallest] = temp;
    }
}


float_t RunningVariance::getIdentityCovariance(uint_t numberOfOutliers){
    uint_t windowSize = size;
    if(!filled){
        windowSize = static_cast<uint_t>(current - values.data());
        numberOfOutliers = std::min(numberOfOutliers, windowSize/4); //at most 1/4 of data can be rejected
    }
    if(windowSize < 2) return 0;
    float_t identityVariance = (windowSize-1)*(windowSize+1)/12.0; //variance of identity function
    float_t identityMean = (windowSize-1)/2;
    sortDifferenceFromMean(0,windowSize);
    float_t dataMean = 0;
    for(float_t* currentValue = values.data(); currentValue < values.data() + windowSize; currentValue++){
        uint_t j;
        for(j=0; j<numberOfOutliers && *currentValue != sorted[j]; j++);
        if(j < numberOfOutliers) dataMean += interpolate(currentValue);
        else dataMean += *currentValue;
    }
    dataMean /= windowSize;
    float_t covariance = 0;
    for(uint_t n = 0; n<windowSize; n++){
        float_t usedValue = *circularAt(n);
        uint_t j;
        for(j=0; j<numberOfOutliers && usedValue != sorted[j]; j++);
        if(j < numberOfOutliers) usedValue = interpolate(circularAt(n));
        covariance += (usedValue - dataMean) * (n-identityMean);
    }
    covariance /= windowSize;
    return covariance/identityVariance;
}

void RunningVariance::sortDifferenceFromMean(uint_t start, uint_t end){
    std::copy(values.begin()+start, values.begin()+end, sorted.begin());
    float_t mean = 0;
    for(uint_t i=start; i<end; i++)
        mean += values[i];
    mean /= (end - start);

    for(uint_t i=start; i<end; i++){
        uint_t largest = i;
        for(uint_t j=i; j<end; j++)
            if(std::abs(sorted[j]-mean) > std::abs(sorted[largest]-mean)) largest = j;
        float_t temp = sorted[i];
        sorted[i] = sorted[largest];
        sorted[largest] = temp;
    }
}

const float_t* RunningVariance::circularAt(uint_t index) const{
    index = index%size;
    if(!filled) return values.data() + index;
    uint_t zeroIndex = static_cast<uint_t>(current - values.data());
    uint_t circularIndex = (zeroIndex + index)%size;
    return values.data() + circularIndex;
}

float_t RunningVariance::interpolate(const float_t* value) const{
    if(!filled && value == values.data()) return values[1];
    if(value == circularAt(circularIndex(current)-1)) return *circularAt(circularIndex(current)-2);
    if(value == current) return *circularAt(circularIndex(current)+1);
    return (*circularAt(circularIndex(value)+size+1) + *circularAt(circularIndex(value)+size-1))/2;
}

uint_t RunningVariance::circularIndex(const float_t* value) const{
    if(!filled) return static_cast<uint_t>(value-values.data());
    uint_t zeroIndex = static_cast<uint_t>(current - values.data());
    uint_t  absIndex = static_cast<uint_t>(value-values.data());
    return (size + absIndex - zeroIndex)%size;
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

void AltimeterVariances::zeroAltimeter1(const Interpreter::Token*){
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
