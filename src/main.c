#include <gb/gb.h>
#include <gb/drawing.h>
#include <string.h>
#include <stdint.h>


#define CAPTION_START_TILE_Y   13u

#define MIN_SCANLINE_NUM   0u
#define MAX_SCANLINE_NUM 153u
#define VBL_START        144u

#define SCALE_Y          4u

#define RANGE_SIZE       (MAX_SCANLINE_NUM + 1u)
#define MAX_COUNT_VALUE  ((CAPTION_START_TILE_Y * 8) - 1 - SCALE_Y) // Screen height draw limit

uint8_t scanline_counts[RANGE_SIZE];


// Put the button vars in HRAM for faster access
SFR buttons, buttons_last, scanline_num, discard_count;


void redraw_background(void) {
    // Clear screen
    color (WHITE, WHITE, SOLID);
    box(0u, 0u, DEVICE_SCREEN_PX_WIDTH - 1u, DEVICE_SCREEN_PX_HEIGHT - 1u, M_FILL);

    // VBLank coloring in graph
    color (LTGREY, LTGREY, SOLID);
    box(VBL_START, 0u, MAX_SCANLINE_NUM, DEVICE_SCREEN_PX_HEIGHT - 1u, M_FILL);

    color (BLACK, WHITE, SOLID);
    // Print info about what this is
    gotogxy(0, CAPTION_START_TILE_Y + 1u);
    gprintf("Button press");
    gotogxy(0, CAPTION_START_TILE_Y + 2u);
    gprintf("scanline chart");

    gotogxy(0, CAPTION_START_TILE_Y);
    gprintf("0----------------");

    // 144 label
    gotogxy(17, CAPTION_START_TILE_Y);
    gprintf("1");
    gotogxy(17, CAPTION_START_TILE_Y + 1u);
    gprintf("4");
    gotogxy(17, CAPTION_START_TILE_Y + 2u);
    gprintf("3");

    // vbl label
    color (WHITE, LTGREY, SOLID);
    gotogxy(18, CAPTION_START_TILE_Y);
    gprintf("v");
    gotogxy(18, CAPTION_START_TILE_Y + 1u);
    gprintf("b");
    gotogxy(18, CAPTION_START_TILE_Y + 2u);
    gprintf("l");

    color (BLACK, WHITE, SOLID);
    gotogxy(0, CAPTION_START_TILE_Y + 3u);
    gprintf("Reset:A+B+ST+SL");
    gotogxy(0, CAPTION_START_TILE_Y + 4u);
    gprintf("Int Off/On:SL+DN/UP");
}


void reset_vars(void) {

    memset(scanline_counts, 0, sizeof(scanline_counts));
    discard_count = 0u;
}


void reinit(void) {
    redraw_background();
    reset_vars();
}


void redraw_entire_histograph(void) {

    redraw_background();

    for (uint8_t c = 0u; c < MAX_SCANLINE_NUM; c++) {
        uint8_t count = scanline_counts[scanline_num] * SCALE_Y;
        if (count > 0u) {
            line(scanline_num, 0, scanline_num, count + (SCALE_Y - 1u));
        }
    }
}


void main(void)
{
    // Enter APA drawing mode
    mode(M_DRAWING);

    reinit();

    // Prevent vblank oam copy from interfering with sampling
    DISABLE_OAM_DMA;

    while (1) {

        // Sample buttons with constant fury! No Vsyncing!
        buttons_last = buttons;
        buttons = joypad();
        // Get the scanline at time of button press
        scanline_num = LY_REG;

        // Check for button press
        if (buttons != 0) {
            if (buttons != buttons_last) {
                if (discard_count > 0) {
                    discard_count--;
                } else {
                   // Plot the updated scanline, count is the Y axis
                    uint8_t count = scanline_counts[scanline_num] * SCALE_Y;
                    if (count < MAX_COUNT_VALUE) {
                        line(scanline_num, count, scanline_num, count + (SCALE_Y - 1u));
                        scanline_counts[scanline_num]++;

                        // After discard the next sample to try and reduce any biasing
                        // toward vblank, etc after the draw above which may wait toward vbl or hbl at times
                        discard_count = 1u;
                    }
                }
            }
            else if (buttons == (J_SELECT | J_UP)) {
                enable_interrupts();
            }
            else if (buttons == (J_SELECT | J_DOWN)) {
                // There is still an APA vblank interrupt running,
                // so for better results:
                // 1. Turn interrupts off (display will look off)
                // 2. Mash buttons
                // 3. Turn interrupts back on (display correct again)
                disable_interrupts();
            }
            else if (buttons == (J_A | J_B | J_START | J_SELECT)) {
                reinit();
            }
        }
    }
}

