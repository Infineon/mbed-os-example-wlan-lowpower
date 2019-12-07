/*******************************************************************************
* File Name: network_activity_handler.h
*
* Version: 1.0
*
* Description: This file contains the macros, enumerations and function
* prototypes used by network_activity_handler.cpp.
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

#define NW_INFO( x )      printf x

#define RX_EVENT_FLAG     ( 1UL << 0 )
#define TX_EVENT_FLAG     ( 1UL << 1 )

/* This enumeration enlists the different states of the network stack */
enum network_stack_state_t
{
    ST_SUCCESS,
    ST_WAIT_TIMEOUT_EXPIRED,
    ST_WAIT_INACTIVITY_TIMEOUT_EXPIRED,
    ST_WAIT_ACTIVITY_TIMEOUT_EXPIRED,
    ST_BAD_ARGS,
    ST_BAD_STATE,
    ST_CONNECT_FAILED,
    ST_DISCONNECT_FAILED,
    ST_WIFI_NET_SUSPENDED_FAILED,
    ST_WIFI_NET_RESUMING_FAILED,
    ST_NET_ACTIVITY
};

/* Forward Declarations */
class WhdSTAInterface;

/*******************************************************************************
* Function Name: wait_net_suspend
********************************************************************************
*
* Summary:
* In this function the network is monitored for inactivity in an interval of
* length inactive_interval_ms. If the network is inactive for a continuous
* duration specified by inactive_window_ms, the TCP/IP network stack is
* suspended and stays suspended until either there is emac activity or a
* duration of activity_interval_ms has expired. Network stack is resumed on
* either detecting emac activity or if activity_interval_ms time has elapsed.
*
* Parameters:
* WhdSTAInterface *wifi: pointer to WLAN interface
* uint32_t wait_ms: The interval for which the network is monitored
* for activity. Network stack is resumed ether at the end of
* activity_interval_ms or on emac activity.
* uint32_t inactive_interval_ms: The interval for which the network is monitored
* for inactivity.
* uint32_t inactive_window_ms: The continuous duration for which network has to
* be inactive in inactive_interval_ms.
*
* Return:
* int32_t: contains status of network stack suspension and corresponding resumption
*
*******************************************************************************/
int32_t wait_net_suspend(WhdSTAInterface *wifi, uint32_t wait_ms,
        uint32_t inactive_interval_ms, uint32_t inactive_window_ms);
