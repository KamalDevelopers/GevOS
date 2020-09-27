#include "gui.hpp"

using namespace GUI;

Image::Image(int width, int height, short int* bmp)
{
    widget_width = width;
    widget_height = height;
    bitmap = bmp;
}

void Image::ImageRenderer(unsigned char* data)
{
    int i = 0;
    bitmap = new short int[widget_height * widget_width];
    for (int y = 0; y < widget_height; y++) {
        for (int x = 0; x < widget_width; x++) {
            // clang-format off
            switch (data[i]) {
            case '0': bitmap[i] = 0; break;
            case '1': bitmap[i] = 1; break;
            case '2': bitmap[i] = 2; break;
            case '3': bitmap[i] = 3; break;
            case '4': bitmap[i] = 4; break;
            case '5': bitmap[i] = 5; break;
            case '6': bitmap[i] = 6; break;
            case '7': bitmap[i] = 7; break;
            case '8': bitmap[i] = 8; break;
            case '9': bitmap[i] = 9; break;
            case 'A': bitmap[i] = 10; break;
            case 'B': bitmap[i] = 11; break;
            case 'C': bitmap[i] = 12; break;
            case 'D': bitmap[i] = 13; break;
            case 'E': bitmap[i] = 14; break;
            case 'F': bitmap[i] = 15; break;
            }
            // clang-format on
            i++;
        }
    }
}

void Image::Add(Graphics* vga, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int index = 0;
    for (int y = 0; y < widget_height; y++) {
        for (int x = 0; x < widget_width; x++) {
            if ((x > parentWidth) || (y > parentHeight))
                return;

            if (bitmap[index] != -1)
                vga->PutPixel(x + parentPosX, y + parentPosY, bitmap[index]);
            index++;
        }
    }
}

Label::Label(int xpos, int ypos, int width, int height, uint8_t fcolor, uint8_t bcolor, char* text)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_width = width;
    widget_height = height;

    widget_color = fcolor;
    box_color = bcolor;
    widget_text = text;
}

void Label::Add(Graphics* vga, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int Xsize = str_len(widget_text) * 8; //8 = Bitmap size
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    if (Xsize > parentWidth)
        return;

    if (twidget_xpos + Xsize > parentWidth + parentPosX)
        return;

    if (twidget_ypos + 8 > parentHeight + parentPosY)
        return;

    for (int y = 0; y < widget_height; y++) {
        for (int x = 0; x < widget_width + Xsize; x++)
            vga->PutPixel(twidget_xpos + x, twidget_ypos + y, box_color);
    }

    vga->ResetOffset();
    vga->Print(widget_text, widget_color, twidget_xpos, twidget_ypos);
}

Input::Input(int xpos, int ypos, int width, int height, uint8_t fcolor, uint8_t bcolor, char* text)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_width = width;
    widget_height = height;

    widget_color = fcolor;
    box_color = bcolor;
    widget_text = text;
}

void Input::Add(Graphics* vga, MouseDriver* mouse, KeyboardDriver* keyboard, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    for (int y = 0; y < widget_height; y++) {
        for (int x = 0; x < widget_width; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;
            int x_e = parentPosX + parentWidth;
            int y_e = parentPosY + parentHeight;

            if ((mouse->GetMouseX() == x_t) && (mouse->GetMouseY() == y_t) && (mouse->GetMousePress() == 1))
                active_input = 1;
            if ((active_input != 1) && (hitbox_expand == 1) && (mouse->GetMouseX() < parentPosX + parentWidth) && (mouse->GetMouseX() > parentPosX))
                if ((mouse->GetMouseY() < parentPosY + parentHeight) && (mouse->GetMouseY() > parentPosY) && (mouse->GetMousePress() == 1))
                    active_input = 1;

            if (input_text_index != keyboard->GetIndex() - backslashoffset - enteroffset) {
                if (active_input == 1) {
                    /*if ((keyboard->GetLastKey() == '*') && (input_text_index != 0)){
                        input_text[input_text_index - backslashoffset - 1] = ' ';
                        backslashoffset++;
                    }else*/
                    if ((keyboard->GetLastKey() != '\n') && (keyboard->GetLastKey() != '*')) {
                        if (((input_text_index * 8) + (str_len(widget_text) * 8)) < widget_width) {
                            input_text[input_text_index - backslashoffset] = keyboard->GetLastKey();
                            input_text[input_text_index - backslashoffset + 1] = '\0';
                            input_text_index++;
                        }
                    }
                    if (keyboard->GetLastKey() == '\n') {
                        strcpy(out_data, input_text);
                        //on_press(out_data);
                        klog(out_data);

                        for (int i = 0; i < input_text_index; i++)
                            input_text[i] = '\0';
                        enteroffset = enteroffset + (input_text_index + 1);

                        vga->ResetOffset();
                        vga->Print(widget_text, widget_color, twidget_xpos, twidget_ypos);
                        input_text_index = 0;
                    }
                } else {
                    enteroffset++;
                }
                vga->PutPixel(x_t, y_t, box_color);
            }
        }
    }
    vga->ResetOffset();
    vga->Print(widget_text, widget_color, twidget_xpos, twidget_ypos);
    vga->Print(input_text, widget_color, twidget_xpos, twidget_ypos);
}

