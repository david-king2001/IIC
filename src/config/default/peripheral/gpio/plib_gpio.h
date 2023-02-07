/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************


/*** Macros for RELAY3 pin ***/
#define RELAY3_Set()               (LATESET = (1<<5))
#define RELAY3_Clear()             (LATECLR = (1<<5))
#define RELAY3_Toggle()            (LATEINV= (1<<5))
#define RELAY3_OutputEnable()      (TRISECLR = (1<<5))
#define RELAY3_InputEnable()       (TRISESET = (1<<5))
#define RELAY3_Get()               ((PORTE >> 5) & 0x1)
#define RELAY3_PIN                  GPIO_PIN_RE5

/*** Macros for RELAY6 pin ***/
#define RELAY6_Set()               (LATESET = (1<<6))
#define RELAY6_Clear()             (LATECLR = (1<<6))
#define RELAY6_Toggle()            (LATEINV= (1<<6))
#define RELAY6_OutputEnable()      (TRISECLR = (1<<6))
#define RELAY6_InputEnable()       (TRISESET = (1<<6))
#define RELAY6_Get()               ((PORTE >> 6) & 0x1)
#define RELAY6_PIN                  GPIO_PIN_RE6

/*** Macros for RELAY7 pin ***/
#define RELAY7_Set()               (LATESET = (1<<7))
#define RELAY7_Clear()             (LATECLR = (1<<7))
#define RELAY7_Toggle()            (LATEINV= (1<<7))
#define RELAY7_OutputEnable()      (TRISECLR = (1<<7))
#define RELAY7_InputEnable()       (TRISESET = (1<<7))
#define RELAY7_Get()               ((PORTE >> 7) & 0x1)
#define RELAY7_PIN                  GPIO_PIN_RE7

/*** Macros for SS_DAC1 pin ***/
#define SS_DAC1_Set()               (LATBSET = (1<<4))
#define SS_DAC1_Clear()             (LATBCLR = (1<<4))
#define SS_DAC1_Toggle()            (LATBINV= (1<<4))
#define SS_DAC1_OutputEnable()      (TRISBCLR = (1<<4))
#define SS_DAC1_InputEnable()       (TRISBSET = (1<<4))
#define SS_DAC1_Get()               ((PORTB >> 4) & 0x1)
#define SS_DAC1_PIN                  GPIO_PIN_RB4

/*** Macros for RELAY2 pin ***/
#define RELAY2_Set()               (LATBSET = (1<<9))
#define RELAY2_Clear()             (LATBCLR = (1<<9))
#define RELAY2_Toggle()            (LATBINV= (1<<9))
#define RELAY2_OutputEnable()      (TRISBCLR = (1<<9))
#define RELAY2_InputEnable()       (TRISBSET = (1<<9))
#define RELAY2_Get()               ((PORTB >> 9) & 0x1)
#define RELAY2_PIN                  GPIO_PIN_RB9

/*** Macros for DIGITAL0 pin ***/
#define DIGITAL0_Set()               (LATBSET = (1<<14))
#define DIGITAL0_Clear()             (LATBCLR = (1<<14))
#define DIGITAL0_Toggle()            (LATBINV= (1<<14))
#define DIGITAL0_OutputEnable()      (TRISBCLR = (1<<14))
#define DIGITAL0_InputEnable()       (TRISBSET = (1<<14))
#define DIGITAL0_Get()               ((PORTB >> 14) & 0x1)
#define DIGITAL0_PIN                  GPIO_PIN_RB14

/*** Macros for RELAY5 pin ***/
#define RELAY5_Set()               (LATFSET = (1<<4))
#define RELAY5_Clear()             (LATFCLR = (1<<4))
#define RELAY5_Toggle()            (LATFINV= (1<<4))
#define RELAY5_OutputEnable()      (TRISFCLR = (1<<4))
#define RELAY5_InputEnable()       (TRISFSET = (1<<4))
#define RELAY5_Get()               ((PORTF >> 4) & 0x1)
#define RELAY5_PIN                  GPIO_PIN_RF4

/*** Macros for RELAY4 pin ***/
#define RELAY4_Set()               (LATFSET = (1<<5))
#define RELAY4_Clear()             (LATFCLR = (1<<5))
#define RELAY4_Toggle()            (LATFINV= (1<<5))
#define RELAY4_OutputEnable()      (TRISFCLR = (1<<5))
#define RELAY4_InputEnable()       (TRISFSET = (1<<5))
#define RELAY4_Get()               ((PORTF >> 5) & 0x1)
#define RELAY4_PIN                  GPIO_PIN_RF5

