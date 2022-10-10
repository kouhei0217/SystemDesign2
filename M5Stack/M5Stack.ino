//#include &lt;Arduino.h&gt;
#include "WiFi.h";
#include "WiFiMulti.h";
#include "HTTPClient.h";
#include "M5Stack.h";

#include <Arduino.h>
#include <Wire.h>
#include <WirePacker.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04
 
WiFiMulti wifi;
HTTPClient http;
 
#define WIFI_SSID "SSIDを入力"
#define WIFI_PASS "パスワードを入力"
const char *server = "hooks.slack.com";
const char *json = "{\"text\":\"【注意】問題が発生しました、直ちに部活動の活動場所に向かってください。\",\"icon_emoji\":\":ghost:\",\"username\":\"m5stackpost\"}";
const char *json2 = "{\"text\":\"\",\"icon_emoji\":\":ghost:\",\"username\":\"m5stackpost\"}";
const char *json3 = "{\"text\":\"部活動開始\",\"icon_emoji\":\":ghost:\",\"username\":\"m5stackpost\"}";
const char *json4 = "{\"text\":\"部活動終了\",\"icon_emoji\":\":ghost:\",\"username\":\"m5stackpost\"}";
bool flag_send = 0;
bool do_club = 0;

 
// ルート証明書
const char* slack_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----\n" ;
   
// 中間証明書
const char * slack_sub_ca = 
"-----BEGIN CERTIFICATE-----\n"
"MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS\n"
"U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n"
"AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a\n"
"qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn\n"
"g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW\n"
"raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB\n"
"Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r\n"
"eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB\n"
"/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU\n"
"A95QNVbRTLtm8KPiGxvDl7I90VUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG\n"
"CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG\n"
"GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh\n"
"Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNydDBCBgNV\n"
"HR8EOzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH\n"
"bG9iYWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwCwYJYIZIAYb9bAIBMAcGBWeBDAEB\n"
"MAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEBCwUAA4IB\n"
"AQCAMs5eC91uWg0Kr+HWhMvAjvqFcO3aXbMM9yt1QP6FCvrzMXi3cEsaiVi6gL3z\n"
"ax3pfs8LulicWdSQ0/1s/dCYbbdxglvPbQtaCdB73sRD2Cqk3p5BJl+7j5nL3a7h\n"
"qG+fh/50tx8bIKuxT8b1Z11dmzzp/2n3YWzW2fP9NsarA4h20ksudYbj/NhVfSbC\n"
"EXffPgK2fPOre3qGNm+499iTcc+G33Mw+nur7SpZyEKEOxEXGlLzyQ4UfaJbcme6\n"
"ce1XR2bFuAJKZTRei9AqPCCcUZlM51Ke92sRKw2Sfh3oius2FkOH6ipjv3U/697E\n"
"A7sKPPcw7+uvTPyLNhBzPvOk\n"
"-----END CERTIFICATE-----\n" ;

 
void init() {
  M5.begin(true, false, true);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(65, 10);
  M5.Lcd.println("Slack example");
  M5.Lcd.setCursor(3, 35);
  M5.Lcd.println("Press A button");
}
 
void setup() {
  init();
  Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
}
 
void sendYo() 
{
  for(int i = 0; i < 2; i++){
  // put your setup code here, to run once:
    wifi.addAP(WIFI_SSID, WIFI_PASS);
   
    while (wifi.run() != WL_CONNECTED) {
      delay(10);
      M5.Lcd.printf(".");
    }
    delay(10);
    
    // post slack
    // https://hooks.slack.com/services/XXXXX/YYYYY
    http.begin( server, 443, "/services/XXXXX/YYYYY", slack_sub_ca );
    http.addHeader("Content-Type", "application/json" );
    delay(5);
    if(i == 1){
      M5.Lcd.println("wifi connect ok");
      M5.Lcd.println("connect slack.com");
      http.POST((uint8_t*)json, strlen(json));
      M5.Lcd.println("post hooks.slack.com");
    }else{
      http.POST((uint8_t*)json2, strlen(json));
    }
    delay(5);
    http.end();
  }
  flag_send = 0;
}

void send_club(){
  for(int i = 0; i < 2; i++){
  // put your setup code here, to run once:
    wifi.addAP(WIFI_SSID, WIFI_PASS);
   
    while (wifi.run() != WL_CONNECTED) {
      delay(10);
      M5.Lcd.printf(".");
    }
    delay(10);
    
    // post slack
    // https://hooks.slack.com/services/XXXXX/YYYYY
    http.begin( server, 443, "/services/XXXXX/YYYYY", slack_sub_ca );
    http.addHeader("Content-Type", "application/json" );
    delay(5);
    if(i == 1){
      M5.Lcd.println("wifi connect ok");
      M5.Lcd.println("connect slack.com");
      if(do_club) http.POST((uint8_t*)json3, strlen(json3));
      else http.POST((uint8_t*)json4, strlen(json4));
      M5.Lcd.println("post hooks.slack.com");
    }else{
      http.POST((uint8_t*)json2, strlen(json));      
    }
    delay(5);
    http.end();
  }
}

void loop() {
  M5.update();
  
  if (!flag_send && M5.BtnA.wasPressed()) {
    if(do_club){
      flag_send = 1;
      
      WirePacker packer;
      packer.write(0);
      packer.end();

      Wire.beginTransmission(I2C_SLAVE_ADDR);
        while (packer.available()) {    // write every packet byte
            Wire.write(packer.read());
        }
        Wire.endTransmission();

      Serial.println("send");
    }
  }else if(M5.BtnB.wasPressed()){
    do_club = !do_club;
    send_club();

    WirePacker packer;
      packer.write(do_club);
      packer.end();

      Wire.beginTransmission(I2C_SLAVE_ADDR);
        while (packer.available()) {    // write every packet byte
            Wire.write(packer.read());
        }
        Wire.endTransmission();

      Serial.println("send");
  }else if (M5.BtnC.wasPressed()) {
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(65, 10);
    M5.Lcd.println("Slack example");
    M5.Lcd.setCursor(3, 35);
    M5.Lcd.println("Press A button");
    flag_send = 0;
  }else flag_send = 0;
  delay(5);
  
  if(flag_send){
    M5.Lcd.println("send slack");
    sendYo();
  }
  delay(100);
}
