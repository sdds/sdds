/***************************************************************************//**
   \file WSNVisualizer.h

   \brief

   \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

   Copyright (c) 2008 , Atmel Corporation. All rights reserved.
   Licensed under Atmel's Limited License Agreement (BitCloudTM).

   \internal
 ******************************************************************************/
#ifndef _WSNVISUALIZER_H
#define _WSNVISUALIZER_H

#ifdef _LCD_
#include "lcd.h"
#endif // _LCD_


#ifdef _LEDS_
#include "leds.h"
#endif // _LEDS_

void
visualizeAppStarting(void);
void
visualizeNwkStarting(void);
void
visualizeNwkStarted(void);
void
visualizeNwkLeaving(void);
void
visualizeNwkLeft(void);
void
visualizeAirTxStarted(void);
void
visualizeAirTxFInished(void);
void
visualizeAirRxFinished(void);
void
visualizeSerialTx(void);
void
visualizeWakeUp(void);
void
visualizeSleep(void);
int
isVisualizerReadyToSleep(void);
void
visializeText(char*);

#ifdef _LEDS_
#define appOpenLeds() BSP_OpenLeds()
#define appCloseLeds() BSP_CloseLeds()
#define appOnLed(id) BSP_OnLed(id)
#define appOffLed(id) BSP_OffLed(id)
#define appToggleLed(id) BSP_ToggleLed(id)
#else
#define appOpenLeds()
#define appCloseLeds()
#define appOnLed(id)
#define appOffLed(id)
#define appToggleLed(id)
#endif // _LEDS_

#ifdef _LCD_
#define appOpenLcd                 BSP_OpenLcd
#define appSendLcdCmd              BSP_SendLcdCmd
#define appSendLcdMsg              BSP_SendLcdMsg
#define appIsLcdOperationCompleted BSP_IsLcdOperationCompleted
#else  // !_LCD_
INLINE void
appSendLcdCmd(uint8_t cmdId) {
    (void)cmdId;
}
INLINE void
appOpenLcd(void) {
}
INLINE void
appSendLcdMsg(const char* str) {
    (void)str;
}
INLINE int
appIsLcdOperationCompleted(void) {
    return 1;
}
#endif  // _LCD_

#endif//  _WSNVISUALIZER_H

// eof WSNVisualizer.h