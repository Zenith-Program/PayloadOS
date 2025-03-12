#include "PayloadOSModelSimStates.h"
#include "PayloadOSModelSimData.h"
#include <Arduino.h>

using namespace PayloadOS;
using namespace Simulation;
using namespace Model;

#define FT_TO_M(x) 0.3048*x
#define F_TO_K(x) (x-32.0)*5/9.0 + 273.15
#define EMPTY_VECTOR {0,0,0}
#define DEG_TO_RAD(x) PI*x/180
//helpers----------------------------------------------------------------------
void SimState::addNoise(UpdateParams& parameters){
    parameters.pressure = altitude_mToPressure_mBar(parameters.altitude + getNoise(ModelSimData::get()->getData(ModifiableDataNames::alt1Noise)->value));
    parameters.pressure2 = altitude_mToPressure_mBar(parameters.altitude2 + getNoise(ModelSimData::get()->getData(ModifiableDataNames::alt2Noise)->value));
    parameters.altitude += getNoise(ModelSimData::get()->getData(ModifiableDataNames::alt1Noise)->value);
    parameters.altitude2 += getNoise(ModelSimData::get()->getData(ModifiableDataNames::alt2Noise)->value);
    parameters.gps.altitude += getNoise(ModelSimData::get()->getData(ModifiableDataNames::GPSAltitudeNoise)->value);
    parameters.gps.position.x += getNoise(ModelSimData::get()->getData(ModifiableDataNames::GPSPositionNoise)->value);
    parameters.gps.position.y += getNoise(ModelSimData::get()->getData(ModifiableDataNames::GPSPositionNoise)->value);
    parameters.voltage += getNoise(ModelSimData::get()->getData(ModifiableDataNames::powerNoise)->value);
    parameters.temp += getNoise(ModelSimData::get()->getData(ModifiableDataNames::Temperature1Noise)->value);
    parameters.temp2 += getNoise(ModelSimData::get()->getData(ModifiableDataNames::Temperature2Noise)->value);
    //IMU
    addNoiseToVector(parameters.acceleration0, ModelSimData::get()->getData(ModifiableDataNames::IMU0Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::LinearPowerRatio)->value);
    addNoiseToVector(parameters.angularVelocity0, ModelSimData::get()->getData(ModifiableDataNames::IMU0Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::AngularPowerRatio)->value);
    addNoiseToVector(parameters.gravity0, ModelSimData::get()->getData(ModifiableDataNames::IMU0Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::GravityPowerRatio)->value);

    addNoiseToVector(parameters.acceleration1, ModelSimData::get()->getData(ModifiableDataNames::IMU1Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::LinearPowerRatio)->value);
    addNoiseToVector(parameters.angularVelocity1, ModelSimData::get()->getData(ModifiableDataNames::IMU1Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::AngularPowerRatio)->value);
    addNoiseToVector(parameters.gravity1, ModelSimData::get()->getData(ModifiableDataNames::IMU1Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::GravityPowerRatio)->value);

    addNoiseToVector(parameters.acceleration2, ModelSimData::get()->getData(ModifiableDataNames::IMU2Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::LinearPowerRatio)->value);
    addNoiseToVector(parameters.angularVelocity2, ModelSimData::get()->getData(ModifiableDataNames::IMU2Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::AngularPowerRatio)->value);
    addNoiseToVector(parameters.gravity2, ModelSimData::get()->getData(ModifiableDataNames::IMU2Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::GravityPowerRatio)->value);

    addNoiseToVector(parameters.acceleration3, ModelSimData::get()->getData(ModifiableDataNames::IMU3Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::LinearPowerRatio)->value);
    addNoiseToVector(parameters.angularVelocity3, ModelSimData::get()->getData(ModifiableDataNames::IMU3Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::AngularPowerRatio)->value);
    addNoiseToVector(parameters.gravity3, ModelSimData::get()->getData(ModifiableDataNames::IMU3Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::GravityPowerRatio)->value);

    addNoiseToVector(parameters.acceleration4, ModelSimData::get()->getData(ModifiableDataNames::IMU4Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::LinearPowerRatio)->value);
    addNoiseToVector(parameters.angularVelocity4, ModelSimData::get()->getData(ModifiableDataNames::IMU4Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::AngularPowerRatio)->value);
    addNoiseToVector(parameters.gravity4, ModelSimData::get()->getData(ModifiableDataNames::IMU4Noise)->value * ModelSimData::get()->getData(ModifiableDataNames::GravityPowerRatio)->value);
}

