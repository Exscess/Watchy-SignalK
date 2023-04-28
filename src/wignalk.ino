#include "Watchy_GSR.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "string"

RTC_DATA_ATTR boolean SLOW_SIGNALK_UPDATE = false;
RTC_DATA_ATTR boolean FAST_SIGNALK_UPDATE = false;
RTC_DATA_ATTR uint8_t WIFI_STEP = 0;
RTC_DATA_ATTR uint8_t DATA_COG = 0;
RTC_DATA_ATTR float DATA_DPT = 0;
RTC_DATA_ATTR float DATA_BV = 0;
RTC_DATA_ATTR float DATA_BA = 0;
String signalKUrl = "https://demo.signalk.org/signalk/v1/api/vessels/urn:mrn:signalk:uuid:a6eb6272-6715-4dcc-9d44-350be66bbe8a";

class OverrideGSR : public WatchyGSR
{
  /*
   * Keep your functions inside the class, but at the bottom to avoid confusion.
   * Be sure to visit https://github.com/GuruSR/Watchy_GSR/blob/main/Override%20Information.md for full information on how to override
   * including functions that are available to your override to enhance functionality.
   */
public:
  OverrideGSR() : WatchyGSR() {}

  void InsertWiFi()
  {
    switch (WIFI_STEP)
    {
    case 1:
    {
      Serial.println("Start SignalK Wifi");

      HTTPClient http;
      http.setConnectTimeout(3000);

      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Wifi not connected, stop");
        DATA_COG = 0;
        DATA_DPT = 0;
        DATA_BV = 0;
        DATA_BA = 0;
        break;
      }

      http.begin(signalKUrl.c_str());

      int httpResponseCode = http.GET();
      if (httpResponseCode != 200)
      {
        Serial.println(httpResponseCode);
        Serial.println("SignalK not connected, stop");
        DATA_COG = 0;
        DATA_DPT = 0;
        DATA_BV = 0;
        DATA_BA = 0;
        break;
      }

      DynamicJsonBuffer JSONBuffer(32768);
      JsonObject &parsed = JSONBuffer.parseObject(http.getString());

      if (!parsed.success())
      {
        Serial.println("Json not parsed");
        DATA_COG = 0;
        DATA_DPT = 0;
        DATA_BV = 0;
        DATA_BA = 0;
        break;
      }

      Serial.println("Converting data");

      float cog = parsed["navigation"]["courseOverGroundTrue"]["value"];
      float dpt = parsed["environment"]["depth"]["belowTransducer"]["value"];
      float bv = parsed["electrical"]["batteries"]["1"]["voltage"]["value"];
      float ba = parsed["electrical"]["batteries"]["1"]["current"]["value"];

      DATA_COG = static_cast<int8_t>(cog * (180.0 / 3.141592653589793238463));
      DATA_DPT = dpt;
      DATA_BV = bv;
      DATA_BA = ba;

      Serial.println("COG: " + String(DATA_COG));
      Serial.println("DPT: " + String(DATA_DPT));
      Serial.println("BV: " + String(DATA_BV));
      Serial.println("BA: " + String(DATA_BA));

      drawDay();

      if (SLOW_SIGNALK_UPDATE)
      {
        Serial.println("Slow mode: data received, stop sync for a minute");
        endWiFi();
      }

      break;
    }
    case 2:
    {
      break;
    }
    }
  };

  void InsertWiFiEnding()
  {
    Serial.println("WiFiEnding");
    VibeTo(true);
    delay(40);
    VibeTo(false);
  };

  void InsertOnMinute()
  {
    if (SLOW_SIGNALK_UPDATE)
      AskForWiFi();
  }

  bool InsertHandlePressed(uint8_t SwitchNumber, bool &Haptic, bool &Refresh)
  {
    Serial.println(SwitchNumber);
    switch (SwitchNumber)
    {
    case 6: // Up

      if (SLOW_SIGNALK_UPDATE)
      {
        SLOW_SIGNALK_UPDATE = false;
        FAST_SIGNALK_UPDATE = false;
        WIFI_STEP = 2;
        drawDay();
        endWiFi();
      }
      else
      {
        SLOW_SIGNALK_UPDATE = true;
        FAST_SIGNALK_UPDATE = false;
        WIFI_STEP = 1;
        drawDay();
        AskForWiFi();
      }

      VibeTo(true);
      delay(40);
      VibeTo(false);
      return true;
      break;
    case 8:

      if (FAST_SIGNALK_UPDATE)
      {
        SLOW_SIGNALK_UPDATE = false;
        FAST_SIGNALK_UPDATE = false;
        WIFI_STEP = 2;
        drawDay();
        endWiFi();
      }
      else
      {
        SLOW_SIGNALK_UPDATE = false;
        FAST_SIGNALK_UPDATE = true;
        WIFI_STEP = 1;
        drawDay();
        AskForWiFi();
      }

      VibeTo(true);
      delay(40);
      VibeTo(false);

      return true;
      break;
    }
    return false;
  };

  void drawYear()
  {
    display.setFont(Design.Face.YearFont);
    display.setTextColor(Design.Face.YearColor);
    drawDataCustom("C" + String(DATA_COG), Design.Face.YearLeft, Design.Face.Year, Design.Face.YearStyle, Design.Face.Gutter); // 1900
  }

  void drawDay() override
  {
    Serial.println("Draw status");
    display.setFont(&Bronova_Regular13pt7b);
    String status = "NS";
    if (SLOW_SIGNALK_UPDATE)
    {
      status = "SS";
    }
    if (FAST_SIGNALK_UPDATE)
    {
      status = "FS";
    }
    status = status + " D:" + String(DATA_DPT);
    String status2 = "B:" + String(DATA_BV) + "V" + String(DATA_BA) + "A";

    drawDataCustom(status, Design.Face.DayLeft, 25, WatchyGSR::DesOps::dLEFT, Design.Face.DayGutter);
    drawDataCustom(status2, Design.Face.DayLeft, 50, WatchyGSR::DesOps::dLEFT, Design.Face.DayGutter);
  };

  void drawDataCustom(String dData, byte Left, byte Bottom, WatchyGSR::DesOps Style, byte Gutter, bool isTime = false, bool PM = false)
  {
    uint16_t w, Width, Height, Ind;
    int16_t X, Y;

    display.getTextBounds(dData, Left, Bottom, &X, &Y, &Width, &Height);

    Bottom = constrain(Bottom, Gutter, 200 - Gutter);
    switch (Style)
    {
    case WatchyGSR::dLEFT:
      Left = Gutter;
      break;
    case WatchyGSR::dRIGHT:
      Left = constrain(200 - (Gutter + Width), Gutter, 200 - Gutter);
      break;
    case WatchyGSR::dSTATIC:
      Left = constrain(Left, Gutter, 200 - Gutter);
      break;
    case WatchyGSR::dCENTER:
      Left = constrain(4 + ((196 - (Gutter + Width)) / 2), Gutter, 200 - Gutter);
      break;
    };
    display.setCursor(Left, Bottom);
    display.print(dData);

    if (isTime && PM)
    {
      if (Style == WatchyGSR::dRIGHT)
        Left = constrain(Left - 12, Gutter, 200 - Gutter);
      else
        Left = constrain(Left + Width + 6, Gutter, 190);
      display.drawBitmap(Left, Bottom - Design.Face.TimeHeight, PMIndicator, 6, 6, ForeColor());
    }
  }
};

// Do not edit anything below this, leave all of your code above.
OverrideGSR watchy;

void setup()
{
  Serial.begin(115200);
  watchy.init();
}

void loop() {}
