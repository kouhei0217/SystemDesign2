#include "Camera.h"
#include "WifiClient.h"
#include "camera_pins.h"

// DeepSleep
#define SLEEP_INTERVAL_SEC 60 //写真を撮る間隔を調整する。

// 環境に合わせて適宜変更します
//const char* ssid = "aterm-b27d1e";
//const char* password = "236fd38a4e01c";
//自宅
const char* ssid = "IODATA-bd7f38-2G";//多分5Gだめ
const char* password = "1234567890";

//const char* upload_endpoint = "http://34.145.94.229/save-image";//アップロード先
const char* upload_endpoint = "http://34.105.1.151/save-image";//アップロード先
uint64_t starttime;
Camera *camera;
WifiClient *wifi;

void deep_sleep(const uint64_t interval_sec)
{
  uint64_t sleeptime = interval_sec * 1000000 - (millis() - starttime) * 1000;
  esp_deep_sleep(sleeptime);  // DeepSleepモードに移行
}


void setup()
{
  camera = new Camera();
  wifi = new WifiClient();

  starttime = millis();

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true); //設定を表示
  Serial.println();
  Serial.println("start!!");//確認用
  wifi->begin(ssid, password);//繋がればconnectedを表示
  camera->begin();

  //  [E][camera.c:1113] camera_probe(): Detected camera not supported.
  //  [E][camera.c:1379] esp_camera_init(): Camera probe failed with error 0x20004 電源が足りていないと思っていたらモデルを間違えていた
  //　Bモデルが正解

}

void loop() {
  // put your main code here, to run repeatedly:
  const char *fb_buf;
  size_t fb_len;

  camera->capture(&fb_buf, &fb_len);
  wifi->post(upload_endpoint, fb_buf, fb_len);
  camera->free();

  deep_sleep(SLEEP_INTERVAL_SEC);
  // deep sleepするとこの先は処理されません。loop()も一度だけです
}
