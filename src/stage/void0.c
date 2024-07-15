/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "void0.h"

#include "../stage.h"
#include "../archive.h"
#include "../mem.h"
#include "../mutil.h"
#include "../timer.h"

// Define constants and parameters
#define EFFECT_TYPE_WAVY 2

// Global variables for effect parameters
int effectType = EFFECT_TYPE_WAVY;
double uSpeed = 0.1;
double uFrequency = 0.1;
double uWaveAmplitude = 10.0;

// Function to convert double to fixed point
fixed_t double_to_fixed(double value) {
    return (fixed_t)(value * (1 << FIXED_SHIFT));
}

//Void 0 background structure
typedef struct
{
    //Stage background base structure
    StageBack back;
    
    //Textures
    Gfx_Tex tex_back0; //Stage and back
} Back_Void0;

// Static parameters for warping effect
static fixed_t void0_back_paraly[] = {
    FIXED_DEC(120, 100),
    FIXED_DEC(120, 100),
    FIXED_DEC(120, 100),
    FIXED_DEC(120, 100),
    FIXED_DEC(56, 10),
    FIXED_DEC(104, 10),
};

static fixed_t void0_back_warpx[] = {
    FIXED_DEC(40, 1),
    FIXED_DEC(40, 1),
    FIXED_DEC(40, 1),
    FIXED_DEC(32, 1),
    FIXED_DEC(32, 1),
    FIXED_DEC(24, 1),
};

static fixed_t void0_back_warpy[] = {
    FIXED_DEC(100, 10),
    FIXED_DEC(80, 10),
    FIXED_DEC(60, 10),
    FIXED_DEC(40, 10),
    FIXED_DEC(40, 10),
    FIXED_DEC(100, 10),
};

// Function to calculate warped X position
static s32 Back_Void0_GetX(int x, int y, u32 count) {
    return ((fixed_t)x << (FIXED_SHIFT + 7)) + FIXED_DEC(-320, 1) - FIXED_MUL(0, void0_back_paraly[y]) + ((MUtil_Cos((count << 2) + ((x + y) << 5)) * void0_back_warpx[y]) >> 7);
}

// Function to calculate warped Y position
static s32 Back_Void0_GetY(int x, int y, u32 count) {
    return ((fixed_t)y << (FIXED_SHIFT + 6)) + FIXED_DEC(-172, 1) - FIXED_MUL(0, void0_back_paraly[y]) + ((MUtil_Sin((count << 2) + ((x + y) << 5)) * void0_back_warpy[y]) >> 7);
}

// Function to draw the wavy background
void Back_Void0_DrawBG3(StageBack *back)
{
    Back_Void0 *this = (Back_Void0*)back;

    // Get the current camera position
    fixed_t fx = stage.camera.x;
    fixed_t fy = stage.camera.y;

    // Draw 32x32 quads of the background
    for (int y = 0; y < 5; y++) {
        RECT back_src = {0, y * 32, 32, 32};
        for (int x = 0; x < 8; x++) {
            // Calculate warped coordinates
            fixed_t points[4][2];
            points[0][0] = Back_Void0_GetX(x, y, animf_count);
            points[0][1] = Back_Void0_GetY(x, y, animf_count);
            points[1][0] = Back_Void0_GetX(x + 1, y, animf_count);
            points[1][1] = Back_Void0_GetY(x + 1, y, animf_count);
            points[2][0] = Back_Void0_GetX(x + 1, y + 1, animf_count);
            points[2][1] = Back_Void0_GetY(x + 1, y + 1, animf_count);
            points[3][0] = Back_Void0_GetX(x, y + 1, animf_count);
            points[3][1] = Back_Void0_GetY(x, y + 1, animf_count);

            // Draw quad and increment source rect
            if (stage.stage_id == StageId_1_1) {
                Stage_DrawTexArb(&this->tex_back0, &back_src, points[0], points[1], points[3], points[2], stage.camera.bzoom, stage.camera.angle);
            }
            if ((back_src.x += 32) >= 0xE0) {
                back_src.w--;
            }
        }
    }
}

// Function to free the background resources
void Back_Void0_Free(StageBack *back)
{
    Back_Void0 *this = (Back_Void0*)back;
    
    // Free structure
    Mem_Free(this);
}

// Function to initialize and load the background
StageBack *Back_Void0_New(void)
{
    // Allocate background structure
    Back_Void0 *this = (Back_Void0*)Mem_Alloc(sizeof(Back_Void0));
    if (this == NULL) {
        return NULL;
    }
    
    // Set background functions
    this->back.draw_hud = NULL;
    this->back.draw_fg = NULL;
    this->back.draw_md = NULL;
    this->back.draw_bg = Back_Void0_DrawBG3;
    this->back.free = Back_Void0_Free;

    //Use non-pitch black background
	Gfx_SetClear(0, 0, 0);
    
    // Load background textures
    IO_Data arc_back = IO_Read("\\BG\\VOID0.ARC;1");
    Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
    Mem_Free(arc_back);
    
    return (StageBack*)this;
}
