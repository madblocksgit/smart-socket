#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
String incomingSta;
// Insert your SSID
constexpr char WIFI_SSID[] = "Madhu P";
typedef struct struct_message
{
    String sta;
} struct_message;
struct_message incomingReadings;
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  incomingSta = incomingReadings.sta;
  if(incomingSta=="lighton")
  {
    Serial.println("Light ON");
    digitalWrite(23,1);
  }
  else if(incomingSta=="lightoff")
  {
    Serial.println("Light OFF");
    digitalWrite(23,0);
  }
  else if(incomingSta=="fanon")
  {
    Serial.println("Fan On");
    digitalWrite(22,1);
  }
  else if(incomingSta=="fanoff")
  {
    Serial.println("Fan Off");
    digitalWrite(22,0);
  }
  else if(incomingSta=="bulbon")
  {
    Serial.println("Bulb On");
    digitalWrite(21,1);
  }
  else if(incomingSta=="bulboff")
  {
    Serial.println("Bulb Off");
    digitalWrite(21,0);
  }
}
int32_t getWiFiChannel(const char *ssid)
{
  if (int32_t n = WiFi.scanNetworks())
  {
      for (uint8_t i=0; i<n; i++)
      {  
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) 
          {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}
 
void setup()
{
  pinMode(23,OUTPUT);//light
  digitalWrite(23,HIGH);
  pinMode(22,OUTPUT);//fan
  digitalWrite(23,HIGH);
  pinMode(21,OUTPUT);//bulb
  digitalWrite(21,HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  int32_t channel = getWiFiChannel(WIFI_SSID);
  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}
void loop() 
{
  
}
