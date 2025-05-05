
#include <my_lorawan.h>
#include <my_gps.h>
#include <my_wifi_location.h>
//// use this devEUI: 0004A30B010651F7

My_GPS mygps;
My_Wifi_Location mywifiloc;

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
    Serial.print("[prepareTxFrame] ");
    Serial.println(lastDownlinkMessage);
    appDataSize = 4;
    appData[0] = 0x00;
    appData[1] = 0x00;
    appData[2] = 0x00;
    appData[3] = (int) lastDownlinkMessage[0];
}

//if true, next uplink will add MOTE_MAC_DEVICE_TIME_REQ 


void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("hello?");
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  mywifiloc.init();

}

void handleLoraWanStateMachine() {
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      Serial.println("DEVICE_STATE_INIT");
      //Serial.println(loraWanClass);
      //Serial.println(loraWanRegion);
      LoRaWAN.init(loraWanClass,loraWanRegion);
      //both set join DR and DR when ADR off 
      LoRaWAN.setDefaultDR(3);
      Serial.println("HM");

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
      // prepareTxFrame( appPort );
      // mygps.prepare_coords_msg(appData, appDataSize);
      // LoRaWAN.send();
      //Serial.println("sent first");
      // mywifiloc.prepare_wifi_msg(appData, appDataSize);
      // LoRaWAN.send();
      Serial.println("sent second");
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

bool needsToReply = true;

void loop()
{
  handleLoraWanStateMachine();

  if (lastDownlinkMessage[0] != 0)
  {
    Serial.print("|");
    int i = 0;
    while (lastDownlinkMessage[i] != 0)
    {
      Serial.print(lastDownlinkMessage[i++], HEX);
    }
    Serial.println("|");

  }

  if (newDownlinkMessage && needsToReply) 
  {
    needsToReply = false;
    deviceState = DEVICE_STATE_SEND;
  }

}