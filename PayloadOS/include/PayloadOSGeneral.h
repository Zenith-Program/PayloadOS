#pragma once

#include "PayloadOS.cfg.h"

//Macro checks-------------------------------------------------
#ifndef PayloadOS_intType
static_assert(false, "PayloadOS_intType must be defined in file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_uintType
static_assert(false, "PayloadOS_uintType must be defined in file PayloadOS.cfg.h");
#endif

#ifndef PayloadOS_floatType
static_assert(false, "PayloadOS_floatType must be defined in file PayloadOS.cfg.h");
#endif


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


//Typedefs-----------------------------------------------------
namespace PayloadOS{
    using int_t = PayloadOS_intType;
    using uint_t = PayloadOS_uintType;
    using float_t = PayloadOS_floatType;
    using error_t = bool;
    constexpr error_t GOOD = false;
    constexpr error_t ERROR = true;
}