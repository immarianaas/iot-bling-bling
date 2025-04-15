/* Heltec Automation LoRaWAN communication example
 *
 * Function:
 * 1. Upload node data to the server using the standard LoRaWAN protocol.
 *  
 * Description:
 * 1. Communicate using LoRaWAN protocol.
 * 
 * HelTec AutoMation, Chengdu, China
 * 成都惠利特自动化科技有限公司
 * www.heltec.org
 *
 * */

// #include "LoRaWan_APP.h"
#include <my_lorawan.h>

//// use this devEUI: 0004A30B010651F7

/* OTAA para*/
uint8_t devEui[] = { 0x00, 0x04, 0xA3, 0x0B, 0x01, 0x06, 0x51, 0xF7 };
uint8_t appEui[] = { 0xBE, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x16, 0x4F };
uint8_t appKey[] = { 0x97, 0x28, 0x3F, 0x8F, 0x8A, 0x54, 0x76, 0x5C, 0x8A, 0xE7, 0x59, 0xF1, 0x52, 0x61, 0x3D, 0x7B };


uint8_t nwkSKey[] = {0x0E, 0xA8, 0xF9, 0x9E, 0xD6, 0x73, 0xE6, 0x68, 0xE1, 0xFE, 0xA1, 0x8A, 0x24, 0x3C, 0xDC, 0xE0 };
uint8_t appSKey[] = { 0x87, 0xE1, 0x51, 0x13, 0x20, 0x73, 0x0A, 0xE9, 0xC1, 0x1B, 0x9F, 0x04, 0x6E, 0x61, 0x71, 0x76 };
uint32_t devAddr =  ( uint32_t )0x01310715;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = false;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;
/*!
* Number of trials to transmit the frame, if the LoRaMAC layer did not
* receive an acknowledgment. The MAC performs a datarate adaptation,
* according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
* to the following table:
*
* Transmission nb | Data Rate
* ----------------|-----------
* 1 (first)       | DR
* 2               | DR
* 3               | max(DR-1,0)
* 4               | max(DR-1,0)
* 5               | max(DR-2,0)
* 6               | max(DR-2,0)
* 7               | max(DR-3,0)
* 8               | max(DR-3,0)
*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
  *appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
  *if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
  *if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
  *for example, if use REGION_CN470, 
  *the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */
    appDataSize = 4;
    appData[0] = 0x00;
    appData[1] = 0x01;
    appData[2] = 0x02;
    appData[3] = 0x03;
}

//if true, next uplink will add MOTE_MAC_DEVICE_TIME_REQ 


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("hello?");
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
}

void handleLoraWanStateMachine() {
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      Serial.println("DEVICE_STATE_INIT");
      LoRaWAN.init(loraWanClass,loraWanRegion);
      //both set join DR and DR when ADR off 
      LoRaWAN.setDefaultDR(3);
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      Serial.println("DEVICE_STATE_JOIN");
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      Serial.println("DEVICE_STATE_SEND");
      prepareTxFrame( appPort );
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;
      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      Serial.println("DEVICE_STATE_CYCLE");
      // Schedule next packet transmission
      txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      // Serial.println("DEVICE_STATE_SLEEP");
      LoRaWAN.sleep(loraWanClass);
      // deviceState = DEVICE_STATE_SEND;
      break;
    }
    default:
    {
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}

void loop()
{
  handleLoraWanStateMachine();

  Serial.println(lastDownlinkMessage);

}