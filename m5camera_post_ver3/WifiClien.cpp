#include "WiFi.h"
#include "HTTPClient.h"
#include "WifiClient.h"
#include "ArduinoJson.h"
char image[15000];
void
WifiClient::begin(const char *ssid, const char *password)
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {//接続確認
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address is ");
  Serial.println(WiFi.localIP());
  delay(1000);//電圧降下対策→いらないかもしれないが一応記述
}
void
WifiClient::post(const char *host, const char *buffer, const size_t length)
{
  Serial.println("post start");
  HTTPClient http;
  http.begin(host);
  Serial.print(http.begin(host));
  Serial.println("");
  http.addHeader("Content-Type", "application/json");

  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> json_request;
  json_request["image"] = buffer;
  char buffer_json[255];
  serializeJson(json_request, buffer_json, sizeof(buffer_json));

  const int status_code = http.POST((uint8_t*)buffer, length);
  Serial.printf("Wi-Fi POST -> %s\nSize=%zu, Status=%d　\n", host, length, status_code);
  Serial.println("end");
  http.end();
}

//void
//WifiClient::post2(const char *host, const char *buffer, const size_t length) {
//  Serial.println("post start");
//  
// 
//  Serial.println("post end");
//  http.end();
//}
