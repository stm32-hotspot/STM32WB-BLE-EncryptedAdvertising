/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/p2p_client_app.c
  * Description        : P2P Client Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
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

#include "main.h"
#include "app_common.h"

#include "dbg_trace.h"

#include "ble.h"
#include "p2p_client_app.h"

#include "stm32_seq.h"
#include "app_ble.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
  P2P_START_TIMER_EVT,
  P2P_STOP_TIMER_EVT,
  P2P_NOTIFICATION_INFO_RECEIVED_EVT,
} P2P_Client_Opcode_Notification_evt_t;

typedef struct
{
  uint8_t * pPayload;
  uint8_t     Length;
}P2P_Client_Data_t;

typedef struct
{
  P2P_Client_Opcode_Notification_evt_t  P2P_Client_Evt_Opcode;
  P2P_Client_Data_t DataTransfered;
}P2P_Client_App_Notification_evt_t;

typedef struct
{
  /**
   * state of the P2P Client
   * state machine
   */
  APP_BLE_ConnStatus_t state;

  /**
   * connection handle
   */
  uint16_t connHandle;

  /**
   * handle of the P2P service
   */
  uint16_t P2PServiceHandle;

  /**
   * end handle of the P2P service
   */
  uint16_t P2PServiceEndHandle;

  /**
   * handle of the Tx characteristic - Write To Server
   *
   */
  uint16_t P2PWriteToServerCharHdle;

  /**
   * handle of the client configuration
   * descriptor of Tx characteristic
   */
  uint16_t P2PWriteToServerDescHandle;

  /**
   * handle of the Rx characteristic - Notification From Server
   *
   */
  uint16_t P2PNotificationCharHdle;

  /**
   * handle of the client configuration
   * descriptor of Rx characteristic
   */
  uint16_t P2PNotificationDescHandle;

  /**
   * handle of the KeyMaterial characteristic
   */  
  uint16_t EAD_Key_MaterialCharHdl;
  uint16_t ALLServiceStartHdl;
  uint16_t ALLServiceEndHdl;
  uint16_t GAPServiceStartHdl;
  uint16_t GAPServiceEndHdl;
  uint16_t GATTServiceStartHdl;
  uint16_t GATTServiceEndHdl;
  uint16_t ServiceChangedCharStartHdl;
  uint16_t ServiceChangedCharValueHdl;

}P2P_ClientContext_t;

/* USER CODE BEGIN PTD */
typedef struct{
  uint8_t                                     Device_Led_Selection;
  uint8_t                                     Led1;
}P2P_LedCharValue_t;

typedef struct{
  uint8_t                                     Device_Button_Selection;
  uint8_t                                     Button1;
}P2P_ButtonCharValue_t;

typedef struct
{

  uint8_t       Notification_Status; /* used to check if P2P Server is enabled to Notify */

  P2P_LedCharValue_t         LedControl;
  P2P_ButtonCharValue_t      ButtonStatus;

  uint16_t ConnectionHandle; 


} P2P_Client_App_Context_t;

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
#define UNPACK_2_BYTE_PARAMETER(ptr)  \
        (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
        (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static P2P_ClientContext_t aP2PClientContext[BLE_CFG_CLT_MAX_NBR_CB];

/**
 * END of Section BLE_APP_CONTEXT
 */
/* USER CODE BEGIN PV */
static P2P_Client_App_Context_t P2P_Client_App_Context;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void Gatt_Notification(P2P_Client_App_Notification_evt_t *pNotification);
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event);
/* USER CODE BEGIN PFP */
static tBleStatus Write_Char(uint16_t UUID, uint8_t Service_Instance, uint8_t *pPayload);
static void Button_Trigger_Received( void );
static void Update_Service( void );

uint8_t encryption_key[16];
uint8_t initialization_vector[16];
extern uint8_t restored_key[16];
extern uint8_t restored_iv[8];
uint8_t char_encr_data_key_mat[24];
extern uint64_t FlashDataToWriteTab[CFG_NBR_OF_FLASH_SECTOR_TO_PROCESS*512];

extern uint32_t flash_fsm_state;
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void P2PC_APP_Init(void)
{
  uint8_t index =0;
/* USER CODE BEGIN P2PC_APP_Init_1 */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_SEARCH_SERVICE_ID, UTIL_SEQ_RFU, Update_Service );
  UTIL_SEQ_RegTask( 1<< CFG_TASK_SW1_BUTTON_PUSHED_ID, UTIL_SEQ_RFU, Button_Trigger_Received );

  /**
   * Initialize LedButton Service
   */
  P2P_Client_App_Context.Notification_Status=0;
  P2P_Client_App_Context.ConnectionHandle =  0x00;

  P2P_Client_App_Context.LedControl.Device_Led_Selection=0x00;/* device Led */
  P2P_Client_App_Context.LedControl.Led1=0x00; /* led OFF */
  P2P_Client_App_Context.ButtonStatus.Device_Button_Selection=0x01;/* Device1 */
  P2P_Client_App_Context.ButtonStatus.Button1=0x00;
/* USER CODE END P2PC_APP_Init_1 */
  for(index = 0; index < BLE_CFG_CLT_MAX_NBR_CB; index++)
  {
    aP2PClientContext[index].state= APP_BLE_IDLE;
  }

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterCltHandler(Event_Handler);

#if(CFG_DEBUG_APP_TRACE != 0)
  APP_DBG_MSG("-- P2P CLIENT INITIALIZED \n");
#endif

/* USER CODE BEGIN P2PC_APP_Init_2 */

/* USER CODE END P2PC_APP_Init_2 */
  return;
}

