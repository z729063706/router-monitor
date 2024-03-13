#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <JPEGDecoder.h>
#include <TFT_eSPI.h>       // Include the graphics library (this includes the sprite functions)

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

const char* ssid = "PanSiAiDong";
const char* password = "808808808";

// 图像URL
const char* imageURL = "http://192.168.1.15:8080/newshot.jpg";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer

  tft.init();
  tft.setRotation(1); // 根据实际情况调整屏幕方向
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check WiFi connection status

    HTTPClient http;  //Declare an object of class HTTPClient

    http.begin(imageURL);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request

    if (httpCode > 0) { //Check the returning code

      // Get the request response payload
      WiFiClient * stream = http.getStreamPtr();

      // Decode JPEG
      JpegDec.decodeHttpStream(stream);

      // Render the JPEG on the TFT
      drawJPEG();

    }

    http.end();   //Close connection
  }

  delay(10000); // Fetch every 10 seconds
}

void drawJPEG() {
  // Retrieve decoded image dimensions
  uint16_t jpgWidth = JpegDec.width;
  uint16_t jpgHeight = JpegDec.height;

  // Calculate scaling factors and positions
  uint16_t xPos = 0;
  uint16_t yPos = 0;

  // Scale the image to fit the screen width/height while maintaining aspect ratio
  float factor = min((float)tft.width() / jpgWidth, (float)tft.height() / jpgHeight);
  uint16_t scaledWidth = jpgWidth * factor;
  uint16_t scaledHeight = jpgHeight * factor;
  xPos = (tft.width() - scaledWidth) / 2;
  yPos = (tft.height() - scaledHeight) / 2;

  // Render the image on screen
  while (JpegDec.read()) {
    // Draw the pixels
    tft.drawPixel(JpegDec.x * factor + xPos, JpegDec.y * factor + yPos, JpegDec.pixels[0]);
  }
}
