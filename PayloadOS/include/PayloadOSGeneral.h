#pragma once

#include "PayloadOS.cfg.h"

#define PayloadOS_ChooseModelSim 0
#define PayloadOS_ChooseDataSim 1

//Macro checks-------------------------------------------------
//types
#ifndef PayloadOS_intType
static_assert(false, "PayloadOS_intType must be defined in file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_uintType
static_assert(false, "PayloadOS_uintType must be defined in file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_floatType
static_assert(false, "PayloadOS_floatType must be defined in file PayloadOS.cfg.h");
#endif

//structure sizes
#ifndef PayloadOS_CommandListSize
static_assert(false, "PayloadOS_CommandListSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_CommandListSize>0, "The command list size defined by PayloadOS_CommandListSize must be positive");
#endif

#ifndef PayloadOS_ConsoleRxBufferSize
static_assert(false, "PayloadOS_ConsoleRxBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_ConsoleRxBufferSize>0, "The buffer size defined by PayloadOS_ConsoleRxBufferSize must be positive");
#endif

#ifndef PayloadOS_ConsoleTxBufferSize
static_assert(false, "PayloadOS_ConsoleTxBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_ConsoleTxBufferSize>0, "The buffer size defined by PayloadOS_ConsoleTxBufferSize must be positive");
#endif

#ifndef PayloadOS_TokenBufferSize
static_assert(false, "PayloadOS_TokenBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_TokenBufferSize>0, "The buffer size defined by PayloadOS_TokenBufferSize must be positive");
#endif

#ifndef PayloadOS_CommandBufferSize
static_assert(false, "PayloadOS_CommandBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_CommandBufferSize>0, "The buffer size defined by PayloadOS_CommandBufferSize must be positive");
#endif

#ifndef PayloadOS_InternalCommandBufferSize
static_assert(false, "PayloadOS_InternalComandBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_InternalCommandBufferSize>0, "The buffer size defined by PayloadOS_InternalCommandBufferSize must be positive");
#endif

#ifndef PayloadOS_LogFileNameSize
static_assert(false, "PayloadOS_LogFileNameSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_LogFileNameSize>0, "The buffer size defined by PayloadOS_LogFileNameSize must be positive");
#endif

#ifndef PayloadOS_SimulationFileNameSize
static_assert(false, "PayloadOS_SimulationFileNameSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_SimulationFileNameSize>0, "The buffer size defined by PayloadOS_SimulationFileNameSize must be positive");
#endif

#ifndef PayloadOS_LogParseBufferSize
static_assert(false, "PayloadOS_PayloadOS_LogParseBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_LogParseBufferSize>0, "The buffer size defined by PayloadOS_LogParseBufferSize must be positive");
#endif

#ifndef PayloadOS_VarianceBufferSize
static_assert(false, "PayloadOS_VarianceBufferSize must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_VarianceBufferSize>0, "The buffer size defined by PayloadOS_VarianceBufferSizemust be positive");
#endif

//sensor configurations
/*
#ifndef PayloadOS_AltimeterSamplePeriod
static_assert(false, "PayloadOS_AltimeterSamplePeriod must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_AltimeterSamplePeriod>0, "The interval defined by PayloadOS_AltimeterSamplePeriod must be positive");
#endif
*/

#ifndef PayloadOS_STEMnautAccelerometerSamplePeriod
static_assert(false, "PayloadOS_STEMnautAccelerometerSamplePeriod must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_STEMnautAccelerometerSamplePeriod>0, "The interval defined by PayloadOS_STEMnautAccelerometerSamplePeriod must be positive");
#endif

#ifndef PayloadOS_STEMnautGyroscopeSamplePeriod
static_assert(false, "PayloadOS_STEMnautGyroscopeSamplePeriod must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_STEMnautGyroscopeSamplePeriod>0, "The interval defined by PayloadOS_STEMnautGyroscopeSamplePeriod must be positive");
#endif

#ifndef PayloadOS_STEMnautMagnetometerSamplePeriod
static_assert(false, "PayloadOS_STEMnautMagnetometerSamplePeriod must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_STEMnautMagnetometerSamplePeriod>0, "The interval defined by PayloadOS_STEMnautMagnetometerSamplePeriod must be positive");
#endif

//simulation configurations
#ifndef PayloadOS_ModelSim_ProcessOrder
static_assert(false, "PayloadOS_ModelSim_ProcessOrder must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_ModelSim_ProcessOrder>1, "The order defined by PayloadOS_ModelSim_ProcessOrder must be greater than zero");
#endif

#ifndef PayloadOS_DataSimDefaultFileName
    static_assert(false, "PayloadOS_DataSimDefaultFileName must be deifned in the file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_DefaultSimulation
    static_assert(false, "PayloadOS_DefaultSimulation must be deifned in the file PayloadOS.cfg.h")
#else
    static_assert(PayloadOS_DefaultSimulation == PayloadOS_ChooseDataSim || PayloadOS_DefaultSimulation == PayloadOS_ChooseModelSim, "PayloadOS_DefaultSimulation must be deifned as PayloadOS_ChooseModelSim or PayloadOS_ChooseDataSim");
#endif

#ifndef PayloadOS_DataSimDefaultClockPeriod_us
static_assert(false, "PayloadOS_DataSimDefaultClockPeriod_us must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_DataSimDefaultClockPeriod_us>0, "The interval defined by PayloadOS_DataSimDefaultClockPeriod_us must be positive");
#endif

#ifndef PayloadOS_ModelSimDefaultClockPeriod_us
static_assert(false, "PayloadOS_ModelSimDefaultClockPeriod_us must be defined in the file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_ModelSimDefaultClockPeriod_us>0, "The interval defined by PayloadOS_ModelSimDefaultClockPeriod_us must be positive");
#endif



//other configurations
#ifndef PayloadOS_ConsoleBaudRate
static_assert(false, "PayloadOS_ConsoleBaudRate must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_ConsoleBaudRate>0, "The baud rate defined by PayloadOS_ConsoleBaudRate must be positive");
#endif

#ifndef PayloadOS_DefaultSamplePeriod
static_assert(false, "PayloadOS_DefaultSamplePeriod must be defined in file PayloadOS.cfg.h");
#else
static_assert(PayloadOS_DefaultSamplePeriod>0, "The state machine & log sample rate defined by PayloadOS_DefaultSamplePeriod must be positive");
#endif

#ifndef PayloadOS_DefaultAnalysisFileName
    static_assert(false, "PayloadOS_DefaultAnalysisFileName must be deifned in the file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_DefaultBlackBoxFileName
    static_assert(false, "PayloadOS_DefaultBlackBoxFileNamee must be deifned in the file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_DefaultEventFileName
    static_assert(false, "PayloadOS_DefaultEventFileName must be deifned in the file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_DefaultToLiveMode
    static_assert(false, "PayloadOS_DefaultToLiveMode must be defined in the file PayloadOS.cfg.h");
#else
    static_assert(PayloadOS_DefaultToLiveMode == true || PayloadOS_DefaultToLiveMode == false, "PayloadOS_DefaultToLiveMode must be a boolean value");
#endif




//Typedefs-----------------------------------------------------
namespace PayloadOS{
    using int_t = PayloadOS_intType;
    using uint_t = PayloadOS_uintType;
    using float_t = PayloadOS_floatType;
    using error_t = bool;
    constexpr error_t GOOD = false;
    constexpr error_t ERROR = true;
}