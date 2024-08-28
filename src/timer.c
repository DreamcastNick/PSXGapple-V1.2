/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "timer.h"

#include "font.h"
#include "stage.h"

#define TIMER_BITS (3)

int blockx;  // Initial position for blockx
int blockx2; // Initial position for blockx2

//Timer state
u32 frame_count, animf_count;
u32 timer_count, timer_lcount, timer_countbase;
u32 timer_persec;

fixed_t timer_sec, timer_dt, timer_secbase;

u8 timer_brokeconf;

//Timer interface
extern void InterruptCallback(int index, void *cb);
extern void ChangeClearRCnt(int t, int m);

void Timer_Callback(void) {
	timer_count++;
}

void Timer_Init(void)
{
	//Check if system is PAL
	u32 status = *((volatile const u32*)0x1f801814);
	boolean pal_console = *((volatile const char*)0xbfc7ff52) == 'E';
	boolean pal_video = (status & 0x00100000) != 0;
	
	//Initialize counters
	frame_count = animf_count = timer_count = timer_lcount = timer_countbase = 0;
	timer_sec = timer_dt = timer_secbase = 0;
	
	//Setup counter IRQ
	static const u32 hblanks_per_sec[4] = {
		15734 >> TIMER_BITS, //!console && !video => 262.5 * 59.940
		15591 >> TIMER_BITS, //!console &&  video => 262.5 * 59.393
		15769 >> TIMER_BITS, // console && !video => 312.5 * 50.460
		15625 >> TIMER_BITS, // console &&  video => 312.5 * 50.000
	};
	timer_persec = hblanks_per_sec[(pal_console << 1) | pal_video];
	
	EnterCriticalSection();
	
	SetRCnt(RCntCNT1, 1 << TIMER_BITS, RCntMdINTR);
	InterruptCallback(5, (void*)Timer_Callback); //IRQ5 is RCNT1
	StartRCnt(RCntCNT1);
	ChangeClearRCnt(1, 0);
	
	ExitCriticalSection();
	
	timer_brokeconf = 0;
}

void Timer_Tick(void)
{
	u32 status = *((volatile const u32*)0x1f801814);
	
	//Increment frame count
	frame_count++;
	
	//Update counter time
	if (timer_count == timer_lcount)
	{
		if (timer_brokeconf != 0xFF)
			timer_brokeconf++;
		if (timer_brokeconf >= 10)
		{
			if ((status & 0x00100000) != 0)
				timer_count += timer_persec / 50;
			else
				timer_count += timer_persec / 60;
		}
	}
	else
	{
		if (timer_brokeconf != 0)
			timer_brokeconf--;
	}
	
	if (timer_count < timer_lcount)
	{
		timer_secbase = timer_sec;
		timer_countbase = timer_lcount;
	}
	fixed_t next_sec = timer_secbase + FIXED_DIV(timer_count - timer_countbase, timer_persec);
	
	timer_dt = next_sec - timer_sec;
	timer_sec = next_sec;
	
	animf_count = (timer_sec * 24) >> FIXED_SHIFT;
	
	timer_lcount = timer_count;
}

void Timer_Reset(void)
{
	Timer_Tick();
	timer_dt = 0;
}

void StageInfo_Draw()
{
    // Draw song name
    stage.font_cdr.draw(&stage.font_cdr,
        stage.stage_def->composer,
        FIXED_DEC(blockx, 1),
        FIXED_DEC(-50, 1),
        FontAlign_Right
    );

    stage.font_cdr.draw(&stage.font_cdr,
        stage.stage_def->credits,
        FIXED_DEC(blockx, 1),
        FIXED_DEC(-40, 1),
        FontAlign_Right
    );

    // Define block source and destination rectangles
    RECT block_src = { 2, 240, 3, 3 };
    RECT_FIXED block_dst = {
        FIXED_DEC(blockx2, 1),        // Adjusted initial x position
        FIXED_DEC(-60, 1),
        FIXED_DEC(200, 1),            // Adjusted width for full visibility
        FIXED_DEC(32, 1)
    };

    // Reset block position when stage reloads
    if (stage.song_step == -29) {
        blockx = -480;   // Initial x position off-screen to the left
        blockx2 = -480;  // Initial x2 position off-screen to the left
    }

	// Determine block movement based on song step
	if (stage.song_step >= 0 && stage.song_step <= 24) {
		// Slide in towards coordinates -10
		blockx = -480 + (stage.song_step * ((-10 + 480) / 24));  // Adjusted slide in distance to reach -10
		blockx2 = -480 + (stage.song_step * 12); // Existing slide in distance remains unchanged
	}

	// Determine block movement based on song step
	else if (stage.song_step >= 48 && stage.song_step <= 60) {
		// Slide out towards coordinates -480
		blockx = -10 + ((stage.song_step - 25) * ((-480 + 10) / 24));  // Adjusted slide out distance to reach -480
		blockx2 = -240 - ((stage.song_step - 48) * 12); // Existing movement can remain or be adjusted as needed
	}

    // Ensure block movement is bounded within limits
    if (blockx > -10) {
        blockx = -10;
    }
    if (blockx < -480) {
        blockx = -480;
    }
    if (blockx2 > -240) {
        blockx2 = -240;
    }
    if (blockx2 < -480) {
        blockx2 = -480;
    }

    // Draw the block with blended texture
    Stage_BlendTex(&stage.tex_hude, &block_src, &block_dst, stage.bump, stage.camera.hudangle, 0);
}