UpdateParams SimState::getBaseParams(){
    UpdateParams parameters;
    parameters.altitude = FT_TO_M(ModelSimData::get()->getData(ModifiableDataNames::initialAltitude)->value);
    parameters.altitude2 = parameters.altitude;
    parameters.voltage = ModelSimData::get()->getData(ModifiableDataNames::initialPower)->value;
    parameters.temp = F_TO_K(ModelSimData::get()->getData(ModifiableDataNames::launchSiteTemp)->value);
    parameters.pressure = altitude_mToPressure_mBar(parameters.altitude);
    parameters.pressure2 = altitude_mToPressure_mBar(parameters.altitude);
    Peripherals::GPSData gps;
    gps.altitude = parameters.altitude;
    gps.satalites = ModelSimData::get()->getData(ModifiableDataNames::VisibleSatalites)->value;
    gps.fixAge = ModelSimData::get()->getData(ModifiableDataNames::GPSFixPeriod)->value;
    gps.position = {ModelSimData::get()->getData(ModifiableDataNames::initialLat)->value, ModelSimData::get()->getData(ModifiableDataNames::initialLon)->value};
    parameters.gps = gps;
    parameters.acceleration0 = EMPTY_VECTOR;
    parameters.acceleration1 = EMPTY_VECTOR;
    parameters.acceleration2 = EMPTY_VECTOR;
    parameters.acceleration3 = EMPTY_VECTOR;
    parameters.acceleration4 = EMPTY_VECTOR;
    parameters.angularVelocity0 = EMPTY_VECTOR;
    parameters.angularVelocity1 = EMPTY_VECTOR;
    parameters.angularVelocity2 = EMPTY_VECTOR;
    parameters.angularVelocity3 = EMPTY_VECTOR;
    parameters.angularVelocity4 = EMPTY_VECTOR;
    parameters.gravity0 = {0,-9.8,0};
    parameters.gravity1 = {0,-9.8,0};
    parameters.gravity2 = {0,-9.8,0};
    parameters.gravity3 = {0,-9.8,0};
    parameters.gravity4 = {0,-9.8,0};
    parameters.arm = true;
    parameters.change = false;
    return parameters;
}

float_t SimState::altitude_mToPressure_mBar(float_t altitude){
    const float_t P_0 = 1013.25;
    const float_t L = 0.0065;
    const float_t T_0 = 288.15;
    const float_t g = 9.8;
    const float_t M = 0.0289644;
    const float_t R = 8.31432;
    const float_t a = g*M/R/L;
    return P_0 * std::pow(1-(L*altitude/T_0), a);
}
float_t SimState::getNoise(float_t power){
    const uint_t range = 0xFFFFFF;
    int32_t randomNum = random(range);
    return power*(2*(static_cast<float_t>(randomNum)/static_cast<float_t>(range))-1);
}
void SimState::addNoiseToVector(Peripherals::LinearVector& v, float_t power){
    v.x += getNoise(power);
    v.y += getNoise(power);
    v.y += getNoise(power);
}
void SimState::addNoiseToVector(Peripherals::RotationVector& v, float_t power){
    v.x_rot += getNoise(power);
    v.y_rot += getNoise(power);
    v.z_rot += getNoise(power);
}

void SimState::setIMUsOrientation(UpdateParams& parameters, Peripherals::LinearVector vector){
    parameters.gravity0 = vector;
    parameters.gravity1 = vector;
    parameters.gravity2 = vector;
    parameters.gravity3 = vector;
    parameters.gravity4 = vector;
}

void SimState::setIMUsAcceleration(UpdateParams& parameters, Peripherals::LinearVector vector){
    parameters.acceleration0 = vector;
    parameters.acceleration1 = vector;
    parameters.acceleration2 = vector;
    parameters.acceleration3 = vector;
    parameters.acceleration4 = vector;
}

void SimState::setIMUsAngularVelocity(UpdateParams& parameters, Peripherals::RotationVector vector){
    parameters.angularVelocity0 = vector;
    parameters.angularVelocity1 = vector;
    parameters.angularVelocity2 = vector;
    parameters.angularVelocity3 = vector;
    parameters.angularVelocity4 = vector;
}

void SimState::setAltitudesAndPressures(UpdateParams& parameters, float_t alt){
    parameters.altitude = alt;
    parameters.altitude2 = alt;
    parameters.pressure = altitude_mToPressure_mBar(alt);
    parameters.pressure2 = altitude_mToPressure_mBar(alt);
}

//state implementations--------------------------------------------------------
//inactive-------------------------------------
Inactive::Inactive(){}

Inactive* Inactive::get(){
    static Inactive instance;
    return &instance;
}

void Inactive::init(){}

