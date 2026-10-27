#include "utils.hpp"

void follow_line(const String& raw_input) {
  // STOP if no line is detected
  if (raw_input == "STOP") {
    Serial.println(raw_input);
    return;
  }

  float cte = raw_input.toFloat(); // convert raw_input into the actual cte float
  float dt = 0.2; // set to 0.2 for stability, this is stupid as shit

  float left_factor = 1.0; // if robot leans right, increase left factor
  float right_factor = 1.0; // if robot leans left, increase right factor

  float speed_correction = -pid_control(cte, dt); // perform PID control on received cte BEST VALUE: 0.25
  float left_side_speed = constrain(BASE_SPEED - speed_correction * left_factor, 0, TOP_SPEED);
  float right_side_speed = constrain(BASE_SPEED + speed_correction * right_factor, 0, TOP_SPEED);

  char direction = determine_direction(cte); // determine how to shift the car's movement based on CTE

  String pid_cmd = String(left_side_speed) + "," + String(right_side_speed) + "," + String(direction);    
  Serial.println(pid_cmd); // send PID values to Arduino Nano
}

float pid_control(float error, float dt) {
  static float integral = 0;
  static float lastError = 0;

  integral += error * dt;
  float derivative = (error - lastError) / dt;
  float output = K_P * error + K_I * integral + K_D * derivative;
  lastError = error;
  return output;
}

char determine_direction(float cte) {
  if (cte >= INFLECTION_POINT) {
    return 'a'; // turn left
  }

  else if (cte <= -INFLECTION_POINT) {
    return 'd'; // turn right
  }

  else {
    return 'w'; // go forward
  }
}

void cameraServerSetup() {
  camera_config_t config = {};
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
  config.pin_sccb_sda = SIOD_GPIO_NUM; // replaced sscb -> sccb
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Use QVGA resolution for higher frame rate and lower memory usage
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  // Use PSRAM if available
  if (psramFound()) {
    config.fb_location = CAMERA_FB_IN_PSRAM;
  }
  else {
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // Drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_vflip(s, 0);        // 1-Upside down, 0-No operation
  s->set_hmirror(s, 0);      // 1-Reverse left and right, 0-No operation
}

void WiFiSetup() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid_STA, password_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin(port);
  WiFi.setAutoReconnect(true);
}

void updatePID(String input) {
  int firstCommaIndex = input.indexOf(',');
  int secondCommaIndex = input.indexOf(',', firstCommaIndex + 1);
  if (firstCommaIndex <= 0 || secondCommaIndex < firstCommaIndex) {
    return;
  }
  K_P = input.substring(0, firstCommaIndex).toFloat();
  K_I = input.substring(firstCommaIndex + 1, secondCommaIndex).toFloat();
  K_D = input.substring(secondCommaIndex + 1).toFloat();
}