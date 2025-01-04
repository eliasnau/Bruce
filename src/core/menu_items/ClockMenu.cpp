#include "ClockMenu.h"
#include "core/display.h"
#include "core/settings.h"

void ClockMenu::optionsMenu() {
    isAnalog = false;
    runClockLoop();
}

void ClockMenu::runClockLoop() {
    int tmp = 0;
    bool redraw = true;

    #if defined(HAS_RTC)
        RTC_TimeTypeDef _time;
        cplus_RTC _rtc;
        _rtc.GetBm8563Time();
        _rtc.GetTime(&_time);
    #endif

    // Clear screen and delay to avoid button bounce
    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    for (;;) {
        if (millis() - tmp > 1000 || redraw) {
            drawClockFace();
            
            #if !defined(HAS_RTC)
                updateTimeStr(rtc.getTimeStruct());
            #endif

            tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
            tft.drawRect(10, 10, tftWidth-15, tftHeight-15, bruceConfig.priColor);

            if (isAnalog) {
                drawAnalogClock();
            } else {
                drawDigitalClock();
            }

            tmp = millis();
            redraw = false;
        }

        if (check(SelPress)) {
            isAnalog = !isAnalog;
            redraw = true;
            delay(200); // Debounce
        }

        if (check(EscPress)) {
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }
        delay(10);
    }
}

void ClockMenu::drawAnalogClock() {
    int centerX = tftWidth / 2;
    int centerY = tftHeight / 2;
    int radius = min(tftWidth, tftHeight) / 3;
    
    // Draw outer circle with double ring
    tft.drawCircle(centerX, centerY, radius, bruceConfig.priColor);
    tft.drawCircle(centerX, centerY, radius - 2, bruceConfig.priColor);
    
    // Draw hour markers
    for (int h = 0; h < 12; h++) {
        float angle = h * 30 * PI / 180;
        if (h % 3 == 0) {
            // Larger markers for 12, 3, 6, 9
            int x1 = centerX + (radius - 15) * sin(angle);
            int y1 = centerY - (radius - 15) * cos(angle);
            int x2 = centerX + radius * sin(angle);
            int y2 = centerY - radius * cos(angle);
            tft.drawWideLine(x1, y1, x2, y2, 3, bruceConfig.priColor, bruceConfig.bgColor);
        } else {
            // Smaller markers for other hours
            int x1 = centerX + (radius - 10) * sin(angle);
            int y1 = centerY - (radius - 10) * cos(angle);
            int x2 = centerX + radius * sin(angle);
            int y2 = centerY - radius * cos(angle);
            tft.drawLine(x1, y1, x2, y2, bruceConfig.priColor);
        }
    }

    // Get current time
    float hour, minute, second;
    #if defined(HAS_RTC)
        RTC_TimeTypeDef _time;
        cplus_RTC _rtc;
        _rtc.GetTime(&_time);
        hour = _time.Hours + _time.Minutes / 60.0;
        minute = _time.Minutes + _time.Seconds / 60.0;
        second = _time.Seconds;
    #else
        struct tm timeInfo = rtc.getTimeStruct();
        hour = timeInfo.tm_hour + timeInfo.tm_min / 60.0;
        minute = timeInfo.tm_min + timeInfo.tm_sec / 60.0;
        second = timeInfo.tm_sec;
    #endif

    // Draw hands with improved styling
    // Hour hand (shorter and thicker)
    drawClockHand(centerX, centerY, hour * 30, radius * 0.45, 4, bruceConfig.priColor);
    // Minute hand (longer and medium thickness)
    drawClockHand(centerX, centerY, minute * 6, radius * 0.65, 3, bruceConfig.priColor);
    // Second hand (longest and thinnest, with counterweight)
    drawClockHand(centerX, centerY, second * 6, radius * 0.8, 1, bruceConfig.priColor);
    // Second hand counterweight
    float radians = ((second * 6) - 90) * PI / 180.0;
    int backX = centerX - (radius * 0.2) * cos(radians);
    int backY = centerY - (radius * 0.2) * sin(radians);
    tft.drawLine(centerX, centerY, backX, backY, bruceConfig.priColor);
    
    // Draw decorative center
    tft.fillCircle(centerX, centerY, 5, bruceConfig.priColor);
    tft.fillCircle(centerX, centerY, 2, bruceConfig.bgColor);
}

void ClockMenu::drawClockHand(int centerX, int centerY, float angle, int length, int width, uint16_t color) {
    float radians = (angle - 90) * PI / 180.0;
    int targetX = centerX + length * cos(radians);
    int targetY = centerY + length * sin(radians);
    tft.drawWideLine(centerX, centerY, targetX, targetY, width, color, bruceConfig.bgColor);
}

void ClockMenu::drawDigitalClock() {
    int centerX = tftWidth / 2;
    int centerY = tftHeight / 2;
    
    #if defined(HAS_RTC)
        RTC_TimeTypeDef _time;
        cplus_RTC _rtc;
        _rtc.GetTime(&_time);
        char timeString[9];
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", _time.Hours, _time.Minutes, _time.Seconds);
        tft.setTextSize(4);
        // Draw shadow effect
        tft.setTextColor(bruceConfig.priColor & 0x39E7, bruceConfig.bgColor);
        tft.drawCentreString(timeString, centerX+1, centerY-12, 1);
        // Draw main text
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString(timeString, centerX, centerY-13, 1);
    #else
        // Draw shadow effect
        tft.setTextSize(4);
        tft.setTextColor(bruceConfig.priColor & 0x39E7, bruceConfig.bgColor);
        tft.drawCentreString(timeStr, centerX+1, centerY-12, 1);
        // Draw main text
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString(timeStr, centerX, centerY-13, 1);
    #endif
}

void ClockMenu::drawClockFace() {
    tft.fillScreen(bruceConfig.bgColor);
}

void ClockMenu::drawIcon(float scale) {
    clearIconArea();

    int radius = scale * 30;
    int pointerSize = scale * 15;

    // Case
    tft.drawArc(
        iconCenterX,
        iconCenterY,
        1.1*radius, radius,
        0, 360,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );

    // Pivot center
    tft.fillCircle(iconCenterX, iconCenterY, radius/10, bruceConfig.priColor);

    // Hours & minutes
    tft.drawLine(
        iconCenterX, iconCenterY,
        iconCenterX - 2*pointerSize/3, iconCenterY - 2*pointerSize/3,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX, iconCenterY,
        iconCenterX + pointerSize, iconCenterY - pointerSize,
        bruceConfig.priColor
    );
}