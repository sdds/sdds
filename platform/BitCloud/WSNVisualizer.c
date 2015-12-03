/**********************************************************************//**
   \file WSNVisualizer.c

   \brief

   \author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

   Copyright (c) 2008 , Atmel Corporation. All rights reserved.
   Licensed under Atmel's Limited License Agreement (BitCloudTM).

   \internal
   History:
    18/12/08 A. Luzhetsky - Modified
**************************************************************************/

/************************************************************************
 **************************************************************************/
#include "WSNVisualizer.h"

/************************************************************************
 **************************************************************************/
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

/************************************************************************
 **************************************************************************/

void
visializeText(char* t) {
    appSendLcdMsg(t);
}


void
visualizeAppStarting(void) {
    appOpenLeds();

    appOpenLcd();
#ifdef _LED_
    appSendLcdCmd(LCD_CMD_SYMB_ANTENNA_LEVEL_2);
    appSendLcdCmd(LCD_CMD_SYMB_ZIGBEE_ON);
    appSendLcdCmd(LCD_CMD_LED_ON);
    appSendLcdCmd(LCD_CMD_SYMB_RAVEN_ON);
    appSendLcdMsg("WSNDEMO");
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeNwkStarting(void) {
    appToggleLed(LED_RED);
    appOffLed(LED_YELLOW);
    appOffLed(LED_GREEN);

#ifdef _LED_
    appSendLcdCmd(LCD_CMD_LED_TOGGLE);
    appSendLcdMsg("JOINING");
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeNwkStarted(void) {
    appOnLed(LED_RED);
    appOffLed(LED_YELLOW);
    appOffLed(LED_GREEN);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_LED_ON);
    appSendLcdCmd(LCD_CMD_SYMB_SPACE_SUN);


#ifdef _COORDINATOR_
    appSendLcdMsg("COORD  ");
#endif
#ifdef _ROUTER_
    appSendLcdMsg("ROUTER ");
#endif
#ifdef _ENDDEVICE
    appSendLcdMsg("ENDDEV ");
#endif
#endif // _LCD_
}

/************************************************************************
 **************************************************************************/
void
visualizeNwkLeaving(void) {
    appOffLed(LED_GREEN);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_LED_OFF);
    appSendLcdMsg("LEAVING");
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeNwkLeft(void) {
    appOffLed(LED_GREEN);
}

/************************************************************************
 **************************************************************************/
void
visualizeAirTxStarted(void) {
    appOnLed(LED_GREEN);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_SYMB_TRX_TX);
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeAirTxFInished(void) {
    appOffLed(LED_GREEN);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_SYMB_TRX_RX);
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeAirRxFinished(void) {
    appToggleLed(LED_YELLOW);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_SYMB_TRX_RX);
    appSendLcdCmd(LCD_CMD_SYMB_TRX_TX);
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeSerialTx(void) {
    appToggleLed(LED_GREEN);
}

/************************************************************************
 **************************************************************************/
void
visualizeWakeUp(void) {
    appOpenLeds();
    appOnLed(LED_RED);

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_SYMB_ANTENNA_LEVEL_2);
    appSendLcdCmd(LCD_CMD_SYMB_ZIGBEE_ON);
    appSendLcdCmd(LCD_CMD_LED_ON);
    appSendLcdCmd(LCD_CMD_SYMB_RAVEN_ON);
    appSendLcdCmd(LCD_CMD_SYMB_SPACE_SUN);
    appSendLcdMsg("ENDDEV ");
#endif
}

/************************************************************************
 **************************************************************************/
void
visualizeSleep(void) {
    appCloseLeds();

#ifdef _LCD_
    appSendLcdCmd(LCD_CMD_SYMB_ANTENNA_LEVEL_0);
    appSendLcdCmd(LCD_CMD_SYMB_SPACE_MOON);
    appSendLcdCmd(LCD_CMD_LED_OFF);
    appSendLcdCmd(LCD_CMD_SYMB_TRX_OFF);
#endif
}

/************************************************************************
 **************************************************************************/
int
isVisualizerReadyToSleep(void) {
    return appIsLcdOperationCompleted();
}

// eof WSNVisualizer.c