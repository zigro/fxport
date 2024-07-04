/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// ---- add start
#include "user_uart.h"
#include "main.h"
// ---- add end
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// ---- add start
#define PRINT_IP_ADDRESS(addr)        printf("%lu.%lu.%lu.%lu", (addr >> 24) & 0xff, (addr >> 16) & 0xff, (addr >> 8) & 0xff, (addr & 0xff))
#define PRINT_DATA(addr, port, data)  printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \n", (addr >> 24) & 0xff, (addr >> 16) & 0xff, (addr >> 8) & 0xff, (addr & 0xff), port, data)
// ---- add end

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
/* USER CODE BEGIN PV */

// ---- add start
TX_THREAD AppUDPThread;   // UDP thread
TX_THREAD AppLinkThread;  // LAN link-check thread
TX_THREAD AppUartThread;  // UART thread

TX_THREAD AppTCPThread;   // TCP thread
TX_THREAD AppTCPSConMgrThread;  // TCPS Connection Manager thread
TX_THREAD AppTCPCthread;  // TCPC thread

TX_THREAD AppGpioThread;  // GPIO thread

ULONG IpAddr;             // my IP addr
ULONG NetMask;            // my netmask

NX_UDP_SOCKET UDPSocket;  // UDP socket
NX_TCP_SOCKET TCPSocket;  // TCP socket
NX_TCP_SOCKET TCPCsocket;  // TCP client socket

NX_PACKET_POOL AppUartPktPool;  // packet pool for send UDP pakcet in UART thread
// ---- add end

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
/* USER CODE BEGIN PFP */

// ---- add start
static VOID App_UDP_Thread_Entry(ULONG thread_input);   //UDP thread entry
static VOID App_Link_Thread_Entry(ULONG thread_input);  //LAN link-check thread entry
static VOID App_UART_Thread_Entry(ULONG thread_input);  //UART thread entry

static VOID App_TCP_Thread_Entry(ULONG thread_input);   //TCP thread entry
static VOID App_TCPS_ConMgr_Thread_Entry(ULONG thread_input);   //TCPS Connection Manager thread entry
static VOID App_TCPC_Thread_Entry(ULONG thread_input);   //TCP thread entry

