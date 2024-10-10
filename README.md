# Encrypted Advertising encryption/decryption
 
Two applications are included in the example.    
One with central role BLE_p2pClient_EAD_Ext, and one with peripheral role BLE_p2pServer_EAD_Ext. 
Once connected and bonded, p2pClient_EAD_Ext can read the Encrypted Data Key Material characteristic of the p2pServer_EAD_Ext and use it to decrypt encrypted advertising data.   

## Setup
These applications are running on two **NUCLEO-WB55RG boards**.    
Open a VT100 terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).
At startup, devices are initialized.
 - The peripheral device starts advertising with Encrypted advertising data. Encrypted Data are received by the central and cannot be decrypted if keys are not stored in RAM nor in Flash.
 On Central:
 - Press SW1: central starts scanning. Scan is stopped if a p2pServer_EAD_Ext device is detected.
 - Press SW3: central initiates a connection. Discovery of service and characteristics. Exchange of max ATT_MTU, pairing and bonding.
 when devices are connected: 
 - Press SW2: initiates a disconnection 
 when devices are not connected:
 - Press SW2: clears security database and erase Flash.
 - Press SW1 to launch a scan. Encrypted Data are received and decrypted if keys are stored in RAM or in Flash.

## Application description
These applications are based on p2pServer_Ext and p2pClient_Ext from STM32CubeWB package v1.20.0.  

On the central terminal, on reception of advertising reports, encryption data are received, if encryption keys are stored in RAM or in Flash, Encrypted advertising data decryption is success. 

On peripheral: 
- At power up of p2pServer_EAD_Ext device, advertising containing encrypted data is started.

On central:
 - At power up of p2pClient_EAD_Ext device, a scan is started. Scan is stopped if a p2pServer_EAD_Ext device is detected.
 - Press SW3: initiates a connection. Discovery of service and characteristics. Exchange of max ATT_MTU, pairing and bonding.
 when devices are connected: 
 - Press SW2: initiates a disconnection.
 when devices are not connected:
 - Press SW2: clears security database and erase Flash.
 - Press SW1: launches a scan. Encrypted Data are received and decrypted if keys are stored in RAM or in Flash.

## Troubleshooting

**Caution** : Issues and the pull-requests are **not supported** to submit problems or suggestions related to the software delivered in this repository. The STM32WB-BLE-EncryptedAdvertising example is being delivered as-is, and not necessarily supported by ST.

**For any other question** related to the product, the hardware performance or characteristics, the tools, the environment, you can submit it to the **ST Community** on the STM32 MCUs related [page](https://community.st.com/s/topic/0TO0X000000BSqSWAW/stm32-mcus).