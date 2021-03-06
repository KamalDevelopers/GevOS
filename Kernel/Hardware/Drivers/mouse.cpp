#include "mouse.hpp"

MouseDriver::MouseDriver(InterruptManager* manager, int screenw, int screenh)
    : InterruptHandler(manager, 0x2C)
    , dataport(0x60)
    , commandport(0x64)
{
    offset = 0;
    buttons = 0;
    w = screenw;
    h = screenh;

    commandport.Write(0xA8);
    commandport.Write(0x20);
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::OnMouseMove(int x, int y)
{
    //x /= 2;
    //y /= 2;

    int32_t new_mouse_x = mouse_x + x;
    if (new_mouse_x < 0)
        new_mouse_x = 0;
    if (new_mouse_x >= w - 2)
        new_mouse_x = w - 2;

    int32_t new_mouse_y = mouse_y + y;
    if (new_mouse_y < 0)
        new_mouse_y = 0;
    if (new_mouse_y >= h - 20)
        new_mouse_y = h - 20;

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;
}

void MouseDriver::OnMouseUp()
{
    mouse_press = 0;
}

void MouseDriver::OnMouseDown(int b)
{
    if (b == 9) {
        mouse_press = 1; //Left Click
    } else if (b == 10) {
        mouse_press = 2; //Right Click
    } else if (b == 12) {
        mouse_press = 3; //Middle Click
    }
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if (!(status & 0x20))
        return esp;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    if (offset == 0) {
        if (buffer[1] != 0 || buffer[2] != 0) {
            OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }

        for (uint8_t i = 0; i < 3; i++) {
            if ((buffer[0] & (0x1 << i)) != (buttons & (0x1 << i))) {
                if (buttons & (0x1 << i))
                    OnMouseUp();
                else
                    OnMouseDown(buffer[0]);
            }
        }

        buttons = buffer[0];
    }
    return esp;
}