void P2PC_APP_Notification(P2PC_APP_ConnHandle_Not_evt_t *pNotification)
{
/* USER CODE BEGIN P2PC_APP_Notification_1 */

/* USER CODE END P2PC_APP_Notification_1 */
  switch(pNotification->P2P_Evt_Opcode)
  {
/* USER CODE BEGIN P2P_Evt_Opcode */

/* USER CODE END P2P_Evt_Opcode */

  case PEER_CONN_HANDLE_EVT :
/* USER CODE BEGIN PEER_CONN_HANDLE_EVT */
    P2P_Client_App_Context.ConnectionHandle = pNotification->ConnectionHandle;
/* USER CODE END PEER_CONN_HANDLE_EVT */
      break;

    case PEER_DISCON_HANDLE_EVT :
/* USER CODE BEGIN PEER_DISCON_HANDLE_EVT */
      {
      uint8_t index = 0;
      P2P_Client_App_Context.ConnectionHandle =  0x00;
      while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
                  (aP2PClientContext[index].state != APP_BLE_IDLE))
      {
        aP2PClientContext[index].state = APP_BLE_IDLE;
      }
      BSP_LED_Off(LED_BLUE);         
      }
/* USER CODE END PEER_DISCON_HANDLE_EVT */
      break;

    default:
/* USER CODE BEGIN P2P_Evt_Opcode_Default */

/* USER CODE END P2P_Evt_Opcode_Default */
      break;
  }
/* USER CODE BEGIN P2PC_APP_Notification_2 */

