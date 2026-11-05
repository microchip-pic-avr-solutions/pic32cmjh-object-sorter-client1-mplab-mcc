/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_servo.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

/*******************************************************************************
* Copyright (C) 2026 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_servo.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

#define SERVO_GATE_OPEN     500U
#define SERVO_GATE_CLOSED   1500U

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_SERVO_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_SERVO_DATA app_servoData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Function: APP_SERVO_SetGate
 * ---------------------------
 * Sets the gate servo position and increments object count when opening.
 *
 * Parameters:
 *   state - Gate state (0 = closed, 1 = open).
 *
 * Returns:
 *   None.
 */
void APP_SERVO_SetGate(uint8_t state)
{
    if (state)
    {
        app_servoData.gateOpen = true;

        TCC0_PWM24bitDutySet(TCC0_CHANNEL1, SERVO_GATE_OPEN);
        app_servoData.objectCount++;
    }

    else
    {
        app_servoData.gateOpen = false;

        TCC0_PWM24bitDutySet(TCC0_CHANNEL1, SERVO_GATE_CLOSED);
    }
}


/*
 * Function: APP_SERVO_GetObjectCount
 * ----------------------------------
 * Gets the current object count.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   The number of objects that have passed through the gate.
 */
uint16_t APP_SERVO_GetObjectCount(void)
{
    return app_servoData.objectCount;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_SERVO_Initialize ( void )

  Remarks:
    See prototype in app_servo.h.
 */

void APP_SERVO_Initialize(void)
{
    app_servoData.state = APP_SERVO_STATE_INIT;
    app_servoData.duty = SERVO_GATE_CLOSED;
    app_servoData.objectCount = 0;
}


/******************************************************************************
  Function:
    void APP_SERVO_Tasks ( void )

  Remarks:
    See prototype in app_servo.h.
 */

void APP_SERVO_Tasks(void)
{
    switch (app_servoData.state)
    {
        case APP_SERVO_STATE_INIT:
        {
            TCC0_PWMStart();
            TCC0_PWM24bitDutySet(TCC0_CHANNEL1, app_servoData.duty);

            app_servoData.state = APP_SERVO_STATE_SERVICE_TASKS;
            break;
        }

        case APP_SERVO_STATE_SERVICE_TASKS:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