UpdateParams Inactive::update(){
    UpdateParams parameters;
    parameters.change = false;
    return parameters;
}

void Inactive::end(){}
//Delay----------------------------------------
Delay::Delay(){}

Delay* Delay::get(){
    static Delay instance;
    return &instance;
}

void Delay::init(){
    Serial.print("Starting simulation, Launching in ");
    Serial.print(ModelSimData::get()->getData(ModifiableDataNames::launchTimeAfterArm)->value);
    Serial.println( "seconds");
    wait_ms = millis();
}

UpdateParams Delay::update(){
    UpdateParams parameters = getBaseParams();
    addNoise(parameters);
    if(millis() - wait_ms >= ModelSimData::get()->getData(ModifiableDataNames::launchTimeAfterArm)->value * 1000) parameters.change = true;
    else parameters.change = false;
    return parameters;
}

void Delay::end(){}
//Boost----------------------------------------
Boost::Boost(){}

Boost* Boost::get(){
    static Boost instance;
    return &instance;
}

void Boost::init(){
    Serial.println("[modelSim]Launched");
    wait_ms = millis();
}

UpdateParams Boost::update(){
    UpdateParams parameters = getBaseParams();
    uint_t elapsedTime_ms = millis() - wait_ms;
    if(elapsedTime_ms >= ModelSimData::get()->getData(ModifiableDataNames::motorBurnTime)->value * 1000) parameters.change = true;
    else parameters.change = false;
    Peripherals::LinearVector unitDirection = {std::sin(ModelSimData::get()->getData(ModifiableDataNames::launchRailAngle)->value), 0, std::cos(ModelSimData::get()->getData(ModifiableDataNames::launchRailAngle)->value)};
    setIMUsOrientation(parameters, {-unitDirection.x, -unitDirection.y, unitDirection.z});
    float_t acceleration = FT_TO_M(ModelSimData::get()->getData(ModifiableDataNames::MotorStrength)->value);
    setIMUsAcceleration(parameters, {acceleration * unitDirection.x, acceleration * unitDirection.y, acceleration * unitDirection.z});
    setAltitudesAndPressures(parameters, FT_TO_M( acceleration * (elapsedTime_ms/1000.0) * (elapsedTime_ms/1000.0) * std::cos(DEG_TO_RAD(ModelSimData::get()->getData(ModifiableDataNames::launchRailAngle)->value)) + ModelSimData::get()->getData(ModifiableDataNames::initialAltitude)->value));
    parameters.voltage -= elapsedTime_ms / 1000.0 * ModelSimData::get()->getData(ModifiableDataNames::consumptionRate)->value;
    addNoise(parameters);
    return parameters;
}

void Boost::end(){
}
//Coast----------------------------------------
Coast::Coast(){

}

Coast* Coast::get(){
    static Coast instance;
    return &instance;
}

void Coast::init(){
    Serial.println("[modelSim]Burnout");
}

UpdateParams Coast::update(){
    return {0};
}

void Coast::end(){
    
}
//Apogee---------------------------------------
Apogee::Apogee(){

}

Apogee* Apogee::get(){
    static Apogee instance;
    return & instance;
}

void Apogee::init(){
    
}

UpdateParams Apogee::update(){
    return {0};
}

void Apogee::end(){
    
}
//Drogue---------------------------------------
Drogue::Drogue(){

}

Drogue* Drogue::get(){
    static Drogue instance;
    return &instance;
}

void Drogue::init(){
    
}

UpdateParams Drogue::update(){
    return {0};
}

void Drogue::end(){
    
}
//Deploy---------------------------------------
Deploy::Deploy(){

}

Deploy* Deploy::get(){
    static Deploy instance;
    return &instance;
}

void Deploy::init(){
    
}

UpdateParams Deploy::update(){
    return {0};
}

void Deploy::end(){
    
}
//Main-----------------------------------------
Main::Main(){

}

Main* Main::get(){
    static Main instance;
    return &instance;
}

void Main::init(){
    
}

UpdateParams Main::update(){
    return {0};
}

void Main::end(){
    
}
//Landing--------------------------------------
Landing::Landing(){

}

Landing* Landing::get(){
    static Landing instance;
    return &instance;
}

void Landing::init(){
    
}

UpdateParams Landing::update(){
    return {0};
}

void Landing::end(){
    
}
//Landed---------------------------------------
Landed::Landed(){

}


Landed* Landed::get(){
    static Landed instance;
    return &instance;
}

void Landed::init(){
    
}

UpdateParams Landed::update(){
    return {0};
}

void Landed::end(){
    
}