/*** Macros for RELAY0 pin ***/
#define RELAY0_Set()               (LATDSET = (1<<8))
#define RELAY0_Clear()             (LATDCLR = (1<<8))
#define RELAY0_Toggle()            (LATDINV= (1<<8))
#define RELAY0_OutputEnable()      (TRISDCLR = (1<<8))
#define RELAY0_InputEnable()       (TRISDSET = (1<<8))
#define RELAY0_Get()               ((PORTD >> 8) & 0x1)
#define RELAY0_PIN                  GPIO_PIN_RD8

/*** Macros for ADC_RDY pin ***/
#define ADC_RDY_Set()               (LATDSET = (1<<9))
#define ADC_RDY_Clear()             (LATDCLR = (1<<9))
#define ADC_RDY_Toggle()            (LATDINV= (1<<9))
#define ADC_RDY_OutputEnable()      (TRISDCLR = (1<<9))
#define ADC_RDY_InputEnable()       (TRISDSET = (1<<9))
#define ADC_RDY_Get()               ((PORTD >> 9) & 0x1)
#define ADC_RDY_PIN                  GPIO_PIN_RD9

/*** Macros for RELAY1 pin ***/
#define RELAY1_Set()               (LATDSET = (1<<0))
#define RELAY1_Clear()             (LATDCLR = (1<<0))
#define RELAY1_Toggle()            (LATDINV= (1<<0))
#define RELAY1_OutputEnable()      (TRISDCLR = (1<<0))
#define RELAY1_InputEnable()       (TRISDSET = (1<<0))
#define RELAY1_Get()               ((PORTD >> 0) & 0x1)
#define RELAY1_PIN                  GPIO_PIN_RD0

/*** Macros for DIGITAL1 pin ***/
#define DIGITAL1_Set()               (LATDSET = (1<<1))
#define DIGITAL1_Clear()             (LATDCLR = (1<<1))
#define DIGITAL1_Toggle()            (LATDINV= (1<<1))
#define DIGITAL1_OutputEnable()      (TRISDCLR = (1<<1))
#define DIGITAL1_InputEnable()       (TRISDSET = (1<<1))
#define DIGITAL1_Get()               ((PORTD >> 1) & 0x1)
#define DIGITAL1_PIN                  GPIO_PIN_RD1

/*** Macros for SS_ADC pin ***/
#define SS_ADC_Set()               (LATDSET = (1<<4))
#define SS_ADC_Clear()             (LATDCLR = (1<<4))
#define SS_ADC_Toggle()            (LATDINV= (1<<4))
#define SS_ADC_OutputEnable()      (TRISDCLR = (1<<4))
#define SS_ADC_InputEnable()       (TRISDSET = (1<<4))
#define SS_ADC_Get()               ((PORTD >> 4) & 0x1)
#define SS_ADC_PIN                  GPIO_PIN_RD4

/*** Macros for SDO pin ***/
#define SDO_Get()               ((PORTD >> 5) & 0x1)
#define SDO_PIN                  GPIO_PIN_RD5

/*** Macros for BTN0 pin ***/
#define BTN0_Set()               (LATDSET = (1<<6))
#define BTN0_Clear()             (LATDCLR = (1<<6))
#define BTN0_Toggle()            (LATDINV= (1<<6))
#define BTN0_OutputEnable()      (TRISDCLR = (1<<6))
#define BTN0_InputEnable()       (TRISDSET = (1<<6))
#define BTN0_Get()               ((PORTD >> 6) & 0x1)
#define BTN0_PIN                  GPIO_PIN_RD6

/*** Macros for DIGITAL3 pin ***/
#define DIGITAL3_Set()               (LATFSET = (1<<0))
#define DIGITAL3_Clear()             (LATFCLR = (1<<0))
#define DIGITAL3_Toggle()            (LATFINV= (1<<0))
#define DIGITAL3_OutputEnable()      (TRISFCLR = (1<<0))
#define DIGITAL3_InputEnable()       (TRISFSET = (1<<0))
#define DIGITAL3_Get()               ((PORTF >> 0) & 0x1)
#define DIGITAL3_PIN                  GPIO_PIN_RF0

