#include <queue>
#include <my_lorawan.h>
#include <my_gps.h>
#include <my_wifi_location.h>
#include <my_battery.h>
#include <my_light_control.h>
//// use this devEUI: 0004A30B010651F7

enum SendDataType {
  BATTERY,
  GPS,
  WIFI
};

bool isLoraInit = false;

RTC_DATA_ATTR std::deque<SendDataType> sendDeque;

RTC_DATA_ATTR My_GPS mygps;
RTC_DATA_ATTR My_Wifi_Location mywifiloc;
RTC_DATA_ATTR My_Battery mybattery;
RTC_DATA_ATTR My_LightControl mylight;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("hello?");
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

  delay(3000);

  mywifiloc.init();
  mybattery.init();
  mylight.init();

}

void handleLoraWanStateMachine() {
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      Serial.println("DEVICE_STATE_INIT");
      LoRaWAN.init(loraWanClass,loraWanRegion);
      //both set join DR and DR when ADR off 
      Serial.println("just before setDefaultDR(3)");
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
      isLoraInit = true;
      Serial.println("DEVICE_STATE_SEND");
      if (sendDeque.empty())
      {
        Serial.println("[DEVICE_STATE_SEND]: nothing to send");
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
      SendDataType data = sendDeque.front();
      sendDeque.pop_front();
      switch ( data ) {
        case BATTERY:
          mybattery.prepare_battery_msg(appData, appDataSize);
          Serial.println(appData[0], HEX);
          Serial.println("[DEVICE_STATE_SEND]: battery");
          break;
        case GPS:
          mygps.prepare_coords_msg(appData, appDataSize);
          Serial.println("[DEVICE_STATE_SEND]: gps");
          break;
        case WIFI:
          mywifiloc.prepare_wifi_msg(appData, appDataSize);
          Serial.println("[DEVICE_STATE_SEND]: wifi");
          break;
      }

      LoRaWAN.send();
      Serial.println("sent");
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


void update_deque() {
  if (mybattery.shouldUpdate()) // BATTERY_SEND_INTERVAL (60 sec)
  {
    sendDeque.push_back(SendDataType::BATTERY);
    mybattery.updateLastSendTime();
    Serial.println("[update_deque]: push_back(BATTERY)");
  }

  if (mywifiloc.shouldUpdate(mylight.is_active()))
  {
    sendDeque.push_back(SendDataType::WIFI);
    mywifiloc.updateLastSendTime();
    Serial.println("[update_deque]: push_back(WIFI)");
  }

  if (mygps.shouldUpdate(mylight.is_active()))
  {
    sendDeque.push_back(SendDataType::GPS);
    Serial.println("[update_deque]: push_back(GPS)");
  }
}

void handle_downlink() {
  if (lastDownlinkMessage[0] == 0)
  {
    return; // no new message
  }

  if (lastDownlinkMessage[0] == 0x01)
  {
    lastDownlinkMessage[0] = 0;
    mybattery.request_buzzer();
    Serial.println("[handle_downlink]: 0x01: request buzzer");
    return;
  }

  if (lastDownlinkMessage[0] == 0x02)
  {
    sendDeque.push_front(SendDataType::GPS);
    lastDownlinkMessage[0] = 0;
    Serial.println("[handle_downlink]: 0x02: request GPS");
    return;
  }

  Serial.print("Received unexpected downlink message: ");
  Serial.print(lastDownlinkMessage[0], HEX);
  Serial.println();
  lastDownlinkMessage[0] = 0;
}

void loop()
{
  mybattery.handle_battery(); // does stuff every 60 seconds (BATTERY_UPDATE_INTERVAL)
  mylight.light_ctrl_active();

  if (!mygps.got_valid_coords())
    mygps.get_coords_init();

  handleLoraWanStateMachine();
  if (!isLoraInit) return;

  mybattery.handle_buzzer();
  handle_downlink();
  update_deque();
  
}