/*******************************************************************************
* File Name: lp_utilities.h
*
* Version: 1.0
*
* Description: This file includes the macros, enumerations and function
* prototypes used in lp_utilities.cpp
*
********************************************************************************
* (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/
#include "HTTP_server.h"
#include "whd_wifi_api.h"
#include "whd_types.h"
#include "WhdSTAInterface.h"

#define APP_INFO( x )           printf("Info:"); printf x
#define ERR_INFO( x )           printf("Error:"); printf x

/* HTTP server macros */
#define HTTP_PORT               (80u)
#define MAX_SOCKETS             (2u)
#define MAX_RETRIES             (3u)

/* Set the macro to 1 to print sleep statistics on the serial terminal.*/
#define ENABLE_SLEEP_STATS      (1u)
/* Set the macro to 1 to print stack statistics on the serial terminal. */
#define ENABLE_STACK_STATS      (0u)
#define PRINT_AFTER_N_LOOPS     (5u)
#define MAX_THREAD_COUNT        (16u)

#define UNKNOWN_MODE            (-1)

#define concat_(x,y) x##y
#define concat(x,y) concat_(x,y)
#define SECURITY concat( NSAPI_SECURITY_,MBED_CONF_NSAPI_DEFAULT_WIFI_SECURITY )

/* This macro specifies the duration in milliseconds for which the STA stays
   awake after receiving frames from AP in PM2 mode. The delay value must be set
   to a multiple of 10 and not equal to zero. Minimum value is 10u. Maximum
   value is 2000u.
*/
#define RETURN_TO_SLEEP_MS      (10u)

/* This data type enlists enumerations that correspond to the different 
   power-save modes available. They are,
   POWERSAVE_WITHOUT_THROUGHPUT:This mode corresponds to (legacy) 802.11 PS-Poll
   mode and should be used to achieve the lowest power consumption possible when
   the Wi-Fi device is primarily passively listening to the network

   POWERSAVE_WITH_THROUGHPUT:This mode attempts to increase throughput by
   waiting for a timeout period before returning to sleep rather than returning
   to sleep immediately.

   POWERSAVE_DISABLED: Powersave mode is disabled.
*/
enum wlan_powersave_mode_t
{
    POWERSAVE_WITHOUT_THROUGHPUT,
    POWERSAVE_WITH_THROUGHPUT,
    POWERSAVE_DISABLED
};

/*******************************************************************************
* Function Name: wifi_connect_handler
********************************************************************************
*
* Summary: This function connects the device to a specified AP whose credentials
* are provided in the mbed_app.json.
*
* Parameters:
* WhdSTAInterface* wifi: This is the pointer to the WLAN interface whose
* power-save mode is to be configured.
*
* Return:
* cy_rslt_t: It contains the status of operation of connecting to the
* specified AP.
*
*******************************************************************************/
cy_rslt_t wifi_connect_handler( WhdSTAInterface* wifi );

/*******************************************************************************
* Function Name: powersave_handler
********************************************************************************
*
* Summary: This function configures a HTTP server page database and starts a
* HTTP server.
*
* Parameters:
* WhdSTAInterface* wifi: This is the pointer to the WLAN interface whose
* power-save mode is to be configured.
*
* Return:
* cy_rslt_t: It contains the status of starting a HTTP server.
*
*******************************************************************************/
cy_rslt_t http_server_init( WhdSTAInterface* wifi, HTTPServer* server);

/*******************************************************************************
* Function Name: dynamic_url_handler
********************************************************************************
*
* Summary: This is the DYNAMIC URL Handler for the HTTP page database resource
* with the URL:http://<IP_ADDRESS>/toggle. This function toggles the LED and
* writes the HTTP response.
*
* Parameters: 
* const char* url_path: UNUSED.
* const char* url_query_string: UNUSED.
* cy_http_response_stream_t* stream: It is the HTTP stream to which the HTTP
* message is written.
* void* arg: UNUSED.
* cy_http_message_body_t* http_data: UNUSED.
*
* Return:
* int32_t: It contains the status of the dynamic url handler.
*
*******************************************************************************/
int32_t dynamic_url_handler( const char* url_path, const char* url_query_string,
                             cy_http_response_stream_t* stream, void* arg, 
                             cy_http_message_body_t* http_data );

/*******************************************************************************
* Function Name: powersave_handler
********************************************************************************
*
* Summary: This function configures the power-save mode of the WLAN device.
* There are three power-save modes supported as defined in the enumeration,
* wlan_powersave_mode_t.
*
* Parameters:
* WhdSTAInterface* wifi: This is the pointer to the WLAN interface whose
* power-save mode is to be configured.
* wlan_powersave_mode_t mode: This enumeration contains information about the
* power-save mode which is to be configured for the WLAN interface.
*
* Return:
* int32_t: It contains the status of operation of configuring the WLAN
* interface's power-save mode.
*
*******************************************************************************/
int32_t powersave_handler( WhdSTAInterface* wifi, wlan_powersave_mode_t mode );