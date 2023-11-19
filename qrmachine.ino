#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <qrcode.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

// The structure to manage the QR code
QRCode qrcode;

// text that comes on the serial port
String serialBuffer;

// Define QR Code properties: https://github.com/ricmoo/QRCode/tree/master
#define QR_VERSION 4
#define QR_ECC 0


/**
  Draws the code for the given content 
  @param content a Text that should be shown
  @param display the display where it should be shown
*/
void drawCode(String content, Adafruit_TFTLCD display)
{
  uint8_t qrcodeBytes[qrcode_getBufferSize(QR_VERSION)];
  qrcode_initText(&qrcode, qrcodeBytes, QR_VERSION, QR_ECC, content.c_str());

  // Get the pixel size
  int ps = (min(display.width(), display.height()) / (qrcode.size+1));

  // Clear screen
  display.fillScreen(BLACK);
  display.fillRect(0, 0, (qrcode.size+1)*ps, (qrcode.size+1)*ps, WHITE);
  
  
  for (uint8_t y = 0; y < qrcode.size; y++) 
  {
    // Left quiet zone
    Serial.print("        ");
    // Each horizontal module
    for (uint8_t x = 0; x < qrcode.size; x++) 
    {
        // Print each module (UTF-8 \u2588 is a solid block)
        Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588\u2588": "  ");
        //display.fillRect(ps/2 + (x*ps), ps/2 + (y*ps), ps, ps, qrcode_getModule(&qrcode, x, y) ? BLACK : WHITE);
        display.fillRoundRect(ps/2 + (x*ps), ps/2 + (y*ps), ps, ps, ps/6.0, qrcode_getModule(&qrcode, x, y) ? BLACK : WHITE);
    }
    Serial.print("\n");
  }

  // Draw the text
  display.setCursor(ps, display.width()+ps);
  display.setTextColor(GREEN);
  display.setTextSize(2);
  display.setTextWrap(true);
  display.println(content);

}

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("TFT LCD test - enter text to display as qr code:"));

 
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  // Start TFT Duisplay
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  drawCode("QR Machine: Enter text via serial console to get a qr code", tft);

  Serial.println(F("Done!"));
}

void loop(void) 
{
  // Check for serial data and trigger the event
    while ( Serial.available() > 0 ) 
    {
      char inchar = (char) Serial.read();
     
      if (inchar != '\r' && inchar != '\n')  
      {
        serialBuffer += inchar;
      }
      else
      {
        drawCode(serialBuffer, tft);
        serialBuffer = "";
      }
    }
}




