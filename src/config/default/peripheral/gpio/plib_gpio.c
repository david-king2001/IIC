/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.c

  Summary:
    GPIO function implementations for the GPIO PLIB.

  Description:
    The GPIO PLIB provides a simple interface to manage peripheral
    input-output controller.

*******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#include "plib_gpio.h"


/* Array to store callback objects of each configured interrupt */
GPIO_PIN_CALLBACK_OBJ portPinCbObj[5];

/* Array to store number of interrupts in each PORT Channel + previous interrupt count */
uint8_t portNumCb[6 + 1] = { 0, 1, 1, 2, 5, 5, 5, };

/******************************************************************************
  Function:
    GPIO_Initialize ( void )

  Summary:
    Initialize the GPIO library.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_Initialize ( void )
{
    /* PORTB Initialization */
    LATB = 0x210; /* Initial Latch Value */
    TRISBCLR = 0x210; /* Direction Control */
    ANSELBCLR = 0x6210; /* Digital Mode Enable */
    CNPDBSET = 0x4000; /* Pull-Down Enable */
    /* Change Notice Enable */
    CNCONBSET = _CNCONB_ON_MASK;
    PORTB;
    IEC1SET = _IEC1_CNBIE_MASK;

    /* PORTC Initialization */

    /* PORTD Initialization */
    LATD = 0x101; /* Initial Latch Value */
    TRISDCLR = 0x111; /* Direction Control */
    ANSELDCLR = 0xe; /* Digital Mode Enable */
    CNPUDSET = 0x10; /* Pull-Up Enable */
    CNPDDSET = 0xa; /* Pull-Down Enable */
    /* Change Notice Enable */
    CNCONDSET = _CNCOND_ON_MASK;
    PORTD;
    IEC1SET = _IEC1_CNDIE_MASK;

    /* PORTE Initialization */
    LATE = 0xf8; /* Initial Latch Value */
    TRISECLR = 0xf8; /* Direction Control */
    ANSELECLR = 0xf4; /* Digital Mode Enable */
    /* Change Notice Enable */
    CNCONESET = _CNCONE_ON_MASK;
    PORTE;
    IEC1SET = _IEC1_CNEIE_MASK;

    /* PORTF Initialization */
    LATF = 0x30; /* Initial Latch Value */
    TRISFCLR = 0x30; /* Direction Control */
    CNPDFSET = 0x3; /* Pull-Down Enable */

    /* PORTG Initialization */
    ANSELGCLR = 0x180; /* Digital Mode Enable */



    /* PPS Input Remapping */
    SDI1R = 0;
    U1RXR = 1;

    /* PPS Output Remapping */
    RPD5R = 8;
    RPG7R = 3;


    uint32_t i;
    /* Initialize Interrupt Pin data structures */
    portPinCbObj[0 + 0].pin = GPIO_PIN_RB13;
    
    portPinCbObj[1 + 0].pin = GPIO_PIN_RD6;
    
    portPinCbObj[2 + 0].pin = GPIO_PIN_RE0;
    
    portPinCbObj[2 + 1].pin = GPIO_PIN_RE1;
    
    portPinCbObj[2 + 2].pin = GPIO_PIN_RE2;
    
    for(i=0; i<5; i++)
    {
        portPinCbObj[i].callback = NULL;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: GPIO APIs which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t GPIO_PortRead ( GPIO_PORT port )

  Summary:
    Read all the I/O lines of the selected port.

  Description:
    This function reads the live data values on all the I/O lines of the
    selected port.  Bit values returned in each position indicate corresponding
    pin levels.
    1 = Pin is high.
    0 = Pin is low.

    This function reads the value regardless of pin configuration, whether it is
    set as as an input, driven by the GPIO Controller, or driven by a peripheral.

  Remarks:
    If the port has less than 32-bits, unimplemented pins will read as
    low (0).
    Implemented pins are Right aligned in the 32-bit return value.
*/
uint32_t GPIO_PortRead(GPIO_PORT port)
{
    return (*(volatile uint32_t *)(&PORTB + (port * 0x40)));
}

// *****************************************************************************
/* Function:
    void GPIO_PortWrite (GPIO_PORT port, uint32_t mask, uint32_t value);

  Summary:
    Write the value on the masked I/O lines of the selected port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value)
{
    *(volatile uint32_t *)(&LATB + (port * 0x40)) = (*(volatile uint32_t *)(&LATB + (port * 0x40)) & (~mask)) | (mask & value);
}

// *****************************************************************************
/* Function:
    uint32_t GPIO_PortLatchRead ( GPIO_PORT port )

  Summary:
    Read the latched value on all the I/O lines of the selected port.

  Remarks:
    See plib_gpio.h for more details.
*/
uint32_t GPIO_PortLatchRead(GPIO_PORT port)
{
    return (*(volatile uint32_t *)(&LATB + (port * 0x40)));
}

// *****************************************************************************
/* Function:
    void GPIO_PortSet ( GPIO_PORT port, uint32_t mask )

  Summary:
    Set the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortSet(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATBSET + (port * 0x40)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortClear ( GPIO_PORT port, uint32_t mask )

  Summary:
    Clear the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortClear(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATBCLR + (port * 0x40)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortToggle ( GPIO_PORT port, uint32_t mask )

  Summary:
    Toggles the selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortToggle(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&LATBINV + (port * 0x40))= mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInputEnable ( GPIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as input.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&TRISBSET + (port * 0x40)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortOutputEnable ( GPIO_PORT port, uint32_t mask )

  Summary:
    Enables selected IO pins of a port as output(s).

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&TRISBCLR + (port * 0x40)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask)

  Summary:
    Enables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNENBSET + (port * 0x40)) = mask;
}

// *****************************************************************************
/* Function:
    void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask)

  Summary:
    Disables IO interrupt on selected IO pins of a port.

  Remarks:
    See plib_gpio.h for more details.
*/
void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask)
{
    *(volatile uint32_t *)(&CNENBCLR + (port * 0x40)) = mask;
}

