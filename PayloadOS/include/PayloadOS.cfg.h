#pragma once
/*PayloadOS Configuration File
 * Macros in this file setup the PayloadOS build
*/
#include <cstdint>

/*PayloadOS Types----------------------------------------------
 * These types should be chosen based on the target MCU's hardware. For teensy 4.1 32 bit values are used
 * Macros:
 * PayloadOS_intType - size of default integer type
 * PayloadOS_uintType - size of default unsigned integer type
 * PayloadOS_floatType - size of default float type
 *
*/

#define PayloadOS_intType int32_t
#define PayloadOS_uintType uint32_t
#define PayloadOS_floatType float

/*Connected Peripherals----------------------------------------
 * If a peripheral is not connected, do undefine the associated macro. If it is, leave the macro defined
 * Macros:
 * PayloadOS_altimeter
 * PayloadOS_IMU
 * PayloadOS_lightAPRS_Tx
 * PayloadOS_lightAPRS_altimeter
 * PayloadOS_lightAPRS_GPS
 * PayloadOS_powerCheck
 * PayloadOS_STEMnaut1
 * PayloadOS_STEMnaut2
 * PayloadOS_STEMnaut3
 * PayloadOS_STEMnaut4
*/

//defines--------------------------------------

#define PayloadOS_altimeter
#define PayloadOS_IMU
#define PayloadOS_lightAPRS_Tx
#define PayloadOS_lightAPRS_altimeter
#define PayloadOS_lightAPRS_GPS
#define PayloadOS_powerCheck
#define PayloadOS_STEMnaut1
#define PayloadOS_STEMnaut2
#define PayloadOS_STEMnaut3
#define PayloadOS_STEMnaut4

//Undefines------------------------------------

#undef PayloadOS_altimeter
#undef PayloadOS_IMU
#undef PayloadOS_lightAPRS_Tx
#undef PayloadOS_lightAPRS_altimeter
#undef PayloadOS_lightAPRS_GPS
#undef PayloadOS_powerCheck
#undef PayloadOS_STEMnaut1
#undef PayloadOS_STEMnaut2
#undef PayloadOS_STEMnaut3
#undef PayloadOS_STEMnaut4


/*Internal Data Structure Sizes--------------------------------
 * Macros:
 * PayloadOS_CommandListSize - specifies the size of the command list data structure. command lists may have at most this many commands.
 * PayloadOS_ConsoleRxBufferSize - specifies the size of the console input buffer. Commands and other transmissions must be lesss than this size to be correctly interpreted.
 * PayloadOS_ConsoleTxBufferSize - specifies the size of the console output buffer. Output from programs must be less than this size to be printed correctly.
 * PayloadOS_TokenBufferSize - specifies the number of tokens that can be interpreted at once by the console interpreter. This limits the number of arguments that commands can have.
 * PayloadOS_CommandBufferSize - specifies the size of the buffer that stores command names durring interpretation. This limits the size of commands.
 * PayloadOS_InternalCommandBufferSize - specifies the size of the buffer that internal OS commands use to copy string their arguments to. This limits the size of strings that can be passed to them.
*/

#define PayloadOS_CommandListSize 10
#define PayloadOS_ConsoleRxBufferSize 256
#define PayloadOS_ConsoleTxBufferSize 256
#define PayloadOS_TokenBufferSize 16
#define PayloadOS_CommandBufferSize 64
#define PayloadOS_InternalCommandBufferSize 64

/*Other Configurations-----------------------------------------
 * Macros:
 * PayloadOS_ConsoleBaudRate - baud rate of the console. Make sure that serial monitor or whatever console program you're usign matches this
*/

#define PayloadOS_ConsoleBaudRate 115200