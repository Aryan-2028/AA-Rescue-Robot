#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// ================= CAMERA MODEL =================

#include "board_config.h"

// ================= WIFI =================

const char* ssid = "Aryan_4G";
const char* password = "ARYAN1974**";

// ================= SENSOR VALUES =================

int distanceValue = 0;
float tempValue = 0;
float humidityValue = 0;
int gasValue = 0;

String botStatus = "WAITING";

// ================= DASHBOARD SERVER =================

WebServer dashboard(82);

// ================= CAMERA SERVER =================

void startCameraServer();
void setupLedFlash();

// ================= AJAX DATA API =================

void handleData()
{
  String data = "";

  data += String(distanceValue);
  data += ",";

  data += String(tempValue);
  data += ",";

  data += String(humidityValue);
  data += ",";

  data += String(gasValue);
  data += ",";

  data += botStatus;

  dashboard.send(200, "text/plain", data);
}

// ================= DASHBOARD PAGE =================

void handleDashboard()
{
  String page = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>AA ROBOTICS</title>

<style>

body{
    margin:0;
    background:#050505;
    color:white;
    font-family:Arial;
}

.header{
    background:#0d0d0d;
    padding:25px;
    text-align:center;
    border-bottom:1px solid #333;
}

.header h1{
    margin:0;
    font-size:48px;
    color:white;
    letter-spacing:5px;
}

.header p{
    color:#888;
    margin-top:10px;
}

.container{
    display:flex;
    flex-wrap:wrap;
    gap:20px;
    padding:20px;
}

.left{
    flex:2;
    min-width:350px;
}

.right{
    flex:1;
    min-width:300px;
}

.card{
    background:#101010;
    border:1px solid #222;
    border-radius:25px;
    padding:25px;
    margin-bottom:20px;
    box-shadow:0 0 20px rgba(255,255,255,0.03);
}

.card h2{
    color:#bbbbbb;
    margin-top:0;
    font-size:28px;
}

.value{
    font-size:45px;
    font-weight:bold;
    color:white;
}

.cameraCard img{
    width:100%;
    border-radius:20px;
    border:2px solid #333;
}

.openBtn{
    display:block;
    text-align:center;
    margin-top:20px;
    background:white;
    color:black;
    text-decoration:none;
    padding:16px;
    border-radius:15px;
    font-weight:bold;
    transition:0.3s;
}

.openBtn:hover{
    background:#ddd;
}

.remote{
    display:grid;
    grid-template-columns:1fr 1fr 1fr;
    gap:15px;
}

.btn{
    background:#1a1a1a;
    border:1px solid #333;
    padding:22px;
    text-align:center;
    border-radius:18px;
    font-size:28px;
    font-weight:bold;
    color:white;
}

.status{
    color:#00ff99;
}

.footer{
    text-align:center;
    color:#666;
    padding:25px;
    font-size:14px;
}

</style>

</head>

<body>

<div class="header">

<h1>AA</h1>

<p>SMART SURVEILLANCE & SAFETY ROBOTICS</p>

</div>

<div class="container">

<div class="left">

<div class="card cameraCard">

<h2>LIVE CAMERA</h2>

<img src="http://192.168.29.229:81/stream">

<a class="openBtn"
href="http://192.168.29.229"
target="_blank">

OPEN CAMERA CONTROL PANEL

</a>

</div>

<div class="card">

<h2>Obstacle Distance</h2>

<div id="distance" class="value">0 cm</div>

</div>

<div class="card">

<h2>Temperature</h2>

<div id="temp" class="value">0 °C</div>

</div>

<div class="card">

<h2>Humidity</h2>

<div id="humidity" class="value">0 %</div>

</div>

<div class="card">

<h2>Gas Sensor</h2>

<div id="gas" class="value">0</div>

</div>

<div class="card">

<h2>Robot Status</h2>

<div id="status" class="value status">WAITING</div>

</div>

</div>

<div class="right">

<div class="card">

<h2>FLYSKY CONTROLLER</h2>

<div class="remote">

<div></div>
<div class="btn">↑</div>
<div></div>

<div class="btn">←</div>
<div class="btn">STOP</div>
<div class="btn">→</div>

<div></div>
<div class="btn">↓</div>
<div></div>

</div>

</div>

</div>

</div>

<div class="footer">

AA ROBOTICS • ESP32-CAM SURVEILLANCE SYSTEM

</div>

<script>

async function updateData()
{
    try
    {
        const response =
        await fetch('/data');

        const text =
        await response.text();

        const values =
        text.split(',');

        document.getElementById("distance").innerHTML =
        values[0] + " cm";

        document.getElementById("temp").innerHTML =
        values[1] + " °C";

        document.getElementById("humidity").innerHTML =
        values[2] + " %";

        document.getElementById("gas").innerHTML =
        values[3];

        document.getElementById("status").innerHTML =
        values.slice(4).join(",");
    }
    catch(e)
    {
        console.log(e);
    }
}

// EXTREME SMOOTH LIVE UPDATE

setInterval(updateData, 200);

updateData();

</script>

</body>
</html>

)rawliteral";

  dashboard.send(200, "text/html", page);
}

