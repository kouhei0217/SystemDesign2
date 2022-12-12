#include "Camera.h"
#include "camera_pins.h"
#include "M5Stack.h"

extern "C" {
#include "crypto/base64.h"
}
Camera::Camera()
{
  fb = NULL;
}

void
Camera::begin()
{
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
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  //追加
#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif
  //
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//反転
    s->set_brightness(s, 1);//明るく
    s->set_saturation(s, -2);//飽和を下げる
  }

  // 好きなようにカメラの設定を変更します
  s->set_framesize(s, FRAMESIZE_SVGA);  // 大きさを800x600にします
  s->set_ae_level(s, 1);  // 少し明るくします

  Serial.println("Camera init Successful");
  delay(1500);  // waiting for Camera Sensor stabilize
}
// 引数に指定されたバッファポインタをJPEG画像の領域に当てるようにします
esp_err_t
Camera::capture(const char **fb_buf, size_t *fb_len)
{
  esp_err_t res = ESP_OK;

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return ESP_FAIL;
  }

  if (fb->format == PIXFORMAT_JPEG) {
    *fb_len = fb->len;
    *fb_buf = (const char *)fb->buf;
    Serial.println("JPEG captured");
  }
  return res;
}

esp_err_t
Camera::capture2(const char **fb_buf, size_t *fb_len)
{
  unsigned char * encoded;
  esp_err_t res = ESP_OK;

  fb = esp_camera_fb_get();//画像取得
  if (!fb) {
    Serial.println("Camera capture failed");
    return ESP_FAIL;
  }//取れなかった場合
  if (fb->format == PIXFORMAT_JPEG) {
    *fb_len = fb->len;
    *fb_buf = (const char *)fb->buf;
    Serial.println("JPEG captured");
    encoded = base64_encode((const unsigned char*)fb_buf, strlen(*fb_buf), fb_len);
    *fb_buf = (const char *)encoded;
    Serial.println("base64 captured");
    Serial.print("Length of encoded message: ");
    Serial.println(*fb_buf);
  }
  return res;
}
void
Camera::free()
{
  if (fb) {
    esp_camera_fb_return(fb);
    fb = NULL;
  }
}