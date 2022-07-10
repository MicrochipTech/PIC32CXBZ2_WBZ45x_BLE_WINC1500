/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "wdrv_winc_client_api.h"
#include "serial_bridge/serial_bridge.h"
#include "platform/platform.h"

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
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

//static DRV_HANDLE wdrvHandle;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


static SERIAL_BRIDGE_DECODER_STATE serialBridgeDecoderState;

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize(void)
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    SerialBridge_PlatformInit();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks(void)
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
//            SERCOM0_USART_Write((uint8_t *)"Hello World\r\n",13);
            appData.state = APP_STATE_INIT_WINC;

            break;
        }

        case APP_STATE_INIT_WINC:
        {
            if (SYS_STATUS_BUSY == WDRV_WINC_Status(sysObj.drvWifiWinc))
            {
                break;
            }

            SerialBridge_Init(&serialBridgeDecoderState, 115200);

            appData.state = APP_STATE_WDRV_OPEN_BRIDGE;
            break;
        }

        case APP_STATE_WDRV_OPEN_BRIDGE:
        {
            SerialBridge_Process(&serialBridgeDecoderState);
            break;
        }

        case APP_STATE_ERROR:
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