/* USER CODE END P2PC_APP_Notification_2 */
  return;
}
/* USER CODE BEGIN FD */
void P2PC_APP_SW1_Button_Action(void)
{

  UTIL_SEQ_SetTask(1<<CFG_TASK_SW1_BUTTON_PUSHED_ID, CFG_SCH_PRIO_0);

}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *blecore_evt;

  P2P_Client_App_Notification_evt_t Notification;

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch(blecore_evt->ecode)
      {

        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE:
        {
          aci_att_read_by_group_type_resp_event_rp0 *pr = (void*)blecore_evt->data;
          uint8_t numServ, i, idx;
          uint16_t uuid, handle, ServiceStartHdl, ServiceEndHdl;
          
          uint8_t index, uuid_offset;
          handle = pr->Connection_Handle;
          index = 0;
          while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
                  (aP2PClientContext[index].state != APP_BLE_IDLE))
          {
            APP_BLE_ConnStatus_t status;

            status = APP_BLE_Get_Client_Connection_Status(aP2PClientContext[index].connHandle);

            if((aP2PClientContext[index].state == APP_BLE_CONNECTED_CLIENT)&&
                    (status == APP_BLE_IDLE))
            {
              /* Handle deconnected */

              aP2PClientContext[index].state = APP_BLE_IDLE;
              aP2PClientContext[index].connHandle = 0xFFFF;
              break;
            }
            index++;
          }

          if(index < BLE_CFG_CLT_MAX_NBR_CB)
          {
            aP2PClientContext[index].connHandle= handle;

            numServ = (pr->Data_Length) / pr->Attribute_Data_Length;

            /* the event data will be
             * 2bytes start handle
             * 2bytes end handle
             * 2 or 16 bytes data
             * we are interested only if the UUID is 16 bit.
             * So check if the data length is 6
             */
            if (pr->Attribute_Data_Length == 20)
            {
              idx = 16;
            }

            if (pr->Attribute_Data_Length == 6)
            {
              idx = 4;
            }
              uuid_offset = 4;
              for (i=0; i<numServ; i++)
              {
                /**/
                ServiceStartHdl =  UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[uuid_offset - 4]);
                ServiceEndHdl = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[uuid_offset - 2]);
                APP_DBG_MSG(" ServiceStartHdl 0x%x\n", ServiceStartHdl);
                APP_DBG_MSG(" ServiceEndHdl 0x%x\n", ServiceEndHdl);
                
                uuid = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx]);
                APP_DBG_MSG("  %d/%d short UUID=0x%04X, handle [0x%04X - 0x%04X]",
                   i + 1, numServ, uuid, ServiceStartHdl,ServiceEndHdl);
                
                
                /* complete context fields */
                if ( (aP2PClientContext[index].ALLServiceStartHdl == 0x0000) || (ServiceStartHdl < aP2PClientContext[index].ALLServiceStartHdl) )
                {
                  aP2PClientContext[index].ALLServiceStartHdl = ServiceStartHdl;
                }
                if ( (aP2PClientContext[index].ALLServiceEndHdl == 0x0000) || (ServiceEndHdl > aP2PClientContext[index].ALLServiceEndHdl) )
                {
                  aP2PClientContext[index].ALLServiceEndHdl = ServiceEndHdl;
                }
                /**/

                if (uuid == GAP_SERVICE_UUID)
                {
                  aP2PClientContext[index].GAPServiceStartHdl = ServiceStartHdl;
                  aP2PClientContext[index].GAPServiceEndHdl = ServiceEndHdl;
                  
                  APP_DBG_MSG(", GAP_SERVICE_UUID found\n");
                }
                else if (uuid == GENERIC_ATTRIBUTE_SERVICE_UUID)
                {
                  aP2PClientContext[index].GATTServiceStartHdl = ServiceStartHdl;
                  aP2PClientContext[index].GATTServiceEndHdl = ServiceEndHdl;
                  
                  APP_DBG_MSG(", GENERIC_ATTRIBUTE_SERVICE_UUID found\n");
                }
                else if(uuid == P2P_SERVICE_UUID)
                {
#if(CFG_DEBUG_APP_TRACE != 0)
                  APP_DBG_MSG("-- GATT : P2P_SERVICE_UUID FOUND - connection handle 0x%x \n", aP2PClientContext[index].connHandle);
#endif
#if (UUID_128BIT_FORMAT==1)
                aP2PClientContext[index].P2PServiceHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-16]);
                aP2PClientContext[index].P2PServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-14]);
#else
                aP2PClientContext[index].P2PServiceHandle = UNPACK_2_BYTE_PARAMETER(&pr->Attribute_Data_List[idx-4]);
                aP2PClientContext[index].P2PServiceEndHandle = UNPACK_2_BYTE_PARAMETER (&pr->Attribute_Data_List[idx-2]);