// *****************************************************************************
// *****************************************************************************
// Section: GPIO APIs which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    bool GPIO_PinInterruptCallbackRegister(
        GPIO_PIN pin,
        const GPIO_PIN_CALLBACK callback,
        uintptr_t context
    );

  Summary:
    Allows application to register callback for configured pin.

  Remarks:
    See plib_gpio.h for more details.
*/
bool GPIO_PinInterruptCallbackRegister(
    GPIO_PIN pin,
    const GPIO_PIN_CALLBACK callback,
    uintptr_t context
)
{
    uint8_t i;
    uint8_t portIndex;

    portIndex = pin >> 4;

    for(i = portNumCb[portIndex]; i < portNumCb[portIndex +1]; i++)
    {
        if (portPinCbObj[i].pin == pin)
        {
            portPinCbObj[i].callback = callback;
            portPinCbObj[i].context  = context;
            return true;
        }
    }
    return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Local Function Implementation
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    void CHANGE_NOTICE_B_InterruptHandler(void)

  Summary:
    Interrupt Handler for change notice interrupt for channel B.

  Remarks:
	It is an internal function, user should not call it directly.
*/
void CHANGE_NOTICE_B_InterruptHandler(void)
{
    uint8_t i;
    uint32_t status;

    status  = CNSTATB;
    status &= CNENB;

    PORTB;
    IFS1CLR = _IFS1_CNBIF_MASK;

    /* Check pending events and call callback if registered */
    for(i = 0; i < 1; i++)
    {
        if((status & (1 << (portPinCbObj[i].pin & 0xF))) && (portPinCbObj[i].callback != NULL))
        {
            portPinCbObj[i].callback (portPinCbObj[i].pin, portPinCbObj[i].context);
        }
    }
}

// *****************************************************************************
/* Function:
    void CHANGE_NOTICE_D_InterruptHandler(void)

  Summary:
    Interrupt Handler for change notice interrupt for channel D.

  Remarks:
	It is an internal function, user should not call it directly.
*/
void CHANGE_NOTICE_D_InterruptHandler(void)
{
    uint8_t i;
    uint32_t status;

    status  = CNSTATD;
    status &= CNEND;

    PORTD;
    IFS1CLR = _IFS1_CNDIF_MASK;

    /* Check pending events and call callback if registered */
    for(i = 1; i < 2; i++)
    {
        if((status & (1 << (portPinCbObj[i].pin & 0xF))) && (portPinCbObj[i].callback != NULL))
        {
            portPinCbObj[i].callback (portPinCbObj[i].pin, portPinCbObj[i].context);
        }
    }
}

// *****************************************************************************
/* Function:
    void CHANGE_NOTICE_E_InterruptHandler(void)

  Summary:
    Interrupt Handler for change notice interrupt for channel E.

  Remarks:
	It is an internal function, user should not call it directly.
*/
void CHANGE_NOTICE_E_InterruptHandler(void)
{
    uint8_t i;
    uint32_t status;

    status  = CNSTATE;
    status &= CNENE;

    PORTE;
    IFS1CLR = _IFS1_CNEIF_MASK;

    /* Check pending events and call callback if registered */
    for(i = 2; i < 5; i++)
    {
        if((status & (1 << (portPinCbObj[i].pin & 0xF))) && (portPinCbObj[i].callback != NULL))
        {
            portPinCbObj[i].callback (portPinCbObj[i].pin, portPinCbObj[i].context);
        }
    }
}

/* Function:
    void CHANGE_NOTICE_InterruptHandler(void)

  Summary:
    Interrupt Handler for change notice interrupt.

  Remarks:
	It is an internal function called from ISR, user should not call it directly.
*/
void CHANGE_NOTICE_InterruptHandler(void)
{
    if(IFS1bits.CNBIF)
    {
        CHANGE_NOTICE_B_InterruptHandler();
    }
    if(IFS1bits.CNDIF)
    {
        CHANGE_NOTICE_D_InterruptHandler();
    }
    if(IFS1bits.CNEIF)
    {
        CHANGE_NOTICE_E_InterruptHandler();
    }
}

/*******************************************************************************
 End of File
*/