Button::Button(int xpos, int ypos, int width, int height, char* text, void (*op)(void))
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_width = width;
    widget_height = height;

    widget_text = text;
    on_press = op;
}

void Button::Add(Graphics* vga, MouseDriver* mouse, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int Xsize = str_len(widget_text) * 8; //8 = Bitmap size
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    if (Xsize > parentWidth)
        return;

    if (twidget_xpos + Xsize > parentWidth + parentPosX)
        return;

    if (twidget_ypos + 8 > parentHeight + parentPosY)
        return;

    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < widget_width + Xsize; x++) {
            if (on_hover_state == 2)
                vga->PutPixel(twidget_xpos + x + 1, twidget_ypos + y + 1, shadow_color);
            else
                vga->PutPixel(twidget_xpos + x + shadow_offset, twidget_ypos + y + shadow_offset, shadow_color);
        }

    on_hover_state = 0;
    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < widget_width + Xsize; x++)
            if ((mouse->GetMouseX() == twidget_xpos + x) && (mouse->GetMouseY() == twidget_ypos + y) && (shadow_offset != 0))
                if (mouse->GetMousePress() == 1)
                    on_hover_state = 2;
                else
                    on_hover_state = 1;

    for (int y = 0; y < widget_height; y++) {
        for (int x = 0; x < widget_width + Xsize; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;

            if ((mouse->GetMouseX() == x_t) && (mouse->GetMouseY() == y_t))
                if (mouse->GetMousePress() == 1)
                    on_press();

            vga->PutPixel(x_t + on_hover_state, y_t + on_hover_state, box_color);
        }
    }

    vga->ResetOffset();
    vga->Print(widget_text, widget_color, on_hover_state + twidget_xpos + 1 + widget_width / 2, on_hover_state + twidget_ypos - 4 + widget_height / 2);
    if (render_image == 1)
        image->Add(vga, on_hover_state + twidget_xpos - 5 + widget_width / 2, on_hover_state + twidget_ypos - 4 + widget_height / 2, widget_width, widget_height);
}

void Button::AddImage(Image* img)
{
    image = img;
    render_image = 1;
}

Panel::Panel(int xpos, int ypos, int width, int height, uint8_t color)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_width = width;
    widget_height = height;
    panel_color = color;
}

void Panel::Add(Graphics* vga, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < widget_width; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;

            vga->PutPixel(x_t, y_t, panel_color);
        }
}

ProgressBar::ProgressBar(int xpos, int ypos, int length)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_length = length;
}

void ProgressBar::Add(Graphics* vga, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    //border color
    for (int y = 0; y < widget_height + 2; y++)
        for (int x = 0; x < widget_length + 2; x++) {
            int x_t = twidget_xpos + x - 1;
            int y_t = twidget_ypos + y - 1;

            vga->PutPixel(x_t, y_t, border_color);
        }
    //Back color
    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < widget_length; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;

            vga->PutPixel(x_t, y_t, widget_color);
        }
    //Bar
    if (progress > 100)
        progress = 100;
    if (progress < 0)
        progress = 0;

    float progressInPixels = (progress / 100) * widget_length;

    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < progressInPixels; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;

            vga->PutPixel(x_t, y_t, bar_color);
        }
}

//Not Finished
CheckBox::CheckBox(int xpos, int ypos)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
}
//Not Finished
void CheckBox::Add(Graphics* vga, MouseDriver* mouse, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    //fill

    for (int y = 0; y < widget_height; y++)
        for (int x = 0; x < widget_width; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;
            if ((mouse->GetMouseX() == x_t) && (mouse->GetMouseY() == y_t))
                if (mouse->GetMousePress() == 1) {
                    if (state == 1) {
                        klog("state is now 0");
                        state = 0;
                    }
                    if (state == 0) {
                        klog("state is now 1");
                        state = 1;
                    }
                }
            if (state == 1)
                vga->PutPixel(x_t, y_t, active_color);
            if (state == 0)
                vga->PutPixel(x_t, y_t, normal_color);
        }
}

