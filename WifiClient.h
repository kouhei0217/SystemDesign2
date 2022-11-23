#pragma once

class WifiClient {
  private:
  public:
    void begin(const char *ssid, const char *password);
    void post(const char *host, const char *buffer, const size_t length);
};