#endif
                  aP2PClientContext[index].state = APP_BLE_DISCOVER_CHARACS ;
                }
                idx += pr->Attribute_Data_Length;
                uuid_offset += pr->Attribute_Data_Length;
              }
            }
        }
        break;

        case ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE:
        {

          aci_att_read_by_type_resp_event_rp0 *pr = (void*)blecore_evt->data;
          uint8_t i, numHdlValuePair;
          uint16_t uuid;
          uint16_t CharStartHdl, CharValueHdl;
          uint8_t CharProperties;

          /* the event data will be
           * 2 bytes start handle
           * 1 byte char properties
           * 2 bytes handle
           * 2 or 16 bytes data
           */

          uint8_t index, uuid_offset, uuid_short_offset;

          index = 0;
          
          for (index = 0 ; index < BLE_CFG_CLT_MAX_NBR_CB ; index++)
          {
            if (aP2PClientContext[index].connHandle == pr->Connection_Handle)
            {
              break;
            }
          }

          if(index < BLE_CFG_CLT_MAX_NBR_CB)
          {
            numHdlValuePair = pr->Data_Length / pr->Handle_Value_Pair_Length;
            uuid_offset = 5;
            if (pr->Handle_Value_Pair_Length == 21)
            {
              uuid_short_offset = 12;
            }
              
            if (pr->Handle_Value_Pair_Length == 7)
            {
              uuid_short_offset = 0;
            }

            pr->Data_Length -= 1;

            for (i = 0; i < numHdlValuePair; i++)
            {
              CharStartHdl = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[uuid_offset - 5]);
              CharProperties = pr->Handle_Value_Pair_Data[uuid_offset - 3];
              CharValueHdl = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[uuid_offset - 2]);
              uuid = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[uuid_offset + uuid_short_offset]);
                /* store the characteristic handle not the attribute handle */