static VOID App_GPIO_Thread_Entry(ULONG thread_input);   //GPIO thread entry
// ---- add end

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

   /* USER CODE BEGIN App_NetXDuo_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END App_NetXDuo_MEM_POOL */
  /* USER CODE BEGIN 0 */

  printf("FxPort Test start.\n");

  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  CHAR *pointer;
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */

  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */

  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the UDP protocol required for  DHCP communication */

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */
  
  // ---- add start
  // create threads in this section

  /* Katsumi Test Add >> */
  ULONG setIpAddr = IP_ADDRESS(192,168,1,55);
  ULONG setNetMask = IP_ADDRESS(255,255,255,0);
  ret = nx_ip_address_set(&NetXDuoEthIpInstance, setIpAddr, setNetMask);
  if ( ret != NX_SUCCESS )
  {

  }

  // create UDP thread
  // allocate thread memory area
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, 2 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  // create thread
  ret = tx_thread_create(&AppUDPThread, "App UDP Thread", App_UDP_Thread_Entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE, DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  // create LAN Link-check thread
  // allocate thread memory area
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,2 *  DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  //create thread
  ret = tx_thread_create(&AppLinkThread, "App Link Thread", App_Link_Thread_Entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE, LINK_PRIORITY, LINK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  // create UART thread
  // allocate thread memory area
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  //create thread
  ret = tx_thread_create(&AppUartThread, "App UART Thread", App_UART_Thread_Entry, 100, pointer, DEFAULT_MEMORY_SIZE, 9, 9, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  // allocate packet pool for UART thread
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_PAYLOAD_SIZE+sizeof(NX_PACKET), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  //create packet pool
  ret = nx_packet_pool_create(&AppUartPktPool, "App UART packet Pool", DEFAULT_PAYLOAD_SIZE, pointer, DEFAULT_PAYLOAD_SIZE+sizeof(NX_PACKET));
  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

  // create TCP thread
  // allocate thread memory area
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, 2 * DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  // create thread
  ret = tx_thread_create(&AppTCPThread, "App TCP Thread", App_TCP_Thread_Entry, 0, pointer, 2 * DEFAULT_MEMORY_SIZE, DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

// create TCP server connection manager thread
  // allocate thread memory area
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  // create thread
  ret = tx_thread_create(&AppTCPSConMgrThread, "App TCPS Connection Manager thread", App_TCPS_ConMgr_Thread_Entry, 0, pointer, DEFAULT_MEMORY_SIZE, DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

// create TCP client thread
  // allocate thread memory area
  ret = tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  // create thread
  ret = tx_thread_create(&AppTCPCthread, "App TCPC thread", App_TCPC_Thread_Entry, 0, pointer, DEFAULT_MEMORY_SIZE, DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }


  // create GPIO thread
  // allocate thread memory area
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  //create thread
  ret = tx_thread_create(&AppGpioThread, "App GPIO Thread", App_GPIO_Thread_Entry, 100, pointer, DEFAULT_MEMORY_SIZE, 13, 13, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }


  // ---- add end

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
	  // ---- add start
	  UINT ret = NX_SUCCESS;
	  ret = nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddr, &NetMask);
	  if ( ret != NX_SUCCESS )
	  {

	  }
//
//	  /* Pingを有効にする */
//	  ret = nx_icmp_enable(&NetXDuoEthIpInstance);
//	  if ( ret != NX_SUCCESS )
//	  {
//
//	  }

	  // print IP addr
	  printf("IP address : ");
	  PRINT_IP_ADDRESS(IpAddr);
	  printf("\n");

	  /* Now the network is correctly initialized, start the UDP server thread */
	  tx_thread_resume(&AppUDPThread);
	  // tx_thread_resume(&AppTCPThread);
	  tx_thread_resume(&AppTCPSConMgrThread);
	  // tx_thread_resume(&AppTCPCthread);

	  /* this thread is not needed any more, we relinquish it */
	  tx_thread_relinquish();

	  // ---- add end

  /* USER CODE END Nx_App_Thread_Entry 0 */

}
/* USER CODE BEGIN 1 */

// ---- add start


UINT USERTX_ThreadRelinquishWithSetLowestPriority( VOID )
{
  UINT oldPriority;
  UINT ret;

  //set lowest priority
  ret = tx_thread_priority_change(tx_thread_identify(), TX_MAX_PRIORITIES-1, &oldPriority);
  if ( ret != TX_SUCCESS )
  {
    return ret;
  }

  //relinquish control 
  tx_thread_relinquish();

  //restore priority
  return tx_thread_priority_change(tx_thread_identify(), oldPriority, &oldPriority);
}



uint8_t isUartDstTcp = 1;  //0:UART destination is UDP / 1:UART destination is TCP


// UDP thread entry
UINT  AppUdpThreadSrcPort;
ULONG AppUdpThreadSrcAddr;
UCHAR AppUdpThreadDatBuf[512];
extern UART_HandleTypeDef huart1;

static VOID App_UDP_Thread_Entry(ULONG thread_input)
{
  UINT ret;
  ULONG bytes_read;

  NX_PACKET *data_packet;

  /* create the UDP socket */
  ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket, "UDP Server Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, QUEUE_MAX_SIZE);
  if (ret != NX_SUCCESS)
  {
  }

  /* bind the socket indefinitely on the required port */
  ret = nx_udp_socket_bind(&UDPSocket, DEFAULT_PORT, TX_WAIT_FOREVER);
  if (ret != NX_SUCCESS)
  {
  }
  else
  {
    printf("UDP Server listening on PORT %d..\n", DEFAULT_PORT);
  }

  while(1)
  {
    TX_MEMSET(AppUdpThreadDatBuf, '\0', sizeof(AppUdpThreadDatBuf));

    /* wait for data for 1 sec */
    ret = nx_udp_socket_receive(&UDPSocket, &data_packet, 1000UL);

    if (ret == NX_SUCCESS)
    {
      /* data is available, read it into the data buffer */
      nx_packet_data_retrieve(data_packet, AppUdpThreadDatBuf, &bytes_read);

      /* get info about the client address and port */
      nx_udp_source_extract(data_packet, &AppUdpThreadSrcAddr, &AppUdpThreadSrcPort);

      /* print the client address, the remote port and the received data */
//      PRINT_DATA(AppUdpThreadSrcAddr, AppUdpThreadSrcPort, AppUdpThreadDatBuf);
      USERUART_Write(&huart1, AppUdpThreadDatBuf, bytes_read, 100UL);
      isUartDstTcp = 0;

//      /* resend the same packet to the client */
//      ret =  nx_udp_socket_send(&UDPSocket, data_packet, AppUdpThreadSrcAddr, AppUdpThreadSrcPort);
      // release received packet
      ret = nx_packet_release(data_packet);
    }
  }
}


// LAN Link-check thread entry
UINT AppLinkThleadLinkDown = 0;

static VOID App_Link_Thread_Entry(ULONG thread_input)
{
  ULONG actual_status;
  UINT status;

  while(1)
  {
    /* Get Physical Link status. */
    status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_LINK_ENABLED, &actual_status, 100UL);

    if(status == NX_SUCCESS)
    {
      if(AppLinkThleadLinkDown == 1)
      {
        AppLinkThleadLinkDown = 0;
        status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_ADDRESS_RESOLVED, &actual_status, 100UL);
        if(status == NX_SUCCESS)
        {
          /* The network cable is connected again. */
          printf("The network cable is connected again.\n");
        }
        else
        {
          /* The network cable is connected. */
          printf("The network cable is connected.\n");
          /* Send command to Enable Nx driver. */
          nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_ENABLE, &actual_status);
          /* Restart DHCP Client. */
          /* Katsumi Test Delete >> */
          // nx_dhcp_stop(&DHCPClient);
          // nx_dhcp_start(&DHCPClient);
          /* << Katsumi Test Delete */
        }
      }
    }
    else
    {
      if(0 == AppLinkThleadLinkDown)
      {
        AppLinkThleadLinkDown = 1;
        /* The network cable is not connected. */
        printf("The network cable is not connected.\n");
      }
    }

    tx_thread_sleep(1000UL);//600);
  }
}


