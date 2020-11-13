/*******************************************************************************
* File Name: lp_utilities.cpp
*
* Description: This file contains functions that perform modular roles like
* connnecting to a network, configuring a HTTP server and configuring the
* powersave mode of WLAN device.
*
********************************************************************************
* (c) 2019-20, Cypress Semiconductor Corporation. All rights reserved.
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
/*******************************************************************************
* Include header files
*******************************************************************************/
#include "mbed.h"
#include "HTTP_server.hpp"
#include "lp_utilities.h"


/*******************************************************************************
* Global Variables
*******************************************************************************/
DigitalOut led( ( PinName )USER_LED );

/* HTTP server object handle. */
HTTPServer *server;

/* HTTP message that is sent.*/
char response[] = "<html><head><title>Hello from Cypress</title></head>"
                  "<body>"
                      "<h1>Cypress webserver</h1>"
                      "<form action=\"/toggle\" method=\"post\">"
                      "<input type=\"submit\" value=\"Toggle LED\">"
                      "</form>"
                  "</body></html>";


#if ENABLE_STACK_STATS
extern mbed_stats_thread_t *thread_stats;
#endif


/*******************************************************************************
* Function definitions
*******************************************************************************/
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
cy_rslt_t wifi_connect_handler( WhdSTAInterface* wifi )
{
    uint32_t        tries;
    nsapi_error_t   net_status = NSAPI_ERROR_OK;
    SocketAddress   sock_addr;

    for ( tries = 0; tries < MAX_RETRIES; tries++ )
    {
        net_status = wifi->connect( MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2 );
        if ( NSAPI_ERROR_OK == net_status )
        {
            break;
        }
        else
        {
            ERR_INFO( ( "Unable to connect to network. Retrying...\r\n" ) );
        }
    }
    if ( NSAPI_ERROR_OK == net_status )
    {
        wifi->get_ip_address(&sock_addr);
        APP_INFO( ( "Connected to the network successfully. IP address: %s\r\n",
                    sock_addr.get_ip_address() ) );
    }
    else
    {
        ERR_INFO( ( "Connecting to the network failed ( %d )!\r\n",
                  net_status ) );
    }

    return net_status;
}


