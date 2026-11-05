/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_t1s.c

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

#include "app_t1s.h"
#include "app_servo.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_T1S_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_T1S_DATA app_t1sData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Function: APP_T1S_TimerCallback
 * -------------------------------
 * Timer callback function for the T1S application.
 *
 * Parameters:
 *   context - User-defined context value associated with the timer callback.
 *
 * Returns:
 *   None.
 */
static void APP_T1S_TimerCallback(uintptr_t context)
{
    app_t1sData.tmrExpired = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/*
 * Function: APP_T1S_Data_Handle
 * -----------------------------
 * Handles pending T1S application data/events.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   true if data was received, false otherwise.
 */
static bool APP_T1S_Data_Handle(void)
{
    uint8_t rxBuffer[T1S_RX_DATA_SIZE];
    uint8_t txBuffer[T1S_TX_DATA_SIZE];

    bool dataReceived = false;

    if (TCPIP_TCP_GetIsReady(app_t1sData.client_socket) >= sizeof(rxBuffer))
    {
        TCPIP_TCP_ArrayGet(app_t1sData.client_socket, rxBuffer, sizeof(rxBuffer));
        dataReceived = true;

        if (T1S_DATA_CMD == rxBuffer[0])
        {
            uint8_t gateCmd = rxBuffer[1];

            if (gateCmd != app_t1sData.gateState)
            {
                app_t1sData.gateState = gateCmd;
                APP_SERVO_SetGate(gateCmd);
            }

            if (TCPIP_TCP_PutIsReady(app_t1sData.client_socket) >= sizeof(txBuffer))
            {
                uint16_t count = APP_SERVO_GetObjectCount();

                memset(txBuffer, 0, sizeof(txBuffer));

                txBuffer[0] = T1S_DATA_ACK;
                txBuffer[1] = gateCmd;
                txBuffer[2] = app_servoData.gateOpen ? 1 : 0;
                txBuffer[3] = (uint8_t)(count & 0xFF);
                txBuffer[4] = (uint8_t)((count >> 8) & 0xFF);

                TCPIP_TCP_ArrayPut(app_t1sData.client_socket, txBuffer, sizeof(txBuffer));
            }
        }
    }

    return dataReceived;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_T1S_Initialize ( void )

  Remarks:
    See prototype in app_t1s.h.
 */

void APP_T1S_Initialize(void)
{
    app_t1sData.state = APP_T1S_STATE_INIT;
    app_t1sData.t1sTimer = SYS_TIME_HANDLE_INVALID;
    app_t1sData.client_socket = INVALID_SOCKET;
    app_t1sData.tmrExpired = false;
    app_t1sData.ipAcquired = false;
    app_t1sData.ipAddr.Val = 0;
    app_t1sData.netH = NULL;
    app_t1sData.nNets = 0;
    app_t1sData.retryCount = 0;
    app_t1sData.disconnectCount = 0;
    app_t1sData.noDataCount = 0;
    app_t1sData.gateState = 0;
}


/******************************************************************************
  Function:
    void APP_T1S_Tasks ( void )

  Remarks:
    See prototype in app_t1s.h.
 */

void APP_T1S_Tasks(void)
{
    switch (app_t1sData.state)
    {
        case APP_T1S_STATE_INIT:
        {
            if (SYS_STATUS_READY == TCPIP_STACK_Status(sysObj.tcpip))
            {
                app_t1sData.state = APP_T1S_STATE_TCPIP_WAIT_FOR_IP;
            }

            else if (SYS_STATUS_UNINITIALIZED > TCPIP_STACK_Status(sysObj.tcpip))
            {
                app_t1sData.state = APP_T1S_STATE_ERROR;
            }
            break;
        }

        case APP_T1S_STATE_TCPIP_WAIT_FOR_IP:
        {
            app_t1sData.nNets = TCPIP_STACK_NumberOfNetworksGet();

            for (int i = 0; i < app_t1sData.nNets; i++)
            {
                app_t1sData.netH = TCPIP_STACK_IndexToNet(i);

                if (!TCPIP_STACK_NetIsReady(app_t1sData.netH))
                {
                    break;
                }

                if (!app_t1sData.ipAcquired)
                {
                    app_t1sData.ipAddr.Val = TCPIP_STACK_NetAddress(app_t1sData.netH);

                    SYS_CONSOLE_PRINT("Client IP Address: %d.%d.%d.%d \r\n",
                            app_t1sData.ipAddr.v[0], app_t1sData.ipAddr.v[1],
                            app_t1sData.ipAddr.v[2], app_t1sData.ipAddr.v[3]);

                    app_t1sData.ipAcquired = true;

                    TCPIP_Helper_StringToIPAddress(T1S_SERVER_IP_ADDR,
                            &app_t1sData.serverAddr.v4Add);
                }

                app_t1sData.state = APP_T1S_STATE_CLIENT_CONNECT;
                break;
            }
            break;
        }

        case APP_T1S_STATE_CLIENT_CONNECT:
        {
            if (app_t1sData.t1sTimer == SYS_TIME_HANDLE_INVALID)
            {
                app_t1sData.t1sTimer = SYS_TIME_CallbackRegisterMS(APP_T1S_TimerCallback,
                    (uintptr_t)NULL, 1000, SYS_TIME_PERIODIC);

                app_t1sData.tmrExpired = true;
            }

            if (app_t1sData.tmrExpired)
            {
                app_t1sData.tmrExpired = false;

                if (app_t1sData.client_socket != INVALID_SOCKET)
                {
                    TCPIP_TCP_Close(app_t1sData.client_socket);
                    app_t1sData.client_socket = INVALID_SOCKET;
                }

                app_t1sData.client_socket = TCPIP_TCP_ClientOpen(IP_ADDRESS_TYPE_IPV4,
                        T1S_CLIENT_PORT, &app_t1sData.serverAddr);

                if (INVALID_SOCKET != app_t1sData.client_socket)
                {
                    app_t1sData.retryCount = 0;
                    app_t1sData.state = APP_T1S_STATE_CLIENT_ACKNOWLEDGE;
                }
            }
            break;
        }

        case APP_T1S_STATE_CLIENT_ACKNOWLEDGE:
        {
            if (app_t1sData.tmrExpired)
            {
                app_t1sData.tmrExpired = false;

                if (TCPIP_TCP_IsConnected(app_t1sData.client_socket) == true)
                {
                    SYS_TIME_TimerDestroy(app_t1sData.t1sTimer);

                    app_t1sData.t1sTimer = SYS_TIME_CallbackRegisterMS(APP_T1S_TimerCallback,
                        (uintptr_t)NULL, 200, SYS_TIME_PERIODIC);

                    SYS_CONSOLE_PRINT("LAN865x_0 Connection Complete \r\n");

                    app_t1sData.retryCount = 0;
                    app_t1sData.disconnectCount = 0;
                    app_t1sData.noDataCount = 0;
                    app_t1sData.state = APP_T1S_STATE_SERVICE_TASKS;
                }

                else
                {
                    app_t1sData.retryCount++;

                    if (app_t1sData.retryCount > 10)
                    {
                        TCPIP_TCP_Close(app_t1sData.client_socket);

                        app_t1sData.client_socket = INVALID_SOCKET;
                        app_t1sData.retryCount = 0;
                        app_t1sData.state = APP_T1S_STATE_CLIENT_CONNECT;
                    }
                }
            }
            break;
        }

        case APP_T1S_STATE_SERVICE_TASKS:
        {
            if (app_t1sData.tmrExpired == true)
            {
                app_t1sData.tmrExpired = false;

                if (!TCPIP_TCP_IsConnected(app_t1sData.client_socket))
                {
                    app_t1sData.disconnectCount++;
                    SYS_CONSOLE_PRINT("Client1: Disconnected (%lu/10)...\r\n", app_t1sData.disconnectCount);

                    if (app_t1sData.disconnectCount >= 10)
                    {
                        TCPIP_TCP_Close(app_t1sData.client_socket);

                        app_t1sData.client_socket = INVALID_SOCKET;
                        app_t1sData.gateState = 0;
                        app_t1sData.disconnectCount = 0;
                        app_t1sData.noDataCount = 0;

                        APP_SERVO_SetGate(0);

                        app_t1sData.state = APP_T1S_STATE_CLIENT_CONNECT;
                    }
                }
                else
                {
                    app_t1sData.disconnectCount = 0;

                    if (APP_T1S_Data_Handle())
                    {
                        app_t1sData.noDataCount = 0;
                    }

                    else
                    {
                        app_t1sData.noDataCount++;

                        if (app_t1sData.noDataCount >= 10)
                        {
                            TCPIP_TCP_Close(app_t1sData.client_socket);

                            app_t1sData.client_socket = INVALID_SOCKET;
                            app_t1sData.gateState = 0;
                            app_t1sData.disconnectCount = 0;
                            app_t1sData.noDataCount = 0;

                            APP_SERVO_SetGate(0);

                            app_t1sData.state = APP_T1S_STATE_CLIENT_CONNECT;
                        }
                    }
                }
            }
            break;
        }

        case APP_T1S_STATE_SOCKET_CLOSURE:
        {
            if (app_t1sData.client_socket != INVALID_SOCKET)
            {
                TCPIP_TCP_Close(app_t1sData.client_socket);
                app_t1sData.client_socket = INVALID_SOCKET;
            }

            app_t1sData.state = APP_T1S_STATE_ERROR;
            break;
        }

        case APP_T1S_STATE_IDLE:
        {
            break;
        }

        case APP_T1S_STATE_ERROR:
        {
            SYS_CONSOLE_PRINT("T1S Task Error! \r\n");

            if (app_t1sData.t1sTimer != SYS_TIME_HANDLE_INVALID)
            {
                SYS_TIME_TimerDestroy(app_t1sData.t1sTimer);
                app_t1sData.t1sTimer = SYS_TIME_HANDLE_INVALID;
            }

            if (app_t1sData.client_socket != INVALID_SOCKET)
            {
                TCPIP_TCP_Close(app_t1sData.client_socket);
                app_t1sData.client_socket = INVALID_SOCKET;
            }

            app_t1sData.gateState = 0;

            APP_SERVO_SetGate(0);

            app_t1sData.state = APP_T1S_STATE_IDLE;
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
