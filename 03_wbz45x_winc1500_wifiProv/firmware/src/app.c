// DOM-IGNORE-BEGIN
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
#include <string.h>
#include <stdio.h>
#include "app.h"
#include "app_ble/app_ble.h"
#include "ble_cms/ble_conn_serv_svc.h"
#include "ble_dis/ble_dis.h"
#include "wdrv_winc_client_api.h"


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

uint16_t conn_hdl;
credentials cred;
static DRV_HANDLE wdrvHandle;
extern TaskHandle_t xWinc_Tasks;
extern TaskHandle_t xBle_Tasks;

extern void APP_ConnService_SendNotification(uint16_t connHandle, uint8_t char_val );

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


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

static void APP_ExampleDHCPAddressEventCallback(DRV_HANDLE handle, uint32_t ipAddress)
{
    char s[20];

    printf("IP address is %s\r\n", inet_ntop(AF_INET, &ipAddress, s, sizeof(s)));
}

static void _APP_ConnectNotifyCallback(DRV_HANDLE handle, WDRV_WINC_ASSOC_HANDLE assocHandle, WDRV_WINC_CONN_STATE currentState, WDRV_WINC_CONN_ERROR errorCode)
{
    APP_Msg_T appMsg;
    if (WDRV_WINC_CONN_STATE_CONNECTED == currentState)
    {
        printf("Wi-Fi Connected\r\n");
    }
    else if (WDRV_WINC_CONN_STATE_DISCONNECTED == currentState)
    {
        printf("Wi-Fi Disconnected\r\n");
    }
}

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    appData.appQueue = xQueueCreate( 64, sizeof(APP_Msg_T) );
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
     
    appData.consoleHandle = SYS_CONSOLE_HandleGet(SYS_CONSOLE_INDEX_0);
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    APP_Msg_T    appMsg[1];
    APP_Msg_T   *p_appMsg;
    p_appMsg=appMsg;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            
            vTaskSuspend(xWinc_Tasks);
                    
            //appData.appQueue = xQueueCreate( 10, sizeof(APP_Msg_T) );
            APP_BleStackInit();
            BLE_GAP_SetAdvEnable(0x01, 0);
            printf("Welcome to WINC1500 BLE Provisioning Example\r\n");
            
            BLE_CONN_SERV_Add();
            BLE_DIS_Add();
            
            if (appInitialized)
            {

                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if (OSAL_QUEUE_Receive(&appData.appQueue, &appMsg, OSAL_WAIT_FOREVER))
            {
                if(p_appMsg->msgId==APP_MSG_BLE_STACK_EVT)
                {
                    // Pass BLE Stack Event Message to User Application for handling
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
                {
                    // Pass BLE LOG Event Message to User Application for handling
                    APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
                }
                else if (p_appMsg->msgId==APP_MSG_BLE_PROV_COMPLETE)
                {
                    
                    APP_ConnService_SendNotification(conn_hdl, NOTIFY_STATE_PROVISIONED);
                    //printf("writeDataLength: 0x%x\r\n", p_event->writeDataLength);
                    printf("Auth Type: %d\r\n", cred.sec_type);
                    printf("SSID: %s\r\n", cred.ssid);
                    printf("SSID Len: %d\r\n", cred.ssid_length);
                    printf("Password: %s\r\n", cred.passphrase);
                    printf("Password Len: %d\r\n", cred.passphrase_length);
                
                    p_appMsg->msgId = APP_MSG_WDRV_INIT;
                    
                    /* De-initialise SPI handling. */
                    //WDRV_WINC_SPIDeinitialize();

                    /* De-initialise the interrupts. */
                    //WDRV_WINC_INTDeinitialize();
                    vTaskResume(xWinc_Tasks);
                    
                    OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 5000);
                    
                }
                else if (p_appMsg->msgId==APP_MSG_BLE_PROV_FAILED)
                {
                    if( cred.sec_type == WDRV_WINC_AUTH_TYPE_WPA_PSK)
                    {
                        printf("WPA Provisioning not supported in Android\r\nPlease test with iPhone\r\n");
                    }
                    else
                    {
                        printf("Unsupported Wi-Fi Authentication type selected\r\n, please use OPEN or WPA method\r\n");
                    }
                    APP_ConnService_SendNotification(conn_hdl, NOTIFY_STATE_PROVISIONFAILED);
                }
                else if (p_appMsg->msgId==APP_MSG_WDRV_INIT)
                {
                    BLE_GAP_Disconnect(conn_hdl, GAP_STATUS_OPERATION_CANCELLED_BY_HOST);
                    if (SYS_STATUS_READY == WDRV_WINC_Status(sysObj.drvWifiWinc))
                    {
                        wdrvHandle = WDRV_WINC_Open(0, 0);
                        p_appMsg->msgId = APP_MSG_WDRV_CONNECT;
                        OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);
                    }
                    else
                    {
                        p_appMsg->msgId = APP_MSG_WDRV_INIT;
                        OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 10);
                    }
                }
                else if (p_appMsg->msgId==APP_MSG_WDRV_CONNECT)
                {
                    if (DRV_HANDLE_INVALID != wdrvHandle)
                    {
                        WDRV_WINC_AUTH_CONTEXT authCtx;
                        WDRV_WINC_BSS_CONTEXT  bssCtx;
                        
                        /* Enable use of DHCP for network configuration, DHCP is the default
                        but this also registers the callback for notifications. */

                        WDRV_WINC_IPUseDHCPSet(wdrvHandle, &APP_ExampleDHCPAddressEventCallback);

                        /* Reset the internal BSS context */
                        WDRV_WINC_BSSCtxSetDefaults(&bssCtx);
                        
                        /* Prepare the BSS context with desired AP's parameters */
                        WDRV_WINC_BSSCtxSetChannel(&bssCtx, WLAN_CHANNEL);
                        WDRV_WINC_BSSCtxSetSSID(&bssCtx, cred.ssid, cred.ssid_length);
                        
                         /* Reset the internal Auth context */
                        WDRV_WINC_AuthCtxSetDefaults(&authCtx);
                        
                        /* Prepare the Auth context with desired AP's Security settings */
                        if (WDRV_WINC_AUTH_TYPE_OPEN == cred.sec_type)
                        {
                            WDRV_WINC_AuthCtxSetOpen(&authCtx);
                        }
                        else if (WDRV_WINC_AUTH_TYPE_WPA_PSK == cred.sec_type)
                        {
                            WDRV_WINC_AuthCtxSetWPA(&authCtx, cred.passphrase, cred.passphrase_length);
                        }
                        else
                        {
                            // other type not considered for this demo. default to open.
                            WDRV_WINC_AuthCtxSetOpen(&authCtx);
                        }
                        if (WDRV_WINC_STATUS_OK == WDRV_WINC_BSSConnect(wdrvHandle, &bssCtx, &authCtx, &_APP_ConnectNotifyCallback))
                        {
                            printf("Wi-Fi Connecting......\r\n");
                            vTaskSuspend(xBle_Tasks);
                        }
                        /* Register callback for socket events. */
                        //WDRV_WINC_SocketRegisterEventCallback(handle, &APP_ExampleSocketEventCallback);
                        
                    }
                }
            }
            break;
        }
        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
