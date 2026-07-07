#include <base/env.h>
#include <base/component.h>
#include <gui_session/connection.h>
#include <input/event.h>
#include <os/pixel_rgb888.h>
#include <util/string.h>

namespace Calculator {
    using namespace Genode;
    using PT = Pixel_rgb888;

    constexpr int WIDTH = 240;
    constexpr int HEIGHT = 320;

    struct Button {
        const char *label;
        int x, y, w, h;
    };

    static constexpr Button buttons[] = {
        {"7", 10,  100, 45, 45}, {"8", 65,  100, 45, 45}, {"9", 120, 100, 45, 45}, {"/", 175, 100, 55, 45},
        {"4", 10,  155, 45, 45}, {"5", 65,  155, 45, 45}, {"6", 120, 155, 45, 45}, {"*", 175, 155, 55, 45},
        {"1", 10,  210, 45, 45}, {"2", 65,  210, 45, 45}, {"3", 120, 210, 45, 45}, {"-", 175, 210, 55, 45},
        {"C", 10,  265, 45, 45}, {"0", 65,  265, 45, 45}, {"=", 120, 265, 45, 45}, {"+", 175, 265, 55, 45}
    };
    constexpr int NUM_BUTTONS = sizeof(buttons) / sizeof(Button);

    // دیتابیس بیت‌مپی فونت مینی‌مال (۵x۵ برای کاراکترهای 0-9 و علائم)
    static const unsigned char font_data[][5] = {
        {0x1F, 0x11, 0x11, 0x11, 0x1F}, // 0
        {0x00, 0x00, 0x1F, 0x00, 0x00}, // 1
        {0x1D, 0x15, 0x15, 0x15, 0x17}, // 2
        {0x15, 0x15, 0x15, 0x15, 0x1F}, // 3
        {0x07, 0x04, 0x04, 0x04, 0x1F}, // 4
        {0x17, 0x15, 0x15, 0x15, 0x1D}, // 5
        {0x1F, 0x15, 0x15, 0x15, 0x1D}, // 6
        {0x01, 0x01, 0x01, 0x01, 0x1F}, // 7
        {0x1F, 0x15, 0x15, 0x15, 0x1F}, // 8
        {0x17, 0x15, 0x15, 0x15, 0x1F}, // 9
        {0x04, 0x04, 0x1F, 0x04, 0x04}, // +
        {0x04, 0x04, 0x04, 0x04, 0x04}, // -
        {0x04, 0x0E, 0x15, 0x0E, 0x04}, // *
        {0x04, 0x02, 0x1F, 0x08, 0x04}, // /
        {0x0A, 0x0A, 0x0A, 0x0A, 0x0A}, // =
        {0x1F, 0x11, 0x11, 0x11, 0x11}  // C
    };

    struct Main
    {
        Env &_env;
        ::Gui::Connection _gui { _env, "calculator" };
        Constructible<Attached_dataspace> _fb_ds { };
        Signal_handler<Main> _input_handler { _env.ep(), *this, &Main::_handle_input };

        int pointer_x = WIDTH / 2;
        int pointer_y = HEIGHT / 2;
        char display_text[16] = "0";

        Main(Env &env) : _env(env)
        {
            _gui.input.sigh(_input_handler);
            _gui.buffer({ .area = { WIDTH, HEIGHT }, .alpha = false });
            _fb_ds.construct(_env.rm(), _gui.framebuffer.dataspace());

            render_ui();
            _gui.view({ 1 }, { .title = "Genode Calculator", .rect = { { 300, 200 }, { WIDTH, HEIGHT } }, .front = true });
            log("Ready! Mini-Calculator Started.");
        }

        void draw_char(PT *pixels, int x, int y, char c, PT color) {
            int idx = -1;
            if (c >= '0' && c <= '9') idx = c - '0';
            else if (c == '+') idx = 10;
            else if (c == '-') idx = 11;
            else if (c == '*') idx = 12;
            else if (c == '/') idx = 13;
            else if (c == '=') idx = 14;
            else if (c == 'C') idx = 15;

            if (idx == -1) return;

            for (int col = 0; col < 5; col++) {
                unsigned char b = font_data[idx][col];
                for (int row = 0; row < 5; row++) {
                    if (b & (1 << row)) {
                        for (int py = 0; py < 3; py++) {
                            for (int px = 0; px < 3; px++) {
                                int out_x = x + (col * 3) + px;
                                int out_y = y + (row * 3) + py;
                                if (out_x >= 0 && out_x < WIDTH && out_y >= 0 && out_y < HEIGHT) {
                                    pixels[out_y * WIDTH + out_x] = color;
                                }
                            }
                        }
                    }
                }
            }
        }

        void draw_string(PT *pixels, int x, int y, const char *str, PT color) {
            int current_x = x;
            while (*str) {
                draw_char(pixels, current_x, y, *str, color);
                current_x += 18;
                str++;
            }
        }

