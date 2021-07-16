#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <HTTPClient.h>

//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x38, 0x23, 0x70};

//Must match the receiver structure
typedef struct struct_message
{
    String sta;
} 
struct_message;

//Create a struct_message called myData
struct_message myData;

String serverName = "https://madblocks.tech/projects/smart-socket/device_pull.php";
unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval =500;        // Interval at which to publish sensor readings
unsigned int readingId = 0;

// Insert your SSID
constexpr char WIFI_SSID[] = "Madhu P";

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

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() 
{
  //Init Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin("Madhu P","madhu2022");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Connected to Wifi Network");
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
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;
  //Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() 
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    if(WiFi.status()== WL_CONNECTED)
    {
      HTTPClient http;
      String serverPath = serverName;  
      http.begin(serverPath);
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) 
      {
        String payload = http.getString();
        Serial.print("Data From APP:");
        Serial.println(payload); 
        myData.sta = payload;
        //Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) 
        {
          Serial.println("Sent with success");
        }
        else
        {
          Serial.println("Error sending the data");
        }
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        ESP.restart();
      }
      // Free resources
      if (http.connected())
         http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    previousMillis = currentMillis;
  }
}