// ================= SETUP =================

void setup()
{
  Serial.begin(115200);

  // UNO DATA RX

  Serial2.begin(9600, SERIAL_8N1, 3, -1);

  Serial.println();

  Serial.println("ESP32-CAM STARTING");

  // ================= CAMERA CONFIG =================

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

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;

  config.frame_size = FRAMESIZE_UXGA;

  config.pixel_format = PIXFORMAT_JPEG;

  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  config.fb_location = CAMERA_FB_IN_PSRAM;

  config.jpeg_quality = 12;

  config.fb_count = 1;

  if(config.pixel_format == PIXFORMAT_JPEG)
  {
    if(psramFound())
    {
      config.jpeg_quality = 10;

      config.fb_count = 2;

      config.grab_mode = CAMERA_GRAB_LATEST;
    }
    else
    {
      config.frame_size = FRAMESIZE_SVGA;

      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  }

  esp_err_t err = esp_camera_init(&config);

  if(err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);

    return;
  }

  sensor_t *s = esp_camera_sensor_get();

  if(s->id.PID == OV3660_PID)
  {
    s->set_vflip(s, 1);

    s->set_brightness(s, 1);

    s->set_saturation(s, -2);
  }

  s->set_framesize(s, FRAMESIZE_QVGA);

  // ================= WIFI =================

  WiFi.begin(ssid, password);

  WiFi.setSleep(false);

  Serial.print("Connecting To WiFi");

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);

    Serial.print(".");
  }

  Serial.println("");

  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");

  Serial.println(WiFi.localIP());

  // ================= CAMERA SERVER =================

  startCameraServer();

  // ================= DASHBOARD =================

  dashboard.on("/", handleDashboard);

  dashboard.on("/data", handleData);

  dashboard.begin();

  Serial.println("Dashboard Ready");

  Serial.print("Dashboard URL: http://");

  Serial.print(WiFi.localIP());

  Serial.println(":82");

  Serial.print("Camera URL: http://");

  Serial.println(WiFi.localIP());
}

// ================= LOOP =================

void loop()
{
  dashboard.handleClient();

  // ================= RECEIVE UNO DATA =================

  if(Serial2.available())
  {
    String data = Serial2.readStringUntil('\n');

    data.trim();

    int first = data.indexOf(',');

    int second = data.indexOf(',', first + 1);

    int third = data.indexOf(',', second + 1);

    int fourth = data.indexOf(',', third + 1);

    if(first > 0 &&
       second > 0 &&
       third > 0 &&
       fourth > 0)
    {
      distanceValue =
      data.substring(0, first).toInt();

      tempValue =
      data.substring(first + 1, second).toFloat();

      humidityValue =
      data.substring(second + 1, third).toFloat();

      gasValue =
      data.substring(third + 1, fourth).toInt();

      botStatus =
      data.substring(fourth + 1);

      botStatus.trim();
    }
  }

  delay(5);
}