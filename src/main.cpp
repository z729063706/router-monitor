#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TFT_eSPI.h>       // Hardware-specific library

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

const char* ssid = "PanSiAiDong";
const char* password = "808808808";
const char* imageURL = "http://192.168.1.15:8080/shot1.bmp";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  tft.init();
  tft.setRotation(1); // Adjust according to your display
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  HTTPClient http;
  http.begin(imageURL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    // Get the TCP stream
    WiFiClient * stream = http.getStreamPtr();

    // Read BMP header
    uint8_t bmpHeader[54];
    stream->readBytes(bmpHeader, 54);

    // Assuming the BMP is 24-bit, parse width and height from the header
    uint32_t bmpWidth = bmpHeader[18] + (bmpHeader[19] << 8) + (bmpHeader[20] << 16) + (bmpHeader[21] << 24);
    uint32_t bmpHeight = bmpHeader[22] + (bmpHeader[23] << 8) + (bmpHeader[24] << 16) + (bmpHeader[25] << 24);

    // Now read the bitmap pixels
    // Assuming the image is 500x500, and each pixel is 3 bytes (24-bit BMP)
    for (int y = bmpHeight - 1; y >= 0; y--) { // BMP rows are stored in reverse order
      for (int x = 0; x < bmpWidth; x++) {
        uint8_t b = stream->read(); // Blue
        uint8_t g = stream->read(); // Green
        uint8_t r = stream->read(); // Red

        // Convert RGB to 16-bit color (5-6-5)
        uint16_t color = tft.color565(r, g, b);

        // Draw the pixel
        tft.drawPixel(x, y, color);
      }
    }
  } else {
    Serial.println("Failed to retrieve the image");
  }

  http.end(); // Free resources
  delay(60000); // Update every 60 seconds
}