/*******************************************************************************
* Function Name: http_server_init
********************************************************************************
*
* Summary: This function configures a HTTP server page database and starts a
* HTTP server.
*
* Parameters:
* WhdSTAInterface* wifi: This is the pointer to the WLAN interface.
*
* Return:
* cy_rslt_t: It contains the status of starting a HTTP server.
*
*******************************************************************************/
cy_rslt_t http_server_init( WhdSTAInterface* wifi )
{
    cy_rslt_t                   result;
    cy_resource_static_data_t   test_data = {response, sizeof( response ) };
    cy_resource_dynamic_data_t  test_data_dynamic ={ dynamic_url_handler, NULL };
    cy_network_interface_t nw_interface;

    nw_interface.object = (void *)wifi;
    nw_interface.type   = CY_NW_INF_TYPE_WIFI;

    /* Initialize HTTP server object. */
    server = new HTTPServer(&nw_interface, HTTP_PORT, MAX_SOCKETS);

    /* Registers a STATIC_URL resource in the HTTP page database. The 
       resource can accessed through the URL http://<IP_ADDRESS>/.Accessing
       the resource results in a response written back to the HTTP stream as
       contained in structure, test_data.
    */
    result = server->register_resource( ( uint8_t* )"/", ( uint8_t* )"text/html", 
                                       CY_STATIC_URL_CONTENT, &test_data );
    if ( CY_RSLT_SUCCESS ==  result )
    {
        /* Registers a DYNAMIC_URL resource in the HTTP page database. The
           resource can accessed through the URL http://<IP_ADDRESS>/toggle.
           Accessing the resource results in execution of dynamic_url_handler
           which is stored in the structure, test_data_dynamic
        */
        result = server->register_resource( ( uint8_t* )"/toggle",
                                            ( uint8_t* )"text/html",
                                            CY_DYNAMIC_URL_CONTENT,
                                            &test_data_dynamic );
        if ( CY_RSLT_SUCCESS ==  result )
        {
            /* Start HTTP server */
            result = server->start();
            if ( CY_RSLT_SUCCESS != result )
            {
                ERR_INFO( ( "Starting HTTP server failed.\r\n" ) );
            }
            else
            {
                APP_INFO( ( "HTTP server started successfully.\r\n" ) );
            }
        }
        else
        {
            ERR_INFO( ( "Registering dynamic resource failed.\r\n" ) );
        }
    }
    else
    {
        ERR_INFO( ( "Registering static resource failed.\r\n" ) );
    }

    return result;
}


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
                             cy_http_message_body_t* http_data )
{

    cy_rslt_t result = CY_RSLT_SUCCESS;

    #if ENABLE_STACK_STATS
    static uint32_t count;
    uint32_t        threshold;
    if ( ( 0 == count ) || ( PRINT_AFTER_N_LOOPS == count ) )
    {

        APP_INFO( ( "================ THREAD STATS ===============\r\n" ) );
        threshold = mbed_stats_thread_get_each( thread_stats, MAX_THREAD_COUNT );
        for ( uint32_t i = 0; i < threshold; i++ )
        {
            APP_INFO( ( "ID: 0x%lx \r\n",        thread_stats[i].id ) );
            APP_INFO( ( "Name: %s \r\n",         thread_stats[i].name ) );
            APP_INFO( ( "State: %ld \r\n",       thread_stats[i].state ) );
            APP_INFO( ( "Priority: %ld \r\n",    thread_stats[i].priority ) );
            APP_INFO( ( "Stack Size: %ld \r\n",  thread_stats[i].stack_size ) );
            APP_INFO( ( "Stack Space: %ld \r\n", thread_stats[i].stack_space ) );
            APP_INFO( ( "=============================================\r\n" ) );
        }
        count = 0;
    }
    ++count;
    #endif

    #if ENABLE_SLEEP_STATS
    APP_INFO( ( "uptime_us: %llu, idle_us: %llu, sleep_us: %llu, dsleep_us: %llu"
              "\r\n",mbed_uptime(), mbed_time_idle(), mbed_time_sleep(),
               mbed_time_deepsleep() ) );
    #endif
    
    led = !led;

    result = server->http_response_stream_write( stream, response, sizeof( response ) );
    if( CY_RSLT_SUCCESS != result )
    {
        ERR_INFO( ( "Failed to write HTTP response\r\n" ) );
    }

    return result;
}


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
int32_t powersave_handler( WhdSTAInterface* wifi, wlan_powersave_mode_t mode )
{
    cy_rslt_t       result = CY_RSLT_SUCCESS;
    whd_interface_t ifp;
    whd_security_t  security_param = WHD_SECURITY_WPA2_MIXED_PSK;
    whd_bss_info_t  ap_info;

    /* Get the instance of the WLAN interface. This instance is used to obtain
       network parameters and to configure power-save mode of the device.
    */
    result = wifi->wifi_get_ifp( &ifp );
    if( CY_RSLT_SUCCESS == result )
    {
        /* The power-save handler can be called even before connecting to an AP
           to configure the power-save mode of WLAN device. But, the power-save
           mode can't be configured until the interface is powered. Hence, the
           check for ifp is performed here to ensure that the interface is
           powered and power-save mode can be successfully set.
        */
        if( ifp == NULL )
        {
            wifi->wifi_on( );
        }
        if( WHD_SUCCESS == wifi->is_interface_connected( ) )
        {
            result = whd_wifi_get_ap_info( ifp, &ap_info, &security_param );
            if( CY_RSLT_SUCCESS == result )
            {
                APP_INFO( ( "Beacon period = %d, DTIM period = %d\n",
                          ap_info.beacon_period, ap_info.dtim_period ) );
            }
            else
            {
                ERR_INFO( ( "Failed to get AP info.\r\n" ) );
            }
        }
        switch( mode )
        {
            case POWERSAVE_WITHOUT_THROUGHPUT:
                result = whd_wifi_enable_powersave( ifp );
                if( CY_RSLT_SUCCESS != result )
                {
                    ERR_INFO( ( "Failed to enable PM1 mode\r\n" ) );
                }
                break;
            case POWERSAVE_WITH_THROUGHPUT:
                result = whd_wifi_enable_powersave_with_throughput( ifp, RETURN_TO_SLEEP_MS );
                if( CY_RSLT_SUCCESS != result )
                {
                    ERR_INFO( ( "Failed to enable PM2 mode\r\n" ) );
                }
                break;
            case POWERSAVE_DISABLED:
                result = whd_wifi_disable_powersave( ifp );
                if( CY_RSLT_SUCCESS != result )
                {
                    ERR_INFO( ( "Failed to disable powersave\r\n" ) );
                }
                break;
            default:
                APP_INFO( ( "Unknown mode\r\n" ) );
                result = UNKNOWN_MODE;
                break;
        }
    }
    else
    {
        ERR_INFO( ( "WiFi interface is not obtained.\r\n" ) );
    }

    return result;
}