Slider::Slider(int xpos, int ypos, int width)
{
    widget_xpos = xpos;
    widget_ypos = ypos;
    widget_width = width;
}

void Slider::Add(Graphics* vga, MouseDriver* mouse, int parentPosX, int parentPosY, int parentWidth, int parentHeight)
{
    int twidget_xpos = widget_xpos + parentPosX;
    int twidget_ypos = widget_ypos + parentPosY;

    for (int y = 0; y < slider_height; y++)
        for (int x = 0; x < widget_width + knob_width; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y;

            vga->PutPixel(x_t, y_t, slider_color);
            vga->PutPixel(x_t + 1, y_t + 1, 0x0);
        }

    for (int y = 0; y < knob_height; y++)
        for (int x = 0; x < knob_width; x++) {
            int x_t = twidget_xpos + x;
            int y_t = twidget_ypos + y - knob_height / 3;

            vga->PutPixel(x_t + valueInPixels, y_t, knob_color);
            vga->PutPixel(x_t + 1 + valueInPixels, y_t + 1, 0x0);
        }

    for (int y = 0; y < knob_height; y++)
        for (int x = 0; x < widget_width + 2; x++) {
            int x_t = twidget_xpos - 1 + x;
            int y_t = twidget_ypos + y - knob_height / 3;

            if ((mouse->GetMouseX() == x_t) && (mouse->GetMouseY() == y_t))
                if (mouse->GetMousePress() == 1) {
                    valueInPixels = mouse->GetMouseX() - widget_xpos;
                }
            //vga->PutPixel(x_t, y_t, 0x4);
        }
}

Window::Window(int xpos, int ypos, int w, int h, uint8_t color, uint8_t wb)
{
    win_bar = wb;
    win_xpos = xpos;
    win_ypos = ypos;
    win_height = h;
    win_width = w;
    win_color = color;
}

uint8_t Window::Begin(Graphics* vga, MouseDriver* mouse, KeyboardDriver* keyboard, uint8_t active)
{
    for (int y = 0 + win_ypos; y < win_height + win_ypos; y++) {
        if (save_mouse_press == 1)
            break;
        for (int x = 0 + win_xpos; x < win_width + win_xpos; x++) {
            vga->PutPixel(x, y, win_color);
            if (x == win_xpos)
                for (int i = 0; i < border_thickness; i++)
                    vga->PutPixel(x + i, y, border_color);

            if (x == win_xpos + win_width - 1)
                for (int i = 0; i < border_thickness; i++)
                    vga->PutPixel(x - i, y, border_color);

            if (y == win_ypos)
                for (int i = 0; i < border_thickness; i++)
                    if (win_bar != 1)
                        vga->PutPixel(x, y + i, border_color);

            if (y == win_ypos + win_height - 1)
                for (int i = 0; i < border_thickness; i++)
                    vga->PutPixel(x, y - i, border_color);
        }
    }

    if (win_bar == 1) {
        for (int y = 0 + win_ypos; y < win_ypos + 10; y++) {
            for (int x = 0 + win_xpos; x < win_width + win_xpos - 10; x++) {
                if ((mouse->GetMouseX() == x) && (mouse->GetMouseY() == y - 4) && (mouse->GetMousePress() == 1) && (active != 1))
                    save_mouse_press = 1;

                if ((mouse->GetMousePress() == 0) && (save_mouse_press == 1))
                    save_mouse_press = 0;

                if (save_mouse_press == 1) {
                    win_xpos = mouse->GetMouseX();
                    win_ypos = mouse->GetMouseY();
                    if (win_xpos >= vga->GetScreenW() - win_width)
                        win_xpos = vga->GetScreenW() - win_width;
                    if (win_ypos >= vga->GetScreenH() - 10)
                        win_ypos = vga->GetScreenH() - 10;
                    if (win_ypos <= 25)
                        win_ypos = 25;
                }
                vga->PutPixel(x, y - 4, 0x8);
            }
        }
        int index = 0;
        for (int x = 0 + win_xpos + win_width - 10; x < 10 + win_xpos + win_width - 10; x++) {
            for (int y = 0 + win_ypos - 4; y < 10 + win_ypos - 4; y++) {
                if ((mouse->GetMouseX() == x) && (mouse->GetMouseY() == y - 4) && (mouse->GetMousePress() == 1))
                    win_hidden = 1;

                if (closewindow_bitmap[index] != -1)
                    vga->PutPixel(x, y, closewindow_bitmap[index]);
                index++;
            }
        }
        vga->ResetOffset();
        vga->Print(win_title, 0x7, win_xpos + 5, win_ypos - 2);
    }

    if (save_mouse_press == 1)
        return active;
    for (int i = 0; i < widget_indexLabel; i++)
        childrenLabel[i]->Add(vga, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexButton; i++)
        childrenButton[i]->Add(vga, mouse, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexInput; i++)
        childrenInput[i]->Add(vga, mouse, keyboard, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexPanel; i++)
        childrenPanel[i]->Add(vga, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexProgressBar; i++)
        childrenProgressBar[i]->Add(vga, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexCheckBox; i++)
        childrenCheckBox[i]->Add(vga, mouse, win_xpos, win_ypos, win_width, win_height);

    for (int i = 0; i < widget_indexSlider; i++)
        childrenSlider[i]->Add(vga, mouse, win_xpos, win_ypos, win_width, win_height);

    active = save_mouse_press;
    return active;
}