// UART thread entry
UCHAR AppUartThreadDatBuf[512];


static VOID App_UART_Thread_Entry(ULONG thread_input)
{
  while ( 1 )
  {
    USERTX_ThreadRelinquishWithSetLowestPriority();

    //get UART rx bytes num
    int32_t rxCount = USERUART_ReceiveCount(&huart1);

    if ( rxCount > 0 )
    {
      if ( rxCount > sizeof(AppUartThreadDatBuf) )
      {
        rxCount = sizeof(AppUartThreadDatBuf);
      }
      int32_t readResult = USERUART_Read(&huart1, AppUartThreadDatBuf, rxCount, 100UL);
      if ( readResult > 0 )
      {
        NX_PACKET* nxPktPtr;

        if ( isUartDstTcp )
        {
          ULONG socketState = 0xFFFFFFFFUL;
          nx_tcp_socket_info_get(&TCPSocket, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, &socketState, NX_NULL, NX_NULL, NX_NULL);
          if ( socketState != NX_TCP_ESTABLISHED )
          {
            continue;
          }

          UINT nxSts = nx_packet_allocate(&AppUartPktPool, &nxPktPtr, NX_IPv4_TCP_PACKET, 1000UL);
          if ( nxSts != NX_SUCCESS )
          {
            __NOP();
          }
          nxSts = nx_packet_data_append(nxPktPtr, AppUartThreadDatBuf, rxCount, &AppUartPktPool, NX_NO_WAIT);

          UINT ret =  nx_tcp_socket_send(&TCPSocket, nxPktPtr, 100);
        }
        else
        {
          UINT nxSts = nx_packet_allocate(&AppUartPktPool, &nxPktPtr, NX_IPv4_UDP_PACKET, 1000UL);
          if ( nxSts != NX_SUCCESS )
          {
            __NOP();
          }
          nxSts = nx_packet_data_append(nxPktPtr, AppUartThreadDatBuf, rxCount, &AppUartPktPool, NX_NO_WAIT);

          UINT ret =  nx_udp_socket_send(&UDPSocket, nxPktPtr, AppUdpThreadSrcAddr, AppUdpThreadSrcPort);
        }

      }
    }
  }
}


