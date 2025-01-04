#ifndef __CLOCK_MENU_H__
#define __CLOCK_MENU_H__

#include <MenuItemInterface.h>

class ClockMenu : public MenuItemInterface {
private:
    bool isAnalog = false;
    void drawAnalogClock();
    void drawDigitalClock();
    void drawClockFace();
    void drawClockHand(int centerX, int centerY, float angle, int length, int width, uint16_t color);
    void runClockLoop();

public:
    ClockMenu() : MenuItemInterface("Clock") {}
    void optionsMenu(void);
    void drawIcon(float scale);
};

#endif
