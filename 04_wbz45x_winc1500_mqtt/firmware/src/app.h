/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"
#include "osal/osal_freertos_extend.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

#define APP_MQTT


#define NOTIFY_STATE_PROVISIONFAILED	(0)
#define NOTIFY_STATE_WIFICONNECTING		(1)
#define NOTIFY_STATE_PROVISIONED		(2)

#define WLAN_CHANNEL        WDRV_WINC_ALL_CHANNELS /* WINC1500's Working Channel e.g. 1, 6, 11 or WDRV_WINC_ALL_CHANNELS*/

/** @brief maximum ssid length */
#define WIFI_PROVISION_MAX_SSID_LENGTH					(32)
/** @brief maximum pass phrase length */
#define WIFI_PROVISION_MAX_PASS_LENGTH					(64)

/** @brief maximum ssid length */
#define WIFI_PROVISION_MAX_SSID_LENGTH					(32)
/** @brief maximum pass phrase length */
#define WIFI_PROVISION_MAX_PASS_LENGTH					(64)

#define WIFI_PROV_CRED_SECTYPE_POS	(0)  //0 - 1 byte security type
#define WIFI_PROV_CRED_SSIDLEN_POS	(WIFI_PROV_CRED_SECTYPE_POS + 1)  //1 - 1 byte ssid length
#define WIFI_PROV_CRED_SSID_POS		(WIFI_PROV_CRED_SSIDLEN_POS + 1)  // 2 - ssid (max len 30) occupies bytes 2-31
#define WIFI_PROV_CRED_PASSLEN_POS	(WIFI_PROV_CRED_SSID_POS + WIFI_PROVISION_MAX_SSID_LENGTH) // 34 - 1 byte passphrase length
#define WIFI_PROV_CRED_PASS_POS		(WIFI_PROV_CRED_PASSLEN_POS + 1) //35 - passphrase (max len 30) occupies byte 35-65


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_STATE_INIT=0,
    APP_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */

} APP_STATES;

typedef enum APP_MsgId_T
{
    APP_MSG_BLE_STACK_EVT,
    APP_MSG_BLE_STACK_LOG,
    APP_MSG_ZB_STACK_EVT,
    APP_MSG_ZB_STACK_CB,
    APP_MSG_BLE_PROV_COMPLETE,
    APP_MSG_BLE_PROV_FAILED,
    APP_MSG_WDRV_INIT,
    APP_MSG_WDRV_CONNECT,
    APP_MSG_MQTT,
    APP_MSG_STACK_END
} APP_MsgId_T;

typedef struct APP_Msg_T
{
    uint8_t msgId;
    uint8_t msgData[256];
} APP_Msg_T;

typedef struct
{
	uint8_t sec_type;
	uint8_t ssid_length;
	uint8_t ssid[WIFI_PROVISION_MAX_SSID_LENGTH];
	uint8_t passphrase_length;
	uint8_t passphrase[WIFI_PROVISION_MAX_PASS_LENGTH];
} credentials;

extern credentials cred;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    /* TODO: Define any additional data used by the application. */
    OSAL_QUEUE_HANDLE_TYPE appQueue;
    
    SYS_CONSOLE_HANDLE consoleHandle;
    
//    TCPIP_NET_HANDLE netHandleWiFi;
//    
//    IPV4_ADDR netIpWiFi;

} APP_DATA;

extern APP_DATA appData;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

typedef enum
{
    APP_WIFI_CONNECT ,
    APP_WIFI_DISCONNECT,
    APP_WIFI_PROVISION,
}APP_CALLBACK_NOTIFY;


void APP_Tasks( void );

typedef void (*APP_CALLBACK) (APP_CALLBACK_NOTIFY value,void *data);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */

