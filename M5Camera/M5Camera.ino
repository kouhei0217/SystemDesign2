/*
  M5CameraでGoogle DriveにJPEGファイルをアップロードするテスト
*/

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>

#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA_PIN 4
#define SCL_PIN 13
#define I2C_SLAVE_ADDR 0x04

//#define M5CAM_MODEL_A
#define M5CAM_MODEL_B

#if defined(M5CAM_MODEL_A)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21
#elif defined(M5CAM_MODEL_B)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     22
  #define SIOC_GPIO_NUM     23
  
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21
#else
  #error "Not supported"
#endif

WiFiClientSecure client;
#define WIFI_CONN_TIMEOUT 30000 //Wi-Fi接続のタイムアウト
String ssid = "SSIDを入力";
String pass = "パスワードを入力";

String client_id = "クライアントIDを入力";
String client_secret = "クライアントシークレットを入力";
String folder_id="フォルダーIDを入力";
boolean flag=false;
boolean flag_camera = false;
/*  事前にgoogle.comのCAを取得しておく */
const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDgDCCAmigAwIBAgIQMsfIjWyiB+LFGtZyIJVooDANBgkqhkiG9w0BAQsFADBI\n" \
"MRswGQYDVQQDDBJFU0VUIFNTTCBGaWx0ZXIgQ0ExHDAaBgNVBAoME0VTRVQsIHNw\n" \
"b2wuIHMgci4gby4xCzAJBgNVBAYTAlNLMB4XDTIyMDYyODA3MjEzOFoXDTMyMDYy\n" \
"NTA3MjEzOFowSDEbMBkGA1UEAwwSRVNFVCBTU0wgRmlsdGVyIENBMRwwGgYDVQQK\n" \
"DBNFU0VULCBzcG9sLiBzIHIuIG8uMQswCQYDVQQGEwJTSzCCASIwDQYJKoZIhvcN\n" \
"AQEBBQADggEPADCCAQoCggEBAOIRiovBljKL+csTYS+KAuEczlIsDPsf6DjMNMTB\n" \
"3I60TvX4PQQ40Pou6x7XD8jjlB/7ShjxIcOjhtjmG4RJ830wRS1Ygnl6AzCS74be\n" \
"iH00psxWmyUvvn6nYyz+HF18CIdXuL/szi4vBZJ56355Dw5Ckp7IuRWW898UVKju\n" \
"4yLc4LWvCSjaBQrNt2AWbZ37IhNHSfNiTJWgW5qyUXMJHNCm58wMFffTy+Lx4SA2\n" \
"8fBthTwuBQ8VCrDQ4RG0/VDLC/uu+7BBu81YNq1MAYdHE4qu5ioqf/vzXSWB7NK3\n" \
"LvrMsmTp+7jd64TfJx78sB+H8N1cBs0nAwuifUaSLMsdkx8CAwEAAaNmMGQwDgYD\n" \
"VR0PAQH/BAQDAgIEMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFKe7FFTG\n" \
"1m5jqVAKpHmFX6+Xks/lMB8GA1UdIwQYMBaAFKe7FFTG1m5jqVAKpHmFX6+Xks/l\n" \
"MA0GCSqGSIb3DQEBCwUAA4IBAQBK4/zZaARjS09NlInwP2HTcHr7HchC9NGTQxx/\n" \
"HVvl90m1CymW5zlWwZsvS+vLiIN87/lWexT5+drQFSzAAJaSrJrncdXQdJll9zCK\n" \
"iIy7PwQfNCpZ63Wf+lkwAuWS659nXxv+hNhBZjZoAZfDqfeJKeOBhMJEdTttzeaU\n" \
"JrCwWmLgFEQ3KwB1nmHjzHDs49mnbyJqesRWIuWNaVhUUUuKw12+XJCAhsyhxJ46\n" \
"BwPHh2MzqbotdOWWbddGJACc9z463O0ly51k2RxgnsOJtTMZpajHxDgfRVt9zLYk\n" \
"ugxLpOfcL+JnBhF1/c2G3Pp26B4J37HeaeBzYG1fnwrHNKEW\n" \
"-----END CERTIFICATE-----\n";

void receiveEvent(int howMany);

/*  HTTPSでPOSTする  */
String httpsPost(String url, String data, int port){
  Serial.println("now:Start_HTTPPost");
  String response = "";
  char rcvBuf[2048];
  int rcvCount = 0;
  client.setCACert(root_ca);
  client.setInsecure();
  while(!client.connect("oauth2.googleapis.com", 443)){
    delay(100);
    Serial.print(".");
  }
  if (client.connected()){
    client.print(data);
    long timeout = millis();
    while (rcvCount == 0){ 
      while (client.available())
      { 
        rcvBuf[rcvCount]=client.read();
        rcvCount ++;
      }
    }
  }
  response = rcvBuf;
  client.stop();
  Serial.println("now:end_HTTPPost");
  return (response);
}


/*  画像をPOSTする */
String postPic(String url, String token, uint8_t* addr, int picLen, int port){ 
  Serial.println("now:Start_PostPic");
  String host = url.substring(8, url.indexOf("/",8));
  const char* charHost = host.c_str();
  String response = "";
  char rcvBuf[2048];
  int rcvCount = 0;
  Serial.print("Connecting ");
  Serial.println(charHost);
  Serial.println("port" + port);
  Serial.println("under_port");
  
  //時計からファイル名生成
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  char filename[128];
  sprintf(filename,"M5Camera-%02d%02d%02d %02d%02d%02d.jpg", timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  
  Serial.print("filename:");
  Serial.print(filename);
  Serial.println();
  Serial.println("under_filename");

  String boundary = "boundary_1234567890\r\n";
  
  String body="\r\n";
  body += "--" + boundary;
  body += "Content-Type: application/json; charset=UTF-8\r\n";
  body += "\r\n";
  //idにてGoogleDriveのフォルダIDを指定することにより、特定のフォルダにアップできます。
  String metadata = "{\r\n";
  metadata += "\"name\": \"" + ((String)filename) + "\",\r\n"; //filenameに/が入るので取り除く
  metadata += "\"mimeType\": \"image/jpeg\",\r\n";
  metadata += "\"parents\": [\"" + folder_id + "\"]\r\n";
  metadata += "}";
  metadata += "\r\n";
  body += metadata;
  body +=  "--" + boundary;
  body += "Content-Type: image/jpeg\r\n";
  body += "\r\n";

  String header =
  "POST /upload/drive/v3/files?uploadType=multipart HTTP/1.1\r\n"
  "HOST: www.googleapis.com\r\n"
  "authorization: Bearer ";
  header += token;
  header += "\r\n";
  header += "content-type: multipart/form-data; boundary=boundary_1234567890\r\n";
  header += "content-length: ";
  int len = body.length();
  len += picLen;
  len += boundary.length();
  len += ((String)"\r\n").length();
  header += (String)len;
  header += "\r\n";
  
  client.setCACert(root_ca);
  client.setInsecure();
  while(!client.connect(charHost, 443)){
    delay(100);
    Serial.print(".");
  }
  if (client.connected()){ 
    client.print(header);
    client.print(body);
    int sizeDivT = ((int)(picLen / 1000));
    int remSize = ((int)picLen - (int)(picLen / 1000)*1000);
    /*  1000bytesに区切ってPOST  */
    for(int i = 0; i < sizeDivT ;i++){
        client.write((addr + i * 1000), 1000);
        Serial.println("under_1000byte_post1");
    }
    if(remSize > 0){
        client.write((addr + (sizeDivT * 1000)), remSize);
        Serial.println("under_1000byte_post2");
    }

    client.print("\r\n--boundary_1234567890--\r\n");
    client.print("\r\n");
    Serial.println("POST request End");
    long timeout = millis();
    while (rcvCount == 0){
      while (client.available()){
        rcvBuf[rcvCount]=client.read();
        Serial.write(rcvBuf[rcvCount]);
        rcvCount ++;
        if(rcvCount>2047)break;
      }
    }
  }
  else Serial.println("Connection failed");
  response = rcvBuf;
  client.stop();
  Serial.println("now:End_PostPic");
  return (response);
}


/*  コードからリフレッシュトークンを取得  */
String getRefreshToken(String code, String client_id, String client_secret){ 
  Serial.println("now:Start_getRefreshToken");

  String body = "client_id=" + client_id + "&";
  body += "client_secret=" + client_secret + "&";
  body += "code=" + code + "&";
  body += "redirect_uri=http://localhost/&";
  body += "grant_type=authorization_code";

  String header=
  "POST /token HTTP/1.1\r\n"
  "Host: oauth2.googleapis.com\r\n"
  "content-type: application/x-www-form-urlencoded\r\n"
  "content-length: " + (String)body.length() + "\r\n"
  "\r\n";

  Serial.println(header);
  Serial.println(body);
  String ret = httpsPost("https://accounts.google.com/o/oauth2/token", header + body, 443);
  
  Serial.println("RAW response:");
  Serial.println(ret);
  Serial.println();

  if(ret.indexOf("200 OK") > -1) Serial.println("Got the refresh token!");
  else Serial.println("NG");
  
  int refresh_tokenStartPos = ret.indexOf("refresh_token");
  refresh_tokenStartPos = ret.indexOf("\"",refresh_tokenStartPos) + 1;
  refresh_tokenStartPos = ret.indexOf("\"",refresh_tokenStartPos) + 1;
  int refresh_tokenEndPos   = ret.indexOf("\"",refresh_tokenStartPos);
  String refresh_token = ret.substring(refresh_tokenStartPos,refresh_tokenEndPos); 
  Serial.println("refresh_token:"+refresh_token);
  Serial.println("now:End_getRefreshToken");
  return refresh_token;
}


/*  リフレッシュトークンからアクセストークンを取得 */
String getAccessToken(String refresh_token, String client_id, String client_secret){
  Serial.println("now:Start_getAccessToken");
  String body =
  "------WebKitFormBoundarytjGcuAGS9sMRWKcr\r\n"
  "Content-Disposition: form-data; name=\"refresh_token\"\r\n"
  "\r\n"
  + refresh_token +
  "\r\n"
  "------WebKitFormBoundarytjGcuAGS9sMRWKcr\r\n"
  "Content-Disposition: form-data; name=\"client_id\"\r\n"
  "\r\n"
  + client_id +
  "\r\n"
  "------WebKitFormBoundarytjGcuAGS9sMRWKcr\r\n"
  "Content-Disposition: form-data; name=\"client_secret\"\r\n"
  "\r\n"
  + client_secret +
  "\r\n"
  "------WebKitFormBoundarytjGcuAGS9sMRWKcr\r\n"
  "Content-Disposition: form-data; name=\"grant_type\"\r\n"
  "\r\n"
  "refresh_token\r\n"
  "------WebKitFormBoundarytjGcuAGS9sMRWKcr--\r\n";
  
  String header=
  "POST /o/oauth2/token HTTP/1.1\r\n"
  "HOST: accounts.google.com\r\n"
  "content-type: multipart/form-data; boundary=----WebKitFormBoundarytjGcuAGS9sMRWKcr\r\n"
  "content-length: "
  + (String)body.length() +
  "\r\n"
  "\r\n";

  Serial.println("Header: "+header);
  Serial.println("Body: "+body);
  String ret = httpsPost("https://accounts.google.com/o/oauth2/token", header + body, 443); 
  
  Serial.println("RAW response:");
  Serial.println(ret);
  Serial.println();
  
  if(ret.indexOf("200 OK") > -1) Serial.println("Got the token!");
  else{
    Serial.println("NG");
    do_setup();
    return getAccessToken(refresh_token, client_id, client_secret);
  }

  int access_tokenStartPos = ret.indexOf("access_token");
  access_tokenStartPos = ret.indexOf("\"",access_tokenStartPos) + 1;
  access_tokenStartPos = ret.indexOf("\"",access_tokenStartPos) + 1;
  int access_tokenEndPos   = ret.indexOf("\"",access_tokenStartPos);
  String access_token = ret.substring(access_tokenStartPos,access_tokenEndPos); 
  Serial.println("Access_token:"+access_token);
  Serial.println("now:End_getAccessToken");
  return access_token;
}


/*  画像取得  */
camera_fb_t * getJPEG(){
  Serial.println("now:Start_getJPEG");
  camera_fb_t * fb = NULL;
  /*  数回撮影してAE(自動露出)をあわせる */
  esp_camera_fb_get();
  esp_camera_fb_get();
  esp_camera_fb_get();
  esp_camera_fb_get();
  fb = esp_camera_fb_get();  //JPEG取得
  /*
  fb->buf JPEGバッファ
  fb->len JPEGサイズ
  */
  
  if (!fb){
    Serial.printf("Camera capture failed");
  }
  esp_camera_fb_return(fb); //後始末
  Serial.printf("JPG: %uB ", (uint32_t)(fb->len));
  Serial.println("now:End_getJPEG");
  return fb;  
}


/*  APに接続 */
bool initSTA(){
  Serial.println("now:Start_initSTA");
  WiFi.begin(ssid.c_str(), pass.c_str());
  int n;
  long timeout = millis();
  while ((n = WiFi.status()) != WL_CONNECTED && (millis()-timeout) < WIFI_CONN_TIMEOUT){
    Serial.print(".");
    delay(500);
    if (n == WL_NO_SSID_AVAIL || n == WL_CONNECT_FAILED)
    {
      delay(1000);
      WiFi.reconnect();
    }
  }
  Serial.println("now:End_initSTA");
  if (n == WL_CONNECTED){ 
    Serial.println("connect!");
    return true;
  }
  else{ 
    Serial.println("feild!");
    return false;
  }
}

void do_setup(){
  String code = "";
  
  client_id.trim();
  File fd = SPIFFS.open("/access_token.txt", "w");
  if (!fd){
    Serial.println("Config open error");
  }
  fd.println(client_id);

  /*  コードを取得してくれと表示 */
  Serial.println("Please access to");
  Serial.println("https://accounts.google.com/o/oauth2/auth?client_id=" + client_id +"&response_type=code&redirect_uri=http://localhost/&scope=https://www.googleapis.com/auth/drive");
    
  /*  コードを入力  */
  Serial.println("Code?");
  while(!Serial.available()){}
  code = Serial.readStringUntil('\n');
  code.trim();

  client_secret.trim();
  fd.println(client_secret);
  Serial.println("OK");
  delay(1000);
  Serial.println("Refresh token:");
  fd.println(getRefreshToken(code, client_id, client_secret));
  fd.close();
}

void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  initSTA();  //APに接続
  String line = "";
  /*
    SPIフラッシュがフォーマットされてなかったらフォーマット
    設定データがなければリフレッシュトークンを取得して保存する。
    設定データは行ごとに分けて
    ・client_id
    ・client_secret
    ・refresh_token
    と保存
  */
  if(!SPIFFS.begin()){
    Serial.println("SPIFFS formatting");
    SPIFFS.format();
  }
  
//  SPIFFS.remove("/access_token.txt");

  File fd = SPIFFS.open("/access_token.txt", "r");
  fd.readStringUntil('\n');
  fd.readStringUntil('\n');
  line = fd.readStringUntil('\n');
  fd.close();
  
  if(line == ""){ 
    do_setup();
  }

  bool success = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
  if (!success) {
      Serial.println("I2C slave init failed");
      while(1) delay(100);
  }else{
    Serial.println("I2C slave init success");
  }

  WireSlave.onReceive(receiveEvent);

  /*  カメラ初期化設定*/
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  //カメラ初期化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
  }
  else{
    flag=true;
  }
  /*
  あとでカメラ設定変更するときは
  sensor_t * s = esp_camera_sensor_get(); //カメラプロパティのポインタ取得
  s->set_framesize(s, FRAMESIZE_UXGA);  //解像度設定
  とかやればおｋ
  */
  //現在時刻をNTPサーバから取得
  configTime(9*3600L,0,"ntp.nict.jp","time.google.com","ntp.jst.mfeed.ad.jp");
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  Serial.printf("%02d-%02d-%02dT%02d:%02d:%02d+09:00\n", timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday,timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

}

