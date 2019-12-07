/*******************************************************************************
* File Name: main.cpp
*
* Version: 1.0
*
* Description: This application demonstrates low power operation of the Host MCU
* and the WLAN device. The application connects to a network whose credentials
* are provided in mbed_app.json. After connecting to the network successfully,
* it starts an HTTP server. The application configures the WLAN device in the
* specified powersave mode and sets the Host MCU in an extended wait state,
* waiting for HTTP requests. The Host MCU is in deepsleep/sleep during
* extended wait state.
*
* Related Document: README.md
*                   AN227910 Low-Power System Design with CYW43012 and PSoC 6
*
* Supported Kits (Target Names):
*   CY8CKIT-062S2-43012 PSoC 6 WiFi-BT Pioneer Kit (CY8CKIT_062S2_43012)
*   CY8CKIT-062-WiFi-BT PSoC 6 WiFi-BT Pioneer Kit (CY8CKIT_062_WIFI_BT)
*   CY8CPROTO-062-4343W PSoC 6 Wi-Fi BT Prototyping Kit (CY8CPROTO_062_4343W)
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

#include "mbed.h"
#include "lp_utilities.h"
#include "network_activity_handler.h"

/* This macro specifies the interval in milliseconds that the device monitors
   the network for inactivity. If the network is inactive for duration lesser 
   than INACTIVE_WINDOW_MS in this interval, the MCU does not suspend the network 
   stack and informs the calling function that the MCU wait period timed out 
   while waiting for network to become inactive.
*/
#define INACTIVE_INTERVAL_MS    (300u)

/* This macro specifies the continuous duration in milliseconds for which the 
   network has to be inactive. If the network is inactive for this duration, the
   MCU will suspend the network stack. Now, the MCU will not need to service the
   network timers which allows it to stay longer in sleep/deepsleep.
*/
#define INACTIVE_WINDOW_MS      (200u)

/* This macro defines the power-save mode that the WLAN device has to be
   configured to. The valid values for this macro are POWERSAVE_WITH_THROUGHPUT,
   POWERSAVE_WITHOUT_THROUGHPUT, POWERSAVE_WITH_THROUGHPUT, POWERSAVE_DISABLED
   which are defined in the enumeration wlan_powersave_mode_t.*/
#define POWERSAVE_SELECT        POWERSAVE_WITH_THROUGHPUT

#if ENABLE_STACK_STATS
mbed_stats_thread_t *thread_stats;
#endif


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
* System entry point. This function configures the WLAN device, connects
* to a network whose credentials are provided in the mbed_app.json. After
* successfully connecting to the network, it configures an HTTP server page
* database and starts an HTTP server. Then, it configures the power-save mode of
* the WLAN device and suspends the network stack while waiting for HTTP request.
* The MCU resumes the network stack when there is network activity. If the
* network activity is due to HTTP request, it responds to them as defined in the
* page database.
*
* Return: int 
*
*******************************************************************************/
int main()
{
    cy_rslt_t                   result = CY_RSLT_SUCCESS;
    cy_network_interface_t      nw_interface;
    WhdSTAInterface             *wifi = NULL;
    HTTPServer                  *http_s = NULL;

    #if ENABLE_STACK_STATS
    thread_stats = new mbed_stats_thread_t[MAX_THREAD_COUNT];
    #endif

    APP_INFO( ( "Connecting to the network using Wifi...\r\n" ) );

    wifi = new WhdSTAInterface( );

    result = wifi_connect_handler( wifi );
    nw_interface.object = ( void* ) wifi;
    nw_interface.type   = CY_NW_INF_TYPE_WIFI;
    if( CY_RSLT_SUCCESS == result )
    {
        http_s = new HTTPServer( &nw_interface, HTTP_PORT, MAX_SOCKETS );
        result = http_server_init( wifi, http_s );
        if( CY_RSLT_SUCCESS == result )
        {
            /* Configures the WLAN interface, pointed by wifi, to desired
               power-save mode.
            */
            result = powersave_handler( wifi, POWERSAVE_SELECT );
            if( CY_RSLT_SUCCESS == result )
            {
                /* Configures an emac activity callback to the WLAN 
                   interface and suspends the network if the network is inactive
                   for a duration of INACTIVE_WINDOW_MS inside an interval of
                   INACTIVE_INTERVAL_MS. The callback is used to signal the 
                   presence/absence of network activity to resume/suspend the 
                   network stack.
                */
                while( true )
                {
                    result = wait_net_suspend( wifi, osWaitForever,
                                               INACTIVE_INTERVAL_MS,
                                               INACTIVE_WINDOW_MS);
                }
            }
        }
    }

    return result;
}