void Window::Border(uint8_t thickness, uint8_t color)
{
    border_thickness = thickness;
    border_color = color;
}

void Window::MouseRelease(uint32_t x, uint32_t y, int b)
{
    mouse_down = 0;
}

void Window::MousePress(uint32_t x, uint32_t y, int b, Graphics* vga)
{
    mouse_down = 1;
    if (win_bar == 1) {
        for (int bar_y = 0 + win_ypos; bar_y < 4 + win_ypos; bar_y++) {
            for (int bar_x = 0 + win_xpos; bar_x < win_width + win_xpos; bar_x++) {
                if ((bar_x == x) and (bar_y == y)) {
                    win_xpos = x;
                    win_ypos = y;
                }
            }
        }
    }
}

Desktop::Desktop(int w, int h, Graphics* g, MouseDriver* m, KeyboardDriver* k)
{
    desk_height = h;
    desk_width = w;
    vga = g;
    mouse = m;
    keyboard = k;
}

void Desktop::MouseRelease(uint32_t x, uint32_t y, int b)
{
    for (int i = 0; i < win_index; i++)
        children[i]->MouseRelease(x, y, b);
}

void Desktop::MousePress(uint32_t x, uint32_t y, int b)
{
    for (int i = 0; i < win_index; i++)
        children[i]->MousePress(x, y, b, vga);
}

void Desktop::DrawMouse(int32_t x, int32_t y)
{
    uint8_t tempc;

    if ((old_mouse_x != x) or (old_mouse_y != y)) {
        tempc = *vga->GetPixelColor(x, y);
        vga->PutPixel(x, y, 0x6);
        vga->PutPixel(old_mouse_x, old_mouse_y, old_mouse_color);
        old_mouse_color = tempc;
    }
    old_mouse_x = x;
    old_mouse_y = y;
}

void Desktop::SetWallpaper(Image* img)
{
    desk_wallpaper = img;
    render_wallpapaper = 1;
}

int Desktop::AddWin(int count, ...)
{
    if (count > 100)
        return 1;

    va_list list;
    int j = 0;

    va_start(list, count);
    for (j = 0; j < count; j++) {
        children[win_index] = va_arg(list, Window*);
        win_index++;
    }

    va_end(list);
    return 0;
}

int Desktop::AppendWin(Window* win)
{
    win_index++;
    children[win_index] = win;
    return 0;
}

void Desktop::Draw()
{
    int windex = 0;
    for (int y = 0; y < 480; y++)
        for (int x = 0; x < 640; x++) {
            if (render_wallpapaper == 1)
                vga->PutPixel(x, y, desk_wallpaper->GetColor(windex));
            else
                vga->PutPixel(x, y, 0x0);
            windex++;
        }

    for (int i = 0; i < win_index; i++) {
        if (children[i]->GetDestroy() == 1) {
            deleteElement(i, win_index, children);
            win_index = win_index - 1;
        } else if (children[i]->GetHidden() != 1)
            active_window = children[i]->Begin(vga, mouse, keyboard, active_window);
    }

    int index = 0;
    for (int y = 0; y < 11; y++)
        for (int x = 0; x < 8; x++) {
            if (mouse_bitmap[index] != -1)
                vga->PutPixel(x + mouse->GetMouseX(), y + mouse->GetMouseY(), mouse_bitmap[index]);
            index++;
        }

    vga->RenderScreen();
}

void Desktop::Start()
{
    while (1) {
        Draw();
    }
}