        void render_ui()
        {
            PT * const pixels = _fb_ds->local_addr<PT>();
            
            // پس‌زمینه اصلی ماشین حساب
            draw_rect(pixels, 0, 0, WIDTH, HEIGHT, PT(30, 30, 35));
            
            // صفحه نمایش بالایی
            draw_rect(pixels, 10, 20, 220, 60, PT(20, 25, 20));
            draw_string(pixels, 20, 40, display_text, PT(0, 255, 0));

            // رسم تک تک دکمه‌ها همراه با متن کپشن
            for (int i = 0; i < NUM_BUTTONS; i++) {
                PT btn_color = (i % 4 == 3 || i == 14) ? PT(230, 140, 40) : PT(60, 60, 65);
                
                if (pointer_x >= buttons[i].x && pointer_x <= buttons[i].x + buttons[i].w &&
                    pointer_y >= buttons[i].y && pointer_y <= buttons[i].y + buttons[i].h) {
                    btn_color = PT(100, 100, 110);
                }

                draw_rect(pixels, buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, btn_color);

                int text_x = buttons[i].x + (buttons[i].w / 2) - 7;
                int text_y = buttons[i].y + (buttons[i].h / 2) - 7;
                draw_string(pixels, text_x, text_y, buttons[i].label, PT(255, 255, 255));
            }

            // رسم پوینتر کنترلی سرخ‌رنگ
            draw_rect(pixels, pointer_x - 4, pointer_y - 4, 8, 8, PT(255, 0, 0));
            _gui.framebuffer.refresh({ { 0, 0 }, { WIDTH, HEIGHT } });
        }

        void draw_rect(PT *pixels, int x, int y, int w, int h, PT color) {
            for (int i = y; i < y + h; i++) {
                for (int j = x; j < x + w; j++) {
                    if (i >= 0 && i < HEIGHT && j >= 0 && j < WIDTH) {
                        pixels[i * WIDTH + j] = color;
                    }
                }
            }
        }

        unsigned get_len(const char *str) {
            unsigned l = 0;
            while (str[l]) { l++; }
            return l;
        }

        // تبدیل مقدار عددی int به رشته با اعمال کست صریح
        void int_to_str(int num, char *buf, int max_len) {
            if (num == 0) {
                Genode::copy_cstring(buf, "0", max_len);
                return;
            }
            int sign = 1;
            if (num < 0) { sign = -1; num = -num; }

            char temp[16];
            int i = 0;
            while (num > 0 && i < 15) {
                temp[i++] = (char)((num % 10) + '0');
                num /= 10;
            }
            int p = 0;
            if (sign == -1 && p < max_len - 1) buf[p++] = '-';
            while (i > 0 && p < max_len - 1) {
                buf[p++] = temp[--i];
            }
            buf[p] = '\0';
        }

        // پارسر مینی‌مال عبارات ریاضی و اجرای عملیات محاسباتی
        void compute_result() {
            int val1 = 0, val2 = 0;
            char op = 0;
            int i = 0;

            while (display_text[i] >= '0' && display_text[i] <= '9') {
                val1 = val1 * 10 + (display_text[i] - '0');
                i++;
            }

            if (display_text[i] == '+' || display_text[i] == '-' || 
                display_text[i] == '*' || display_text[i] == '/') {
                op = display_text[i];
                i++;
            } else {
                return; 
            }

            while (display_text[i] >= '0' && display_text[i] <= '9') {
                val2 = val2 * 10 + (display_text[i] - '0');
                i++;
            }

            int res = 0;
            if (op == '+') res = val1 + val2;
            if (op == '-') res = val1 - val2;
            if (op == '*') res = val1 * val2;
            if (op == '/') { res = (val2 != 0) ? (val1 / val2) : 0; }

            int_to_str(res, display_text, sizeof(display_text));
        }

        void check_click() {
            for (int i = 0; i < NUM_BUTTONS; i++) {
                if (pointer_x >= buttons[i].x && pointer_x <= buttons[i].x + buttons[i].w &&
                    pointer_y >= buttons[i].y && pointer_y <= buttons[i].y + buttons[i].h) {
                    
                    log("ACTION: Clicked on button [", buttons[i].label, "]");
                    
                    if (Genode::strcmp(buttons[i].label, "C") == 0) {
                        Genode::copy_cstring(display_text, "0", sizeof(display_text));
                    } 
                    else if (Genode::strcmp(buttons[i].label, "=") == 0) {
                        compute_result();
                    } 
                    else {
                        if (Genode::strcmp(display_text, "0") == 0) {
                            Genode::copy_cstring(display_text, buttons[i].label, sizeof(display_text));
                        } else {
                            unsigned len = get_len(display_text);
                            if (len < sizeof(display_text) - 1) {
                                display_text[len] = buttons[i].label[0];
                                display_text[len + 1] = '\0';
                            }
                        }
                    }
                    log("DISPLAY: ", (const char*)display_text);
                    break;
                }
            }
        }

        void _handle_input()
        {
            while (_gui.input.pending()) {
                _gui.input.for_each_event([&] (Input::Event const &ev) {
                    ev.handle_press([&] (Input::Keycode key, Input::Codepoint) {
                        int const key_code = (int)key;

                        if (key_code == 105) pointer_x -= 12;
                        if (key_code == 106) pointer_x += 12;
                        if (key_code == 103) pointer_y -= 12;
                        if (key_code == 108) pointer_y += 12;

                        if (key_code == 28 || key_code == 272 || key_code == 57) {
                            check_click();
                        }

                        if (pointer_x < 4) pointer_x = 4;
                        if (pointer_x > WIDTH - 4) pointer_x = WIDTH - 4;
                        if (pointer_y < 4) pointer_y = 4;
                        if (pointer_y > HEIGHT - 4) pointer_y = HEIGHT - 4;

                        render_ui();
                    });
                });
            }
        }
    };
}

void Component::construct(Genode::Env &env) { static Calculator::Main main { env }; }
