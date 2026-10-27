#pragma once
#include <WiFi.h>
#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

#define BASE_SPEED 90
#define TOP_SPEED 255
#define INFLECTION_POINT 3.0
#define port 5005

// define PID control constants
inline double K_P = 0.9;
inline double K_I = 0.0;
inline double K_D = 0.5;
inline WiFiServer server(port);

inline const char *ssid_STA = "ENGR100-400"; // Enter the router name
inline const char *password_STA = "notapwd777"; // Enter the router password

inline IPAddress local_IP(192, 168, 50, 204); // Set the IP address of ESP32 itself
inline IPAddress gateway(192, 168, 50, 1); // Gateway for this device is the AP/Object Detection ESP32's IP Address
inline IPAddress subnet(255, 255, 255, 0);

void WiFiSetup();

void follow_line(const String& raw_input);
float pid_control(float error, float dt);
char determine_direction(float cte);
void startCameraServer();
void cameraServerSetup();
void updatePID(String input);