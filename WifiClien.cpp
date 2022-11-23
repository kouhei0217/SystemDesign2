#include <WiFi.h>
#include <HTTPClient.h>
#include "WifiClient.h"

void 
WifiClient::begin(const char *ssid, const char *password)
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address is ");
  Serial.println(WiFi.localIP());
  //画像確認用
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/capture 画像確認");//追加
  delay(100);//電圧降下対策→いらないかも
}

void 
WifiClient::post(const char *host, const char *buffer, const size_t length)
{
  HTTPClient http;
  http.begin(host);
  Serial.print(http.begin(host));
  Serial.println("");
  http.addHeader("Content-Type", "application/json");
  const int status_code = http.POST((uint8_t*)buffer, length);

  Serial.printf("Wi-Fi POST -> %s\nSize=%zu, Status=%d", host, length, status_code);
  Serial.println("");
  Serial.println("end");
  http.end();
}