// TCP thread entry
UINT  AppTcpThreadSrcPort;
ULONG AppTcpThreadSrcAddr;
UCHAR AppTcpThreadDatBuf[512];

TX_SEMAPHORE AppTcpSemaphoe;

#if 0
VOID AppTcpCallback_Listen(NX_TCP_SOCKET *socket_ptr, UINT port)
{
  UINT ret = nx_tcp_server_socket_accept(&TCPSocket, 300);
}

VOID AppTcpCallback_Disconnect(NX_TCP_SOCKET *socket_ptr)
{
  // received FIN+ACK, and send ACK
  // now TCP status is CLOSE_WAIT

  UINT ret = nx_tcp_socket_disconnect(&TCPSocket, 100); // send FIN+ACK

  ret = nx_tcp_server_socket_unaccept(&TCPSocket);  // send RST, if before receive ACK
  ret = nx_tcp_server_socket_relisten(&NetXDuoEthIpInstance, 60001U, &TCPSocket);
}


static VOID App_TCPS_ConMgr_Thread_Entry(ULONG thread_input)
{
}


static VOID App_TCP_Thread_Entry(ULONG thread_input)
{
  // create the TCP socket
  UINT socketRet = nx_tcp_socket_create(&NetXDuoEthIpInstance, &TCPSocket, "TCP Server Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1460, NX_NULL, AppTcpCallback_Disconnect);
  if ( socketRet != NX_SUCCESS )
  {
    tx_thread_suspend(tx_thread_identify());
  }

  while ( 1 )
  {
    USERTX_ThreadRelinquishWithSetLowestPriority();

    // check socket state
    ULONG socketState = 0xFFFFFFFFUL;
    UINT socketRet = nx_tcp_socket_info_get(&TCPSocket, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, &socketState, NX_NULL, NX_NULL, NX_NULL);

    // if TCP socket closed, start listen
    if ( socketState == NX_TCP_CLOSED )
    {
      socketRet = nx_tcp_server_socket_listen(&NetXDuoEthIpInstance, 60001U, &TCPSocket, 1, AppTcpCallback_Listen);
    }

    // wait TCP accept complete
    // nx_tcp_socket_state_wait(&TCPSocket, NX_TCP_ESTABLISHED, 100);
    if ( socketState != NX_TCP_ESTABLISHED )
    {
      continue;
    }
    // TCP is Connected

    NX_PACKET *data_packet;
    ULONG bytes_read;

    // check TCP received
    TX_MEMSET(AppTcpThreadDatBuf, '\0', sizeof(AppTcpThreadDatBuf));
    socketRet = nx_tcp_socket_receive(&TCPSocket, &data_packet, 100);
    if ( socketRet != NX_SUCCESS )
    {
      continue;
    }

    // TCP received
    socketRet = nx_packet_data_retrieve(data_packet, AppTcpThreadDatBuf, &bytes_read);
    // socketRet = nx_tcp_socket_send(&TCPSocket, data_packet, NX_NO_WAIT);
    USERUART_Write(&huart1, AppTcpThreadDatBuf, bytes_read, 100UL);
    nx_packet_release(data_packet);
    tx_thread_resume(&AppTCPCthread);
    isUartDstTcp = 1;
  }
}
#else
UINT TcpServerPort = 60001;

VOID AppTcpCallback_Listen(NX_TCP_SOCKET *socket_ptr, UINT port)
{
  // received SYN
  // now TCP status is SYN_RECEIVED

  UINT ret = nx_tcp_server_socket_accept(socket_ptr, 3000UL);  // send SYN+ACK
  if ( ret != NX_SUCCESS )
  {
    ret = nx_tcp_server_socket_unaccept(socket_ptr);
  }
  else
  {
    printf("Accept Success");
  }
}

VOID AppTcpCallback_Disconnect(NX_TCP_SOCKET *socket_ptr)
{
  // received FIN+ACK, and send ACK
  // now TCP status is CLOSE_WAIT

  // ret = nx_tcp_server_socket_unaccept(socket_ptr);  // send RST, if before receive ACK
  // ret = nx_tcp_server_socket_relisten(&NetXDuoEthIpInstance, TcpServerPort, socket_ptr);
}


static VOID App_TCPS_ConMgr_Thread_Entry(ULONG thread_input)
{
  // create the TCP socket
  UINT socketRet = nx_tcp_socket_create(&NetXDuoEthIpInstance, &TCPSocket, "TCP Server Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1460, NX_NULL, AppTcpCallback_Disconnect);
  if ( socketRet != NX_SUCCESS )
  {
    tx_thread_suspend(tx_thread_identify());
  }

  while ( 1 )
  {
    USERTX_ThreadRelinquishWithSetLowestPriority();

    // check socket state
    ULONG socketState = 0xFFFFFFFFUL;
    socketRet = nx_tcp_socket_info_get(&TCPSocket, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, &socketState, NX_NULL, NX_NULL, NX_NULL);

    // if TCP socket is connected, resume TCPS thread
    if ( socketState == NX_TCP_ESTABLISHED )
    {
      tx_thread_resume(&AppTCPThread);
    }
    else
    {
      tx_thread_suspend(&AppTCPThread);
    }

    // if TCP socket closed, start listen
    if ( socketState == NX_TCP_CLOSED )
    {
      socketRet = nx_tcp_server_socket_listen(&NetXDuoEthIpInstance, TcpServerPort, &TCPSocket, 1, AppTcpCallback_Listen);
    }
    // if TCP received disconnect, restart listen
    else if ( socketState == NX_TCP_CLOSE_WAIT )
    {
      socketRet = nx_tcp_socket_disconnect(&TCPSocket, 3000UL); // send FIN+ACK
      // now TCP status is LAST_ACK

      socketRet = nx_tcp_socket_state_wait(&TCPSocket, NX_TCP_LISTEN_STATE, 3000UL);
      
      socketRet = _nxe_tcp_server_socket_unaccept(&TCPSocket);
      // socketRet = _nxe_tcp_server_socket_unlisten(&NetXDuoEthIpInstance, TcpServerPort);
      socketRet = _nxe_tcp_server_socket_relisten(&NetXDuoEthIpInstance, TcpServerPort, &TCPSocket);
    }

  }

}


static VOID App_TCP_Thread_Entry(ULONG thread_input)
{
  while ( 1 )
  {
    NX_PACKET *data_packet;
    ULONG bytes_read;

    USERTX_ThreadRelinquishWithSetLowestPriority();

    // check TCP received
    TX_MEMSET(AppTcpThreadDatBuf, '\0', sizeof(AppTcpThreadDatBuf));
    UINT socketRet = nx_tcp_socket_receive(&TCPSocket, &data_packet, 1000UL);
    if ( socketRet != NX_SUCCESS )
    {
      continue;
    }

    // TCP received
    socketRet = nx_packet_data_retrieve(data_packet, AppTcpThreadDatBuf, &bytes_read);
    // socketRet = nx_tcp_socket_send(&TCPSocket, data_packet, NX_NO_WAIT);
    USERUART_Write(&huart1, AppTcpThreadDatBuf, bytes_read, 100UL);
    nx_packet_release(data_packet);
    tx_thread_resume(&AppTCPCthread);
    isUartDstTcp = 1;
  }
}
#endif


ULONG TcpcDstIp = IP_ADDRESS(192, 168, 1, 22);
UINT  TcpcDstPort = 60002;


VOID AppTcpcCallback_Disconnect(NX_TCP_SOCKET *socket_ptr)
{
  // received FIN+ACK, and send ACK

  __NOP();
}


static VOID App_TCPC_Thread_Entry(ULONG thread_input)
{
  // create the TCP socket
  UINT nxRet = nx_tcp_socket_create(&NetXDuoEthIpInstance, &TCPCsocket, "TCP Client Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 1460, NX_NULL, AppTcpcCallback_Disconnect);
  if ( nxRet != NX_SUCCESS )
  {
    tx_thread_suspend(tx_thread_identify());
  }

  UINT port = 50000;
  while ( 1 )
  {
//  USERTX_ThreadRelinquishWithSetLowestPriority();
//    tx_thread_sleep(200);

    // check socket state
    ULONG socketState = 0xFFFFFFFFUL;
    nxRet = nx_tcp_socket_info_get(&TCPCsocket, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, NX_NULL, &socketState, NX_NULL, NX_NULL, NX_NULL);

    //if TCP scoket closed, start connect
    if ( socketState == NX_TCP_CLOSED )
    {
      //check bind port
      nxRet = nx_tcp_client_socket_port_get(&TCPCsocket, &port);
      if ( nxRet == NX_NOT_BOUND )
      {
        // find free port for myself
        nxRet = nx_tcp_free_port_find(&NetXDuoEthIpInstance, port, &port);
        if ( nxRet != NX_SUCCESS )
        {
          continue;
        }

        // bind port
        nxRet = nx_tcp_client_socket_bind(&TCPCsocket, port, 1000UL);
        if ( nxRet != NX_SUCCESS )
        {
          continue;
        }
      }

      // connect to destination
      nxRet = nx_tcp_client_socket_connect(&TCPCsocket, TcpcDstIp, TcpcDstPort, NX_WAIT_FOREVER);
      if ( nxRet != NX_SUCCESS )
      {
        continue;
      }
    }

    // wait TCP connected
    nxRet = nx_tcp_socket_state_wait(&TCPCsocket, NX_TCP_ESTABLISHED, 3000UL);
    if ( nxRet != NX_SUCCESS )
    {
      continue;
    }

    // send adata
    NX_PACKET* nxPktPtr;
    nxRet = nx_packet_allocate(&AppUartPktPool, &nxPktPtr, NX_IPv4_TCP_PACKET, 1000UL);
    if ( nxRet != NX_SUCCESS )
    {
      continue;
    }
    nxRet = nx_packet_data_append(nxPktPtr, "TCPC data", sizeof("TCPC data"), &AppUartPktPool, NX_NO_WAIT);
    nxRet = nx_tcp_socket_send(&TCPCsocket, nxPktPtr, 3000UL);

    // close socket
    nxRet = nx_tcp_socket_disconnect(&TCPCsocket, 3000UL);

    // wait TCP TIME_WAIT
    nxRet = nx_tcp_socket_state_wait(&TCPCsocket, NX_TCP_TIMED_WAIT, 3000UL);
    if ( nxRet != NX_SUCCESS )
    {
      continue;
    }

    // unbind port
    nxRet = nx_tcp_client_socket_unbind(&TCPCsocket);
    if ( nxRet != NX_SUCCESS )
    {
      continue;
    }

    port = (port+1 == 0) ? 50000 : port+1;
    tx_thread_suspend(tx_thread_identify());
  }
}


//GPIO & HalTick test Thread
static VOID App_GPIO_Thread_Entry( ULONG thread_input )
{
//  uint32_t ledTickStart[3] = { 0, 0, 0};
//  const uint32_t ledWait[3] = { 1000UL/2, 2000UL/2, 3000UL/2 };
//  const GPIO_TypeDef* ledGpioPort[3] = { LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port };
//  const uint16_t ledGpioPin[3] = { LED1_Pin, LED2_Pin, LED3_Pin };
//
//  for ( uint8_t i=0 ; i<3 ; i++ )
//  {
//    ledTickStart[i] = HAL_GetTick();
//  }
//
//  while ( 1 )
//  {
//    USERTX_ThreadRelinquishWithSetLowestPriority();
//
//    for ( uint8_t j=0 ; j<3 ; j++ )
//    {
//      //elapsed wait tick
//      if ( HAL_GetTick() - ledTickStart[j] >= ledWait[j] )
//      {
//        HAL_GPIO_TogglePin(ledGpioPort[j], ledGpioPin[j]);
//        ledTickStart[j] = HAL_GetTick();
//      }
//    }
//
//  }
}

// ---- add end

/* USER CODE END 1 */
