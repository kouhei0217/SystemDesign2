#pragma once

#include "esp_camera.h"

class Camera {
  private:
    camera_fb_t *fb;
  public:
    Camera();
    void begin();
    esp_err_t capture(const char **fb_buf, size_t *fb_len);
    esp_err_t capture2(const char **fb_buf, size_t *fb_len);
    void free();
};
