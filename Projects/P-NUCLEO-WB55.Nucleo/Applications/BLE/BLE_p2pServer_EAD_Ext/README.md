## __BLE_p2pServer_EAD_Ext Application Description__

How to demonstrate Point-to-Point communication using BLE (as GATT server).

p2pServer_EAD_Ext application advertises and waits for a connection from:

 - p2pClient_EAD_Ext application running on STM32WBxx devices

Once connected and bonded, p2pClient_EAD_Ext can read the Encrypted Data Key Material characteristic of p2pServer_EAD_Ext.
p2pServer_EAD_Ext encrypt advertising data and build advertising event.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/stm32wbxx_hal_conf.h       HAL configuration file
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/stm32wbxx_it.h             Interrupt handlers header file
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/main.h                     Header for main.c module
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/app_ble.h            Header for app_ble.c module
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/app_common.h               Header for all modules with common definition
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/app_conf.h                 Parameters configuration file of the application
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/app_debug.h                Header for app_debug.c module
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/app_entry.h                Interface to the application
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/stm32wbxx_nucleo_conf.h    Discovery Module configuration file
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/hw_conf.h                  Configuration file of the HW
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/hw_if.h                    Hardware Interface
  - BLE/BLE_p2pServer_EAD_Ext/Core/Inc/utilities_conf.h           Configuration file of the utilities 
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/stm32wbxx_it.c             Interrupt handlers
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/main.c                     Main program
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/system_stm32wbxx.c         stm32wbxx system source file
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/app_entry.c                Initialization of the application
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/app_debug.c           Debug capabilities source file for STM32WPAN Middleware
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/stm32_lpm_if.c             Low Power Manager Interface
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/hw_timerserver.c           Timer Server based on RTC
  - BLE/BLE_p2pServer_EAD_Ext/Core/Src/hw_uart.c                  UART Driver
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/ble_conf.h           BLE Services configuration
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/ble_dbg_conf.h       BLE Traces configuration of the BLE services
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/p2p_server_app.h     Header for p2p_server_app.c module
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/adv_ext_app.h        Header for adv_ext_app.c module
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/app_ble.c            BLE Profile implementation
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/app_ble.h            Header for app_ble.c module 
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/p2p_server_app.c     P2P Server application
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/adv_ext_app.c        Advertising Extended application
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/App/tl_debug_conf.h      Debug configuration file for stm32wpan transport layer interface
  - BLE/BLE_p2pServer_EAD_Ext/STM32_WPAN/Target/hw_ipcc.c         IPCC Driver

### __Hardware and Software environment__

  - This application runs on STM32WB55 Nucleo board.
  - Another STM32WBxx Nucleo board is necessary to run p2pClient_EAD_Ext application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Power up p2pServer_EAD_Ext device, advertising is started.
 - Power up p2pClient_EAD_Ext devices next to p2pServer_EAD_Ext device. 

 - On p2pClient_EAD_Ext device, press SW1 to launch a scan. Scan is stopped if a p2pServer_EAD_Ext device is detected.
 - Press SW3 to initiate a connection. Discovery of service and characteristics. Exchange of max ATT_MTU, pairing and bonding. Store Keys in Flash.
 when devices are connected: 
 - Press SW2 to initiate a disconnection 
 when devices are not connected:
 - Press SW2 to clear security database and erase Flash.

 - Press SW1 to launch a scan. Encrypted Data are received and decrypted if keys are stored in RAM or Flash.
 
 
 - When disconnection occurs, p2pServer_EAD_Ext restarts advertising.
