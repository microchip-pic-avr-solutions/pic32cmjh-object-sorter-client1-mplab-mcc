/*******************************************************************************
  Board Support Package Header File.

  Company:
    Microchip Technology Inc.

  File Name:
    bsp.h

  Summary:
    Board Support Package Header File 

  Description:
    This file contains constants, macros, type definitions and function
    declarations 
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef BSP_H
#define BSP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "device.h"
#include "peripheral/port/plib_port.h"

// *****************************************************************************
// *****************************************************************************
// Section: BSP Macros
// *****************************************************************************
// *****************************************************************************
#define PIC32CMJH01_CURIOSITY_NANO_BASE
#define BOARD_NAME    "PIC32CMJH01-CURIOSITY-NANO-BASE"

/*** Macros for MIKROBUS1_INT output pin ***/ 
#define BSP_MIKROBUS1_INT_PIN        PORT_PIN_PB9
#define BSP_MIKROBUS1_INT_Get()      ((PORT_REGS->GROUP[1].PORT_IN >> 9U) & 0x01U)
#define BSP_MIKROBUS1_INT_Set()      (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 9U))
#define BSP_MIKROBUS1_INT_Clear()    (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 9U))
#define BSP_MIKROBUS1_INT_Toggle()   (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 9U))
#define BSP_MIKROBUS1_INT_On()       BSP_MIKROBUS1_INT_Set()
#define BSP_MIKROBUS1_INT_Off()      BSP_MIKROBUS1_INT_Clear() 

/*** Macros for MIKROBUS1_RST output pin ***/ 
#define BSP_MIKROBUS1_RST_PIN        PORT_PIN_PA7
#define BSP_MIKROBUS1_RST_Get()      ((PORT_REGS->GROUP[0].PORT_IN >> 7U) & 0x01U)
#define BSP_MIKROBUS1_RST_Set()      (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 7U))
#define BSP_MIKROBUS1_RST_Clear()    (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 7U))
#define BSP_MIKROBUS1_RST_Toggle()   (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 7U))
#define BSP_MIKROBUS1_RST_On()       BSP_MIKROBUS1_RST_Set()
#define BSP_MIKROBUS1_RST_Off()      BSP_MIKROBUS1_RST_Clear() 

/*** Macros for MIKROBUS1_SPI_CS output pin ***/ 
#define BSP_MIKROBUS1_SPI_CS_PIN        PORT_PIN_PA14
#define BSP_MIKROBUS1_SPI_CS_Get()      ((PORT_REGS->GROUP[0].PORT_IN >> 14U) & 0x01U)
#define BSP_MIKROBUS1_SPI_CS_Set()      (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 14U))
#define BSP_MIKROBUS1_SPI_CS_Clear()    (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 14U))
#define BSP_MIKROBUS1_SPI_CS_Toggle()   (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 14U))
#define BSP_MIKROBUS1_SPI_CS_On()       BSP_MIKROBUS1_SPI_CS_Clear()
#define BSP_MIKROBUS1_SPI_CS_Off()      BSP_MIKROBUS1_SPI_CS_Set() 

/*** Macros for LED0 output pin ***/ 
#define BSP_LED0_PIN        PORT_PIN_PA19
#define BSP_LED0_Get()      ((PORT_REGS->GROUP[0].PORT_IN >> 19U) & 0x01U)
#define BSP_LED0_Set()      (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 19U))
#define BSP_LED0_Clear()    (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 19U))
#define BSP_LED0_Toggle()   (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 19U))
#define BSP_LED0_On()       BSP_LED0_Clear()
#define BSP_LED0_Off()      BSP_LED0_Set() 



/*** Macros for SW0 input pin ***/ 
#define BSP_SW0_PIN                    PORT_PIN_PA27
#define BSP_SW0_Get()                  ((PORT_REGS->GROUP[0].PORT_IN >> 27U) & 0x01U)
#define BSP_SW0_STATE_PRESSED          0
#define BSP_SW0_STATE_RELEASED         1


// *****************************************************************************
// *****************************************************************************
// Section: Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void BSP_Initialize(void)

  Summary:
    Performs the necessary actions to initialize a board

  Description:
    This function initializes the LED and Switch ports on the board.  This
    function must be called by the user before using any APIs present on this
    BSP.

  Precondition:
    None.

  Parameters:
    None

  Returns:
    None.

  Example:
    <code>
    BSP_Initialize();
    </code>

  Remarks:
    None
*/

void BSP_Initialize(void);

#endif // BSP_H

/*******************************************************************************
 End of File
*/