//                handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_Value_Pair_Data[uuid_offset - 5]);

                if ( (uuid != 0x0) && (CharProperties != 0x0) && (CharStartHdl != 0x0) && (CharValueHdl != 0) )
                {
                  if (uuid == DEVICE_NAME_UUID)
                  {
                    APP_DBG_MSG(", GAP DEVICE_NAME charac found\n");
                  }
                  else if (uuid == APPEARANCE_UUID)
                  {
                    APP_DBG_MSG(", GAP APPEARANCE charac found\n");
                  }
                  else if (uuid == SERVICE_CHANGED_CHARACTERISTIC_UUID)
                  {
                    aP2PClientContext[index].ServiceChangedCharStartHdl = CharStartHdl;
                    aP2PClientContext[index].ServiceChangedCharValueHdl = CharValueHdl;
                    APP_DBG_MSG(", GATT SERVICE_CHANGED_CHARACTERISTIC_UUID charac found\n");
                  }
                  else if (uuid == ENCRYPTED_DATA_KEY_MATERIAL_UUID)
                  {
#if(CFG_DEBUG_APP_TRACE != 0)
                    APP_DBG_MSG("-- GATT : ENCRYPTED_DATA_KEY_MATERIAL_UUID FOUND  - connection handle 0x%x\n", aP2PClientContext[index].connHandle);
#endif
                    aP2PClientContext[index].EAD_Key_MaterialCharHdl = CharStartHdl;
                    
                    
                  }
                  else if(uuid == P2P_WRITE_CHAR_UUID)
                  {
#if(CFG_DEBUG_APP_TRACE != 0)
                    APP_DBG_MSG("-- GATT : WRITE_UUID FOUND - connection handle 0x%x\n", aP2PClientContext[index].connHandle);
#endif
                    aP2PClientContext[index].state = APP_BLE_DISCOVER_WRITE_DESC;
                    aP2PClientContext[index].P2PWriteToServerCharHdle = CharStartHdl;
                  }
                  
                  else if(uuid == P2P_NOTIFY_CHAR_UUID)
                  {
#if(CFG_DEBUG_APP_TRACE != 0)
                    APP_DBG_MSG("-- GATT : NOTIFICATION_CHAR_UUID FOUND  - connection handle 0x%x\n", aP2PClientContext[index].connHandle);
#endif
                    aP2PClientContext[index].state = APP_BLE_DISCOVER_NOTIFICATION_CHAR_DESC;
                    aP2PClientContext[index].P2PNotificationCharHdle = CharStartHdl;
                  }                 
                  else
                  {
                    APP_DBG_MSG("\n");
                  }
                  
                }
                uuid_offset += pr->Handle_Value_Pair_Length;
            }
          }
          else
          {
            APP_DBG_MSG("ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE, failed handle not found in connection table !\n");
          }
        }
        break;

        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE:
        {
          aci_att_find_info_resp_event_rp0 *pr = (void*)blecore_evt->data;

          uint8_t numDesc, idx, i;
          uint16_t uuid, handle;

          /*
           * event data will be of the format
           * 2 bytes handle
           * 2 bytes UUID
           */

          uint8_t index;

          index = 0;
          while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
                  (aP2PClientContext[index].connHandle != pr->Connection_Handle))

            index++;

          if(index < BLE_CFG_CLT_MAX_NBR_CB)
          {

            numDesc = (pr->Event_Data_Length) / 4;
            /* we are interested only in 16 bit UUIDs */
            idx = 0;
            if (pr->Format == UUID_TYPE_16)
            {
              for (i=0; i<numDesc; i++)
              {
                handle = UNPACK_2_BYTE_PARAMETER(&pr->Handle_UUID_Pair[idx]);
                uuid = UNPACK_2_BYTE_PARAMETER(&pr->Handle_UUID_Pair[idx+2]);

                if(uuid == CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID)
                {
#if(CFG_DEBUG_APP_TRACE != 0)
                  APP_DBG_MSG("-- GATT : CLIENT_CHAR_CONFIG_DESCRIPTOR_UUID- connection handle 0x%x\n", aP2PClientContext[index].connHandle);
#endif
                  if( aP2PClientContext[index].state == APP_BLE_DISCOVER_NOTIFICATION_CHAR_DESC)
                  {

                    aP2PClientContext[index].P2PNotificationDescHandle = handle;
                    aP2PClientContext[index].state = APP_BLE_ENABLE_NOTIFICATION_DESC;

                  }
                }
                idx += 4;
              }
            }
          }
        }
        break; /*ACI_ATT_FIND_INFO_RESP_VSEVT_CODE*/

        case ACI_GATT_NOTIFICATION_VSEVT_CODE:
        {
          aci_gatt_notification_event_rp0 *pr = (void*)blecore_evt->data;
          uint8_t index;

          index = 0;
          while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
                  (aP2PClientContext[index].connHandle != pr->Connection_Handle))
            index++;

          if(index < BLE_CFG_CLT_MAX_NBR_CB)
          {

            if ( (pr->Attribute_Handle == aP2PClientContext[index].P2PNotificationCharHdle) &&
                    (pr->Attribute_Value_Length == (2)) )
            {

              Notification.P2P_Client_Evt_Opcode = P2P_NOTIFICATION_INFO_RECEIVED_EVT;
              Notification.DataTransfered.Length = pr->Attribute_Value_Length;
              Notification.DataTransfered.pPayload = &pr->Attribute_Value[0];

              Gatt_Notification(&Notification);

              /* INFORM APPLICATION BUTTON IS PUSHED BY END DEVICE */

            }
          }
        }
        break;/* end ACI_GATT_NOTIFICATION_VSEVT_CODE */

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE:
        {
          aci_gatt_proc_complete_event_rp0 *pr = (void*)blecore_evt->data;
#if(CFG_DEBUG_APP_TRACE != 0)
          APP_DBG_MSG("-- GATT : ACI_GATT_PROC_COMPLETE_VSEVT_CODE \n");
          APP_DBG_MSG("\n");
#endif

          uint8_t index;

          index = 0;
          while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
                  (aP2PClientContext[index].connHandle != pr->Connection_Handle))
            index++;

          if(index < BLE_CFG_CLT_MAX_NBR_CB)
          {

            UTIL_SEQ_SetTask( 1<<CFG_TASK_SEARCH_SERVICE_ID, CFG_SCH_PRIO_0);

          }
        }
        break; /*ACI_GATT_PROC_COMPLETE_VSEVT_CODE*/
        case ACI_ATT_READ_RESP_VSEVT_CODE:
        {
          aci_att_read_resp_event_rp0 *read_resp_event;
          read_resp_event = (aci_att_read_resp_event_rp0 *)blecore_evt->data;
          uint8_t i;
          uint64_t *p_encryption_key_flash;
          uint64_t *p_init_vector_flash;
          APP_DBG_MSG("  ACI_ATT_READ_RESP_VSEVT_CODE\n");
          APP_DBG_MSG("  read_resp_event->Event_Data_Length %d\n", read_resp_event->Event_Data_Length);
          UNUSED(read_resp_event);
          /* USER CODE BEGIN ACI_ATT_READ_RESP_VSEVT_CODE */
          
          for (i=0;i<8;i++)
          {
            initialization_vector[i] = (uint8_t)read_resp_event->Attribute_Value[i];
          }
          for (i=0;i<16;i++)
          {
            encryption_key[i] = read_resp_event->Attribute_Value[8+i];
          }
          /* store initialization vector and encryption key in Flash */
          p_encryption_key_flash = (uint64_t*)(encryption_key);
          for (i=0;i<2;i++)
          {
            FlashDataToWriteTab[i] = (uint64_t)(*(p_encryption_key_flash)++);
          }
          p_init_vector_flash = (uint64_t*)(initialization_vector);
          
          FlashDataToWriteTab[2] = (uint64_t)(*(p_init_vector_flash));
          
          flash_fsm_state = APP_BLE_EraseBeforeWrite_Flash;
          UTIL_SEQ_SetTask( 1<<CFG_TASK_FLASH_OPERATION_REQ_ID, CFG_SCH_PRIO_0);

          /* USER CODE END ACI_ATT_READ_RESP_VSEVT_CODE */
          
        }
        break;
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 * exchange_mtu_resp;
          exchange_mtu_resp = (aci_att_exchange_mtu_resp_event_rp0 *)blecore_evt->data;
          APP_DBG_MSG("  MTU exchanged size = %d\n",exchange_mtu_resp->Server_RX_MTU );
          uint8_t result;
          UNUSED(exchange_mtu_resp);
          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          if (aP2PClientContext[0].EAD_Key_MaterialCharHdl != 0x0000)
          {
            result = aci_gatt_read_char_value(aP2PClientContext[0].connHandle,
                                              aP2PClientContext[0].EAD_Key_MaterialCharHdl+1);
            if (result == BLE_STATUS_SUCCESS)
            {
              APP_DBG_MSG("aci_gatt_read_char_value success\n\n");
            }
            else
            {
              APP_DBG_MSG("aci_gatt_read_char_value fail, status =0x%02X\n\n", result);
            }
            /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          }
        }
        break;
        default:
          break;
      }
    }

    break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

    default:
      break;
  }

  return(return_value);
}/* end BLE_CTRL_Event_Acknowledged_Status_t */

