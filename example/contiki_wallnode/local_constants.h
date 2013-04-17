/*
 * =====================================================================================
 *
 *       Filename:  local_constants.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  17.04.2013 12:01:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kai Beckmann (kai), kai-oliver.beckmann@hs-rm.de
 *        Company:  Hochschule RheinMain - DOPSY Labor für verteilte Systeme
 *
 * =====================================================================================
 */

#ifndef _LOCAL_CONSTANTS_H_
#define _LOCAL_CONSTANTS_H_

// for movement sensor PB0 PCINT0
#define GPIO_INPUT_IRQ_ENABLE_PCINT0

// for door, window and switch sensors

// PE7 INT7
#define GPIO_INPUT_IRQ_ENABLE_INT7
// PE5 INT5
#define GPIO_INPUT_IRQ_ENABLE_INT5
// PE4 INT4
#define GPIO_INPUT_IRQ_ENABLE_INT4

/* Settings for LEDS
*/

//#define DRV_LED_DIMMING 1

//#define DRV_LED_DIMMING_GAMMACORRECTION
/* value is the bit length of the pwm register */
//#define DRV_LED_DIMMING_GAMMACORRECTION_10BIT
/*#define DRV_LED_DIMMING_GAMMACORRECTION_16BIT*/
/*#define DRV_LED_DIMMING_GAMMACORRECTION_8BIT */


#endif /*  _LOCAL_CONSTANTS_H_ */