/*  15秒に一回撮影 */
void loop(){
  WireSlave.update();
  if(flag && flag_camera){
    /*  設定読み込み  */
  String code = "";
  String client_id = "";
  String client_secret = "";
  File fd = SPIFFS.open("/access_token.txt", "r");
  client_id = fd.readStringUntil('\r');
  Serial.print("Client ID");
  client_id.trim();
  Serial.print(client_id);
  Serial.println();
  client_secret = fd.readStringUntil('\r');
  client_secret.trim();
  Serial.print("Client Secret:");
  Serial.print(client_secret);
  Serial.println();
  String refresh_token = fd.readStringUntil('\r');
  refresh_token.trim();  
  Serial.print("Refresh token:");
  Serial.print(refresh_token);
  Serial.println();
  fd.close();
  
  Serial.println("Upload Start");
  client.setCACert(root_ca);  
  String access_token = getAccessToken(refresh_token, client_id, client_secret);
  Serial.println("Access token: "+access_token);
  camera_fb_t * fb = getJPEG();
  String ret = postPic("https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart", access_token, fb->buf, fb->len, 443); 
  Serial.println("Return: "+ret);
  }else{
  Serial.println("Failure");
  }
  delay(10000);
}

void receiveEvent(int howMany)
{   
    Serial.println("aa");
    while (1 < WireSlave.available()) // loop through all but the last byte
    {
        int c = WireSlave.read();  // receive byte as a character
        Serial.print(c);            // print the character
//        if(c == 1) flag_camera = true;
//        else if(c == 0) flag_camera = false;
    }

    int x = WireSlave.read();   // receive byte as an integer
    if(x == 1) flag_camera = true;
    else if(x == 0) flag_camera = false;
    Serial.println(x);          // print the integer
}