void Gatt_Notification(P2P_Client_App_Notification_evt_t *pNotification)
{
/* USER CODE BEGIN Gatt_Notification_1*/

/* USER CODE END Gatt_Notification_1 */
  switch(pNotification->P2P_Client_Evt_Opcode)
  {
/* USER CODE BEGIN P2P_Client_Evt_Opcode */

/* USER CODE END P2P_Client_Evt_Opcode */

    case P2P_NOTIFICATION_INFO_RECEIVED_EVT:
/* USER CODE BEGIN P2P_NOTIFICATION_INFO_RECEIVED_EVT */
    {
      P2P_Client_App_Context.LedControl.Device_Led_Selection=pNotification->DataTransfered.pPayload[0];
      switch(P2P_Client_App_Context.LedControl.Device_Led_Selection) {

        case 0x01 : {

          P2P_Client_App_Context.LedControl.Led1=pNotification->DataTransfered.pPayload[1];

          if(P2P_Client_App_Context.LedControl.Led1==0x00){
            BSP_LED_Off(LED_BLUE);
            APP_DBG_MSG(" -- P2P APPLICATION CLIENT : NOTIFICATION RECEIVED - LED OFF \n\r");
            APP_DBG_MSG(" \n\r");
          } else {
            APP_DBG_MSG(" -- P2P APPLICATION CLIENT : NOTIFICATION RECEIVED - LED ON\n\r");
            APP_DBG_MSG(" \n\r");
            BSP_LED_On(LED_BLUE);
          }

          break;
        }
        default : break;
      }

    }
/* USER CODE END P2P_NOTIFICATION_INFO_RECEIVED_EVT */
      break;

    default:
/* USER CODE BEGIN P2P_Client_Evt_Opcode_Default */

/* USER CODE END P2P_Client_Evt_Opcode_Default */
      break;
  }
/* USER CODE BEGIN Gatt_Notification_2*/

/* USER CODE END Gatt_Notification_2 */
  return;
}

uint8_t P2P_Client_APP_Get_State( void ) {
  return aP2PClientContext[0].state;
}
/* USER CODE BEGIN LF */
/**
 * @brief  Feature Characteristic update
 * @param  pFeatureValue: The address of the new value to be written
 * @retval None
 */