/*** Macros for DIGITAL2 pin ***/
#define DIGITAL2_Set()               (LATFSET = (1<<1))
#define DIGITAL2_Clear()             (LATFCLR = (1<<1))
#define DIGITAL2_Toggle()            (LATFINV= (1<<1))
#define DIGITAL2_OutputEnable()      (TRISFCLR = (1<<1))
#define DIGITAL2_InputEnable()       (TRISFSET = (1<<1))
#define DIGITAL2_Get()               ((PORTF >> 1) & 0x1)
#define DIGITAL2_PIN                  GPIO_PIN_RF1

/*** Macros for SS_DAC2 pin ***/
#define SS_DAC2_Set()               (LATESET = (1<<3))
#define SS_DAC2_Clear()             (LATECLR = (1<<3))
#define SS_DAC2_Toggle()            (LATEINV= (1<<3))
#define SS_DAC2_OutputEnable()      (TRISECLR = (1<<3))
#define SS_DAC2_InputEnable()       (TRISESET = (1<<3))
#define SS_DAC2_Get()               ((PORTE >> 3) & 0x1)
#define SS_DAC2_PIN                  GPIO_PIN_RE3

/*** Macros for LED_RED pin ***/
#define LED_RED_Set()               (LATESET = (1<<4))
#define LED_RED_Clear()             (LATECLR = (1<<4))
#define LED_RED_Toggle()            (LATEINV= (1<<4))
#define LED_RED_OutputEnable()      (TRISECLR = (1<<4))
#define LED_RED_InputEnable()       (TRISESET = (1<<4))
#define LED_RED_Get()               ((PORTE >> 4) & 0x1)
#define LED_RED_PIN                  GPIO_PIN_RE4


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_B = 0,
    GPIO_PORT_C = 1,
    GPIO_PORT_D = 2,
    GPIO_PORT_E = 3,
    GPIO_PORT_F = 4,
    GPIO_PORT_G = 5,
} GPIO_PORT;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RB0 = 0,
    GPIO_PIN_RB1 = 1,
    GPIO_PIN_RB2 = 2,
    GPIO_PIN_RB3 = 3,
    GPIO_PIN_RB4 = 4,
    GPIO_PIN_RB5 = 5,
    GPIO_PIN_RB6 = 6,
    GPIO_PIN_RB7 = 7,
    GPIO_PIN_RB8 = 8,
    GPIO_PIN_RB9 = 9,
    GPIO_PIN_RB10 = 10,
    GPIO_PIN_RB11 = 11,
    GPIO_PIN_RB12 = 12,
    GPIO_PIN_RB13 = 13,
    GPIO_PIN_RB14 = 14,
    GPIO_PIN_RB15 = 15,
    GPIO_PIN_RC12 = 28,
    GPIO_PIN_RC13 = 29,
    GPIO_PIN_RC14 = 30,
    GPIO_PIN_RC15 = 31,
    GPIO_PIN_RD0 = 32,
    GPIO_PIN_RD1 = 33,
    GPIO_PIN_RD2 = 34,
    GPIO_PIN_RD3 = 35,
    GPIO_PIN_RD4 = 36,
    GPIO_PIN_RD5 = 37,
    GPIO_PIN_RD6 = 38,
    GPIO_PIN_RD7 = 39,
    GPIO_PIN_RD8 = 40,
    GPIO_PIN_RD9 = 41,
    GPIO_PIN_RD10 = 42,
    GPIO_PIN_RD11 = 43,
    GPIO_PIN_RE0 = 48,
    GPIO_PIN_RE1 = 49,
    GPIO_PIN_RE2 = 50,
    GPIO_PIN_RE3 = 51,
    GPIO_PIN_RE4 = 52,
    GPIO_PIN_RE5 = 53,
    GPIO_PIN_RE6 = 54,
    GPIO_PIN_RE7 = 55,
    GPIO_PIN_RF0 = 64,
    GPIO_PIN_RF1 = 65,
    GPIO_PIN_RF3 = 67,
    GPIO_PIN_RF4 = 68,
    GPIO_PIN_RF5 = 69,
    GPIO_PIN_RG6 = 86,
    GPIO_PIN_RG7 = 87,
    GPIO_PIN_RG8 = 88,
    GPIO_PIN_RG9 = 89,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;


void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite((GPIO_PORT)(pin>>4), (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead((GPIO_PORT)(pin>>4))) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead((GPIO_PORT)(pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable((GPIO_PORT)(pin>>4), 0x1 << (pin & 0xF));
}


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
