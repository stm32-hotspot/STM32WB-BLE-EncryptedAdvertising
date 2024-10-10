## __BLE_p2pClient_EAD_Ext Application Description__

Demonstrate STM32WB acting as BLE central and GATT client.

p2pClient_EAD_Ext application scans and connects to p2pServer_EAD_Ext device.

Once connected and bonded, p2pClient_EAD_Ext can read the Encrypted Data Key Material characteristic of the p2pServer_EAD_Ext and use it to decrypt encrypted advertising data.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, extended

### __Directory contents__

  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/stm32wbxx_it.h          Interrupt handlers header file
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/stm32_lpm_if.h          Header for stm32_lpm_if.c module (device specific LP management)
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/main.h                  Header for main.c module 
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/app_common.h            Header for all modules with common definition
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/app_conf.h              Parameters configuration file of the application
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/app_debug.h             Header for app_debug.c module
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/app_entry.h             Interface to the application
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/flash_driver.h          Dual core Flash driver interface
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/stm32wbxx_nucleo_conf.h Discovery Module configuration file
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/hw_conf.h               Configuration file of the HW
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/hw_if.h                 Hardware Interface
  - BLE/BLE_p2pClient_EAD_Ext/Core/Inc/utilities_conf.h        Configuration file of the utilities  
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/stm32wbxx_it.c          Interrupt handlers
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/main.c                  Main program
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/stm32wbxx_hal_msp.c     This file provides code for the MSP Initialization 
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/system_stm32wbxx.c      stm32wbxx system source file
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/app_entry.c             Initialization of the application
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/app_debug.c           Debug capabilities source file for STM32WPAN Middleware
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/stm32_lpm_if.c          Low Power Manager Interface
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/hw_timerserver.c        Timer Server based on RTC
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/hw_uart.c               UART Driver
  - BLE/BLE_p2pClient_EAD_Ext/Core/Src/flash_driver.c          Dual core Flash driver interface
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/app_ble.c         BLE Profile implementation
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/app_ble.h         Header for app_ble.c module
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/ble_conf.h        BLE Services configuration
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/ble_dbg_conf.h    BLE Traces configuration of the BLE services
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/p2p_client_app.h  Header for p2p_lcient_app.c module
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/p2p_client_app.c  P2P Client Application Implementation
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/App/tl_debug_conf.h   Debug configuration file for stm32wpan transport layer interface
  - BLE/BLE_p2pClient_EAD_Ext/STM32_WPAN/Target/hw_ipcc.c      IPCC Driver
  

### __Hardware and Software environment__

  - This example runs on STM32WB55xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
  - Another STM32WBxx Nucleo board may be necessary to run p2pServer_Ext application.
    
### __How to use it ?__

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Power up p2pClient_EAD_Ext devices next to a p2pServer_EAD_Ext device. It launches a scan. Scan is stopped if a p2pServer_EAD_Ext device is detected.
 - Press SW3 to initiate a connection. Discovery of service and characteristics. Exchange of max ATT_MTU, pairing and bonding.
 when devices are connected: 
 - Press SW2 to initiate a disconnection 
 when devices are not connected:
 - Press SW2 to clear security database and erase Flash.
 - Press SW1 to launch a scan. Encrypted Data are received and decrypted if keys are stored in RAM or Flash.

 - When disconnection occurs, p2pServer_EAD_Ext restarts advertising.