tBleStatus Write_Char(uint16_t UUID, uint8_t Service_Instance, uint8_t *pPayload)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t index;

  index = 0;
  while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
          (aP2PClientContext[index].state != APP_BLE_IDLE))
  {
    switch(UUID)
    {
      case P2P_WRITE_CHAR_UUID: /* SERVER RX -- so CLIENT TX */
        ret = aci_gatt_write_without_resp(aP2PClientContext[index].connHandle,
                                         aP2PClientContext[index].P2PWriteToServerCharHdle,
                                         2, /* charValueLen */
                                         (uint8_t *)  pPayload);
        break;
      default:
        break;
    }
    index++;
  }

  return ret;
}/* end Write_Char() */

void Button_Trigger_Received(void)
{
  APP_DBG_MSG("-- P2P APPLICATION CLIENT  : BUTTON PUSHED - WRITE TO SERVER \n ");
  APP_DBG_MSG(" \n\r");
  if(P2P_Client_App_Context.ButtonStatus.Button1 == 0x00)
  {
    P2P_Client_App_Context.ButtonStatus.Button1 = 0x01;
  }else {
    P2P_Client_App_Context.ButtonStatus.Button1 = 0x00;
  }

  Write_Char( P2P_WRITE_CHAR_UUID, 0, (uint8_t *)&P2P_Client_App_Context.ButtonStatus);

  return;
}

void Update_Service()
{
  uint16_t enable = 0x0001;
  uint16_t disable = 0x0000;
  uint8_t index;

  index = 0;
  while((index < BLE_CFG_CLT_MAX_NBR_CB) &&
          (aP2PClientContext[index].state != APP_BLE_IDLE))
  {
    switch(aP2PClientContext[index].state)
    {
      
      case APP_BLE_DISCOVER_SERVICES:
        APP_DBG_MSG("P2P_DISCOVER_SERVICES\n");
        break;
      case APP_BLE_DISCOVER_CHARACS:
        APP_DBG_MSG("* GATT : Discover P2P Characteristics\n");
        aci_gatt_disc_all_char_of_service(
                           aP2PClientContext[index].connHandle,
                           aP2PClientContext[index].ALLServiceStartHdl,
                           aP2PClientContext[index].ALLServiceEndHdl);
        
        break;
      case APP_BLE_DISCOVER_WRITE_DESC: /* Not Used - No descriptor */
        APP_DBG_MSG("* GATT : Discover Descriptor of TX - Write  Characteristic\n");
        aci_gatt_disc_all_char_desc(aP2PClientContext[index].connHandle,
                                    aP2PClientContext[index].P2PWriteToServerCharHdle,
                                    aP2PClientContext[index].P2PWriteToServerCharHdle+2);
        break;
      case APP_BLE_DISCOVER_NOTIFICATION_CHAR_DESC:
        APP_DBG_MSG("* GATT : Discover Descriptor of Rx - Notification  Characteristic\n");
        aci_gatt_disc_all_char_desc(aP2PClientContext[index].connHandle,
                                    aP2PClientContext[index].P2PNotificationCharHdle,
                                    aP2PClientContext[index].P2PNotificationCharHdle+2);
        break;
      case APP_BLE_ENABLE_NOTIFICATION_DESC:
        APP_DBG_MSG("* GATT : Enable Server Notification\n");
        aci_gatt_write_char_desc(aP2PClientContext[index].connHandle,
                                 aP2PClientContext[index].P2PNotificationDescHandle,
                                 2,
                                 (uint8_t *)&enable);
        aP2PClientContext[index].state = APP_BLE_CONNECTED_CLIENT;
        BSP_LED_Off(LED_RED);
        UTIL_SEQ_SetTask(1 << CFG_TASK_SECURITY_REQ_ID, CFG_SCH_PRIO_0);
        break;
      case APP_BLE_DISABLE_NOTIFICATION_DESC :
        APP_DBG_MSG("* GATT : Disable Server Notification\n");
        aci_gatt_write_char_desc(aP2PClientContext[index].connHandle,
                                 aP2PClientContext[index].P2PNotificationDescHandle,
                                 2,
                                 (uint8_t *)&disable);
        aP2PClientContext[index].state = APP_BLE_CONNECTED_CLIENT;
        break;
      default:
        break;
    }
    index++;
  }
  return;
}
/* USER CODE END LF */
