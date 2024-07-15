/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "menu.h"

#include "mem.h"
#include "main.h"
#include "timer.h"
#include "io.h"
#include "gfx.h"
#include "audio.h"
#include "pad.h"
#include "random.h"
#include "archive.h"
#include "mutil.h"
#include "network.h"

#include "font.h"
#include "trans.h"
#include "loadscr.h"

#include "stage.h"
#include "save.h"
#include "character/gf.h"

#include "stdlib.h"

#include "disc_swap_disc1.h"
#include "disc_swap_disc2.h"
#include "disc_swap_disc3.h"

static u32 Sounds[3];
static boolean first_load = true;
//Menu messages
static const char *funny_messages[][2] = {
	{"PSX PORT BY DRAMCASTNICK", "YOU KNOW IT"},
	{"PORTED BY DREAMCASTNICK", "WHAT YOU GONNA DO"},
	{"FUNKIN", "FOREVER"},
	{"WHAT THE HELL", "BRUTUS PSX"},
	{"LIKE LAMMY", "BUT COOLER"},
	{"THE JAPI", "EL JAPI"},
	{"PICO FUNNY", "PICO FUNNY"},
	{"OPENGL BACKEND", "BY CLOWNACY"},
	{"DCNICK", "SETTING STANDARDS"},
	{"lool", "inverted colours"},
	{"NEVER LOOK AT", "SONIC THE CHRONIC FANART"},
	{"PSXDEV", "HOMEBREW"},
	{"ZERO POINT ZERO TWO TWO EIGHT", "ONE FIVE NINE ONE ZERO FIVE"},
	{"DOPE ASS GAME", "PLAYSTATION MAGAZINE"},
	{"NEWGROUNDS", "FOREVER"},
	{"NO FPU", "NO PROBLEM"},
	{"OK OKAY", "WATCH THIS"},
	{"ITS MORE SAFE", "THAN ANYTHING"},
	{"USE A CONTROLLER", "LOL"},
	{"SNIPING THE KICKSTARTER", "HAHA"},
	{"SHITS UNOFFICIAL", "NOT A PROBLEM"},
	{"SYSCLK", "RANDOM SEED"},
	{"THEY DIDNT HIT THE GOAL", "STOP"},
	{"FCEFUWEFUETWHcfUEZDSLVNSP", "PQRYQWENQWKBVZLZSLDNSVPBM"},
	{"THE FLOORS ARE", "THREE DIMENSIONAL"},
	{"PSXFUNKIN BY CKDEV", "SUCK IT DOWN"},
	{"PLAYING ON EPSXE HUH", "YOURE THE PROBLEM"},
	{"NEXT IN LINE", "ATARI JAGUAR"},
	{"HAXEFLIXEL", "WEAK"},
	{"HAHAHA", "I DONT CARE"},
	{"GET ME TO STOP", "TRY"},
	{"FNF MUKBANG GIF", "THATS UNRULY"},
	{"OPEN SOURCE", "FOREVER"},
	{"ITS A PORT", "ITS BETTER THAN ANDROID OPTIMIZATIONS"},
	{"WOW GATO", "WOW GATO"},
	{"PEE PEE BALLS", "POO POO DICK"},
};


//Menu state
static struct
{
	//Menu state
	u8 page, next_page;
	boolean page_swap;
	u8 select, next_select;

	char scoredisp[30];
	
	fixed_t scroll;
	fixed_t trans_time;
	
	//Page specific state
	union
	{
		struct
		{
			u8 funny_message;
		} opening;
		struct
		{
			fixed_t logo_bump;
			fixed_t fade, fadespd;
		} title;
		struct
		{
			fixed_t fade, fadespd;
		} story;
		struct
		{
			fixed_t back_r, back_g, back_b;
		} freeplay;
	} page_state;
	
	union
	{
		struct
		{
			u8 id, diff;
			boolean story;
		} stage;
	} page_param;
	
	//Menu assets
	Gfx_Tex tex_back, tex_ng, tex_story, tex_title, tex_icon;
	IO_Data bgs, bgs_ptrs[128];
	FontData font_bold, font_arial;
	
	Character *gf; //Title Girlfriend
} menu;

//Internal menu functions
char menu_text_buffer[0x100];

static const char *Menu_LowerIf(const char *text, boolean lower)
{
	//Copy text
	char *dstp = menu_text_buffer;
	if (lower)
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'A' && *srcp <= 'Z')
				*dstp++ = *srcp | 0x20;
			else
				*dstp++ = *srcp;
		}
	}
	else
	{
		for (const char *srcp = text; *srcp != '\0'; srcp++)
		{
			if (*srcp >= 'a' && *srcp <= 'z')
				*dstp++ = *srcp & ~0x20;
			else
				*dstp++ = *srcp;
		}
	}
	
	//Terminate text
	*dstp++ = '\0';
	return menu_text_buffer;
}


static void Menu_DrawBack(boolean flash, s32 scroll, u8 r0, u8 g0, u8 b0, u8 r1, u8 g1, u8 b1)
{
	RECT back_src = {0, 0, 256, 256};
	RECT back_dst = {0, -scroll - SCREEN_WIDEADD2, SCREEN_WIDTH, SCREEN_WIDTH * 4 / 5};
	if (menu.page == MenuPage_Story)
	{
		int back = 0;
		char bgtxt[32];

		switch (menu.select) {
			case 0:
				back = 0;
				break;
			case 1:
				back = 1;
				break;
			case 2:
				back = 2;
				break;
			case 3:
				back = 3;
				break;
			case 4:
				back = 4;
				break;
			case 5:
				back = 5;
				break;
			case 6:
				back = 6;
				break;
			case 7:
				back = 7;
				break;
			default:
				back = RandomRange(8, 14);
				break;
		}

		sprintf(bgtxt, "back%d.tim", back);
    	Gfx_LoadTex(&menu.tex_back, Archive_Find(menu.bgs, bgtxt), 0);
	}
	if (first_load) {
		int back = RandomRange(8, 14);
		char bgtxt[20];
		sprintf(bgtxt, "back%d.tim", back);
		Gfx_LoadTex(&menu.tex_back, Archive_Find(menu.bgs, bgtxt), 0);
		first_load = false;
	}
	if (first_load && menu.page == MenuPage_Freeplay) {
		int back = RandomRange(8, 14);
		char bgtxt[20];
		sprintf(bgtxt, "back%d.tim", back);
		Gfx_LoadTex(&menu.tex_back, Archive_Find(menu.bgs, bgtxt), 0);
		first_load = false;
	}

	if (flash || (animf_count & 4) == 0)
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r0, g0, b0);
	else
		Gfx_DrawTexCol(&menu.tex_back, &back_src, &back_dst, r1, g1, b1);
}

static void Menu_DifficultySelector(s32 x, s32 y)
{
	//Change difficulty
	if (menu.next_page == menu.page && Trans_Idle())
	{
		if (pad_state.press & PAD_LEFT)
		{
			if (menu.page_param.stage.diff > StageDiff_Easy)
				menu.page_param.stage.diff--;
			else
				menu.page_param.stage.diff = StageDiff_Hard;
		}
		if (pad_state.press & PAD_RIGHT)
		{
			if (menu.page_param.stage.diff < StageDiff_Hard)
				menu.page_param.stage.diff++;
			else
				menu.page_param.stage.diff = StageDiff_Easy;
		}
	}
	
	//Draw difficulty arrows
	static const RECT arrow_src[2][2] = {
		{{224, 64, 16, 32}, {224, 96, 16, 32}}, //left
		{{240, 64, 16, 32}, {240, 96, 16, 32}}, //right
	};
	
	Gfx_BlitTex(&menu.tex_story, &arrow_src[0][(pad_state.held & PAD_LEFT) != 0], x - 40 - 16, y - 16);
	Gfx_BlitTex(&menu.tex_story, &arrow_src[1][(pad_state.held & PAD_RIGHT) != 0], x + 40, y - 16);
	
	//Draw difficulty
	static const RECT diff_srcs[] = {
		{  0, 96, 64, 18},
		{ 64, 96, 80, 18},
		{144, 96, 64, 18},
	};
	
	const RECT *diff_src = &diff_srcs[menu.page_param.stage.diff];
	Gfx_BlitTex(&menu.tex_story, diff_src, x - (diff_src->w >> 1), y - 9 + ((pad_state.press & (PAD_LEFT | PAD_RIGHT)) != 0));
}

static void Menu_DrawHealth(u8 i, s16 x, s16 y, boolean is_selected)
{
    //Icon Size
    u8 icon_size = 36;

    u8 col = (is_selected) ? 128 : 64;

    //Get src and dst
    RECT src = {
        (i % 6) * icon_size,
        (i / 6) * icon_size,
        icon_size,
        icon_size
    };
    RECT dst = {
        x,
        y,
        36,
        36
    };
    
    //Draw health icon
    Gfx_DrawTexCol(&menu.tex_icon, &src, &dst, col, col, col);
}

//Menu functions
void Menu_Load(MenuPage page)
{
	stage.stage_id = StageId_1_1;
	//Load menu assets
	IO_Data menu_arc = IO_Read("\\MENU\\MENU.ARC;1");
	Gfx_LoadTex(&menu.tex_ng,    Archive_Find(menu_arc, "ng.tim"),    0);
	Gfx_LoadTex(&menu.tex_story, Archive_Find(menu_arc, "story.tim"), 0);
	Gfx_LoadTex(&menu.tex_title, Archive_Find(menu_arc, "title.tim"), 0);
	Gfx_LoadTex(&menu.tex_icon,  Archive_Find(menu_arc, "icon.tim"),  0);
	Mem_Free(menu_arc);

	menu.bgs = IO_Read("\\MENU\\BACK.ARC;1");
    Gfx_LoadTex(&menu.tex_back, Archive_Find(menu.bgs, "back8.tim"), 0);
	
	FontData_Load(&menu.font_bold, Font_Bold);
	FontData_Load(&menu.font_arial, Font_Arial);
	
	menu.gf = Char_GF_New(FIXED_DEC(62,1), FIXED_DEC(-12,1));
	stage.camera.x = stage.camera.y = FIXED_DEC(0,1);
	stage.camera.bzoom = FIXED_UNIT;
	stage.gf_speed = 4;
	
	//Initialize menu state
	menu.select = menu.next_select = 0;
	
	switch (menu.page = menu.next_page = page)
	{
		case MenuPage_Opening:
			//Get funny message to use
			//Do this here so timing is less reliant on VSync
			menu.page_state.opening.funny_message = ((*((volatile u32*)0xBF801120)) >> 3) % COUNT_OF(funny_messages); //sysclk seeding
			break;
		default:
			break;
	}
	menu.page_swap = true;
	
	menu.trans_time = 0;
	Trans_Clear();
	
	stage.song_step = 0;

	// to load
	CdlFILE file;
    IO_FindFile(&file, "\\SOUNDS\\SCROLL.VAG;1");
    u32 *data = IO_ReadFile(&file);
    Sounds[0] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CONFIRM.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[1] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	IO_FindFile(&file, "\\SOUNDS\\CANCEL.VAG;1");
    data = IO_ReadFile(&file);
    Sounds[2] = Audio_LoadVAGData(data, file.size);
    Mem_Free(data);

	//Play menu music
	if (currentDisc == 1) {
		Audio_PlayXA_TrackDisc1(XA_GettinFreaky_Disc1, 0x40, 0, true);
		Audio_WaitPlayXA();
	}
	if (currentDisc == 2) {
		Audio_PlayXA_TrackDisc2(XA_GettinFreaky_Disc2, 0x40, 0, true);
		Audio_WaitPlayXA();
	}
	if (currentDisc == 3) {
		Audio_PlayXA_TrackDisc3(XA_GettinFreaky_Disc3, 0x40, 0, true);
		Audio_WaitPlayXA();
	}
	
	//Set background colour
	Gfx_SetClear(0, 0, 0);
}

void Menu_Unload(void)
{
	//Free title Girlfriend
	Character_Free(menu.gf);
	Mem_Free(menu.bgs);
}

void Menu_ToStage(StageId id, StageDiff diff, boolean story)
{
	menu.next_page = MenuPage_Stage;
	menu.page_param.stage.id = id;
	menu.page_param.stage.story = story;
	menu.page_param.stage.diff = diff;
	Trans_Start();
}

void Menu_Tick(void)
{
	//Clear per-frame flags
	stage.flag &= ~STAGE_FLAG_JUST_STEP;
	
	//Get song position
	u16 next_step = Audio_TellXA_Milli() / 147; //100 BPM
	if (next_step != stage.song_step)
	{
		if (next_step >= stage.song_step)
			stage.flag |= STAGE_FLAG_JUST_STEP;
		stage.song_step = next_step;
	}
	
	//Handle transition out
	if (Trans_Tick())
	{
		//Change to set next page
		menu.page_swap = true;
		menu.page = menu.next_page;
		menu.select = menu.next_select;
	}
	
	//Tick menu page
	MenuPage exec_page;
	switch (exec_page = menu.page)
	{
		case MenuPage_Opening:
		{
			u16 beat = stage.song_step >> 2;
			
			//Start title screen if opening ended
			if (beat >= 16)
			{
				menu.page = menu.next_page = MenuPage_Title;
				menu.page_swap = true;
				//Fallthrough
			}
			else
			{
				//Start title screen if start pressed
				if (pad_state.held & PAD_START)
					menu.page = menu.next_page = MenuPage_Title;
				
				//Draw different text depending on beat
				RECT src_ng = {0, 0, 128, 128};
				const char **funny_message = funny_messages[menu.page_state.opening.funny_message];
				
				switch (beat)
				{
					case 3:
						menu.font_bold.draw(&menu.font_bold, "PRESENTS", SCREEN_WIDTH2, SCREEN_HEIGHT2 + 32, FontAlign_Center);
				//Fallthrough
					case 2:
					case 1:
						menu.font_bold.draw(&menu.font_bold, "DREAMCASTNICK",   SCREEN_WIDTH2, SCREEN_HEIGHT2 - 32, FontAlign_Center);
						break;
					
					case 7:
						menu.font_bold.draw(&menu.font_bold, "NEWGROUNDS",    SCREEN_WIDTH2, SCREEN_HEIGHT2 - 32, FontAlign_Center);
						Gfx_BlitTex(&menu.tex_ng, &src_ng, (SCREEN_WIDTH - 128) >> 1, SCREEN_HEIGHT2 - 16);
				//Fallthrough
					case 6:
					case 5:
						menu.font_bold.draw(&menu.font_bold, "IN ASSOCIATION", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 64, FontAlign_Center);
						menu.font_bold.draw(&menu.font_bold, "WITH",           SCREEN_WIDTH2, SCREEN_HEIGHT2 - 48, FontAlign_Center);
						break;
					
					case 11:
						menu.font_bold.draw(&menu.font_bold, funny_message[1], SCREEN_WIDTH2, SCREEN_HEIGHT2, FontAlign_Center);
				//Fallthrough
					case 10:
					case 9:
						menu.font_bold.draw(&menu.font_bold, funny_message[0], SCREEN_WIDTH2, SCREEN_HEIGHT2 - 16, FontAlign_Center);
						break;
					
					case 15:
						menu.font_bold.draw(&menu.font_bold, "FUNKIN", SCREEN_WIDTH2, SCREEN_HEIGHT2 + 8, FontAlign_Center);
				//Fallthrough
					case 14:
						menu.font_bold.draw(&menu.font_bold, "NIGHT", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 8, FontAlign_Center);
				//Fallthrough
					case 13:
						menu.font_bold.draw(&menu.font_bold, "FRIDAY", SCREEN_WIDTH2, SCREEN_HEIGHT2 - 24, FontAlign_Center);
						break;
				}
				break;
			}
		}
	//Fallthrough
		case MenuPage_Title:
		{
			//Initialize page
			if (menu.page_swap)
			{
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(90,1);
			}
			
			//Draw white fade
			if (menu.page_state.title.fade > 0)
			{
				RECT flash = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
				u8 flash_col = menu.page_state.title.fade >> FIXED_SHIFT;
				Gfx_BlendRect(&flash, flash_col, flash_col, flash_col, 1);
				menu.page_state.title.fade -= FIXED_MUL(menu.page_state.title.fadespd, timer_dt);
			}
			
			//Go to main menu when start is pressed
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0) {
    			first_load = true;
				Trans_Start();
			}
			
			if ((pad_state.press & PAD_START) && menu.next_page == menu.page && Trans_Idle())
			{
				//play confirm sound
				Audio_PlaySound(Sounds[1], 0x3fff);
				menu.trans_time = FIXED_UNIT;
				menu.page_state.title.fade = FIXED_DEC(255,1);
				menu.page_state.title.fadespd = FIXED_DEC(300,1);
				menu.next_page = MenuPage_Main;
				menu.next_select = 0;
			}
			
			//Draw Friday Night Funkin' logo
			if ((stage.flag & STAGE_FLAG_JUST_STEP) && (stage.song_step & 0x3) == 0 && menu.page_state.title.logo_bump == 0)
				menu.page_state.title.logo_bump = (FIXED_DEC(7,1) / 24) - 1;
			
			static const fixed_t logo_scales[] = {
				FIXED_DEC(1,1),
				FIXED_DEC(101,100),
				FIXED_DEC(102,100),
				FIXED_DEC(103,100),
				FIXED_DEC(105,100),
				FIXED_DEC(110,100),
				FIXED_DEC(97,100),
			};
			fixed_t logo_scale = logo_scales[(menu.page_state.title.logo_bump * 24) >> FIXED_SHIFT];
			u32 x_rad = (logo_scale * (176 >> 1)) >> FIXED_SHIFT;
			u32 y_rad = (logo_scale * (112 >> 1)) >> FIXED_SHIFT;
			
			RECT logo_src = {0, 0, 176, 112};
			RECT logo_dst = {
				100 - x_rad + (SCREEN_WIDEADD2 >> 1),
				68 - y_rad,
				x_rad << 1,
				y_rad << 1
			};
			Gfx_DrawTex(&menu.tex_title, &logo_src, &logo_dst);
			
			if (menu.page_state.title.logo_bump > 0)
				if ((menu.page_state.title.logo_bump -= timer_dt) < 0)
					menu.page_state.title.logo_bump = 0;
			
			//Draw "Press Start to Begin"
			if (menu.next_page == menu.page)
			{
				//Blinking blue
				s16 press_lerp = (MUtil_Cos(animf_count << 3) + 0x100) >> 1;
				u8 press_r = 51 >> 1;
				u8 press_g = (58  + ((press_lerp * (255 - 58))  >> 8)) >> 1;
				u8 press_b = (206 + ((press_lerp * (255 - 206)) >> 8)) >> 1;
				
				RECT press_src = {0, 112, 256, 32};
				Gfx_BlitTexCol(&menu.tex_title, &press_src, (SCREEN_WIDTH - 256) / 2, SCREEN_HEIGHT - 48, press_r, press_g, press_b);
			}
			else
			{
				//Flash white
				RECT press_src = {0, (animf_count & 1) ? 144 : 112, 256, 32};
				Gfx_BlitTex(&menu.tex_title, &press_src, (SCREEN_WIDTH - 256) / 2, SCREEN_HEIGHT - 48);
			}
			
			//Draw Girlfriend
			menu.gf->tick(menu.gf);
			break;
		}
		case MenuPage_Main:
		{
			static const char *menu_options[] = {
				"PLAY",
				"CREDITS",
				"OPTIONS",
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = menu.select * FIXED_DEC(12,1);
				
			
			//Draw version identification
			menu.font_bold.draw(&menu.font_bold,
				"PSXGAPPLE BY DCNICK",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0) {
    			first_load = true;
				Trans_Start();
			}
			
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[1], 0x3fff);
					switch (menu.select)
					{
						case 0: //Story Mode
							menu.next_page = MenuPage_Story;
							break;
						case 1: //Credits
							menu.next_page = MenuPage_Credits;
							break;
						case 2: //Options
							menu.next_page = MenuPage_Options;
							break;
					}
					menu.next_select = 0;
					menu.trans_time = FIXED_UNIT;
				}
				
				//Return to title screen if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Title;
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(12,1);

			menu.scroll += (next_scroll - menu.scroll) >> 2;
			
			if (menu.next_page == menu.page || menu.next_page == MenuPage_Title)
			{
				//Draw all options
				for (u8 i = 0; i < COUNT_OF(menu_options); i++)
				{
					menu.font_bold.draw(&menu.font_bold,
						Menu_LowerIf(menu_options[i], menu.select != i),
						SCREEN_WIDTH2,
						SCREEN_HEIGHT2 + (i << 5) - 48 - (menu.scroll >> FIXED_SHIFT),
						FontAlign_Center
					);
				}
			}
			else if (animf_count & 2)
			{
				//Draw selected option
				menu.font_bold.draw(&menu.font_bold,
					menu_options[menu.select],
					SCREEN_WIDTH2,
					SCREEN_HEIGHT2 + (menu.select << 5) - 48 - (menu.scroll >> FIXED_SHIFT),
					FontAlign_Center
				);
			}
			
			//Draw background
			Menu_DrawBack(
				menu.next_page == menu.page || menu.next_page == MenuPage_Title,
				menu.scroll >> (FIXED_SHIFT + 3),
				253 >> 1, 231 >> 1, 113 >> 1,
				253 >> 1, 113 >> 1, 155 >> 1
			);
			break;
		}
		case MenuPage_Story:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				{StageId_1_1, "DISRUPTION", false},
				{StageId_1_2, "APPLECORE", false},
				{StageId_1_3, "DISABILITY", false},
				{StageId_1_4, "WIREFRAME", false},
				{StageId_1_5, "ALGEBRA", false},
				{StageId_1_6, "DEFORMATION", false},
				{StageId_1_7, "FEROCIOUS", false},
				{StageId_Temp, "EXTRAS", false},
			};
			
			//Handle option and selection
			if (menu.trans_time > 0 && (menu.trans_time -= timer_dt) <= 0) {
    			first_load = true;
				Trans_Start();
			}

			sprintf(menu.scoredisp, "PERSONAL BEST: %d",(
				stage.prefs.savescore[menu_options[menu.select].stage][menu.page_param.stage.diff]*10)
			);

			//Draw Score
			menu.font_arial.draw(&menu.font_arial,
				menu.scoredisp,
				0,
				7,
				FontAlign_Left
			);

			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.freeplay.back_r = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_g = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_b = FIXED_DEC(255,1);
			}
			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(SCREEN_WIDTH - 100, SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					switch (menu.select)
					{
						case 0:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 1:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 2:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 3:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 4:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 5:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 6:
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Stage;
							menu.page_param.stage.id = menu_options[menu.select].stage;
							menu.page_param.stage.story = true;
							menu.trans_time = FIXED_UNIT;
							Trans_Start();
						}
						break;
						case 7: //Extras
						{
							//play confirm sound
							Audio_PlaySound(Sounds[1], 0x3fff);
							menu.next_page = MenuPage_Freeplay;
							menu.next_select = 0;
							menu.trans_time = FIXED_UNIT;
						}
						break;
					}
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 0; //Freeplay
					Trans_Start();
				}
			}
	
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					SCREEN_WIDTH2,
					SCREEN_HEIGHT2 + y - 8,
					FontAlign_Center
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 231 >> 1, 113 >> 1,
				253 >> 1, 113 >> 1, 155 >> 1
			);
			break;
		}		
		case MenuPage_Freeplay:
		{
			static const struct
			{
				StageId stage;
				u32 col;
				const char *text;
				u8 icon;
				boolean difficulty;
			} menu_options[] = {
				{StageId_2_1, 0xFF00A013, "SUGAR RUSH", 0, false},
				{StageId_2_2, 0xFF00A013, "GIFT CARD", 1, false},
				{StageId_2_3, 0xFF00A013, "THE BIG DINGLE", 2, false},
				{StageId_2_4, 0xFF00A013, "DALE", 3, false},
				{StageId_2_5, 0xFF00A013, "ORIGIN", 4, false},
				{StageId_2_6, 0xFFB21B62, "APPRENTICE", 5, false},
				{StageId_2_7, 0xFF00A013, "RESUMED", 6, false},
				{StageId_2_8, 0xFF00A013, "READY LOUD", 7, false},
				{StageId_2_9, 0xFF00A013, "BOOKWORM", 8, false},
				{StageId_2_10, 0xFF4059E1, "CUBEROOT", 9, false},
				{StageId_2_11, 0xFF4059E1, "ALTERNATE", 10, false},
				{StageId_2_12, 0xFF4059E1, "UNHINGED", 11, false},
				{StageId_2_13, 0xFFFDFD3F, "THEFT", 12, false},
				{StageId_2_14, 0xFF0000FF, "TOO SHINY", 13, false},
				{StageId_3_1, 0xFF00A013, "TANTALUM", 14, false},
				{StageId_3_2, 0xFF00A013, "STRAWBERRY", 15, false},
				{StageId_3_3, 0xFF00A013, "KEYBOARD", 16, false},
				{StageId_3_4, 0xFF00A013, "SILLIER", 17, false},
				{StageId_4_1, 0xFF4059E1, "WHEELS", 18, false},
				{StageId_4_2, 0xFFB21B62, "POOPERS", 19, false},
				{StageId_4_3, 0xFF000000, "THE BOOPADOOP SONG", 20, false},
				{StageId_5_1, 0xFF4059E1, "OG", 21, false},
				{StageId_5_2, 0xFF4059E1, "SICK TRICKS", 22, false},
				{StageId_5_3, 0xFF00A013, "GALACTIC", 0, false},
				{StageId_6_1, 0xFF000000, "CELL", 1, false},
				{StageId_6_2, 0xFF00A013, "TICKING", 2, false},
				{StageId_6_3, 0xFFFFFFFF, "RECOVERED PROJECT", 3, false},
				{StageId_6_4, 0xFF00FFFF, "DAVE X BAMBI SHIPPING CUTE", 4, false},
				{StageId_7_1, 0xFF00A013, "SLICES", 5, false},
				{StageId_8_1, 0xFF00A013, "SUNSHINE", 6, false},
				{StageId_8_2, 0xFF4059E1, "LEFT UNCHECKED", 7, false},
				{StageId_8_3, 0xFF4059E1, "THUNDERSTORM", 8, false},
				{StageId_8_4, 0xFF0000FF, "COLLISION", 9, false},
			};

			sprintf(menu.scoredisp, "PERSONAL BEST: %d",(
				stage.prefs.savescore[menu_options[menu.select].stage][menu.page_param.stage.diff]*10)
			);

			menu.font_arial.draw(&menu.font_arial,
				menu.scoredisp,
				150,
				SCREEN_HEIGHT / 2 - 75,
				FontAlign_Left
			);

			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
				menu.page_state.freeplay.back_r = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_g = FIXED_DEC(255,1);
				menu.page_state.freeplay.back_b = FIXED_DEC(255,1);
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"FREEPLAY",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(SCREEN_WIDTH - 100, SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Select option if cross is pressed
				if (pad_state.press & (PAD_START | PAD_CROSS))
				{
					//play confirm sound
					Audio_PlaySound(Sounds[1], 0x3fff);
					menu.next_page = MenuPage_Stage;
					menu.page_param.stage.id = menu_options[menu.select].stage;
					menu.page_param.stage.story = false;
					Trans_Start();
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 0; //Play
					Trans_Start();
				}
			}
	
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw Icon
				Menu_DrawHealth(menu_options[i].icon, strlen(menu_options[i].text) * 14 + 36 + (y >> 2), SCREEN_HEIGHT2 + y - 20, menu.select == i);
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			fixed_t tgt_r = (fixed_t)((menu_options[menu.select].col >> 16) & 0xFF) << FIXED_SHIFT;
			fixed_t tgt_g = (fixed_t)((menu_options[menu.select].col >>  8) & 0xFF) << FIXED_SHIFT;
			fixed_t tgt_b = (fixed_t)((menu_options[menu.select].col >>  0) & 0xFF) << FIXED_SHIFT;
			
			menu.page_state.freeplay.back_r += (tgt_r - menu.page_state.freeplay.back_r) >> 4;
			menu.page_state.freeplay.back_g += (tgt_g - menu.page_state.freeplay.back_g) >> 4;
			menu.page_state.freeplay.back_b += (tgt_b - menu.page_state.freeplay.back_b) >> 4;
			
			Menu_DrawBack(
				true,
				8,
				menu.page_state.freeplay.back_r >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_g >> (FIXED_SHIFT + 1),
				menu.page_state.freeplay.back_b >> (FIXED_SHIFT + 1),
				0, 0, 0
			);
			break;
		}
		case MenuPage_Credits:
		{
			static const struct
			{
				StageId stage;
				const char *text;
				boolean difficulty;
			} menu_options[] = {
				{StageId_1_1, "	PORT DEVS", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "DREAMCASTNICK", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "	PLAYTESTERS", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "ANYONE WHO DOWNLOADED", false},
				{StageId_1_1, "	THIS ROM", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "	COOL PEOPLE", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "UNSTOPABLE", false},
				{StageId_1_1, "IGORSOU", false},
				{StageId_1_1, "BILIOUSDATA", false},
				{StageId_1_1, "NINTENDOBRO", false},
				{StageId_1_1, "LORD SCOUT", false},
				{StageId_1_1, "MR RUMBLEROSES", false},
				{StageId_1_1, "CKDEV", false},
				{StageId_1_1, "PICHUMANTEN", false},
				{StageId_1_1, "JOHN PAUL", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "	CREDITS CODE", false},
				{StageId_1_1,    "", false},
				{StageId_1_1, "ZERIBEN", false},
			};
			    
			//Initialize page
			if (menu.page_swap)
			{
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
				menu.page_param.stage.diff = StageDiff_Normal;
			}
			
			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"CREDITS",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Draw difficulty selector
			if (menu_options[menu.select].difficulty)
				Menu_DifficultySelector(SCREEN_WIDTH - 100, SCREEN_HEIGHT2 - 48);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 1; //Credits
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(menu_options[i].text, menu.select != i),
					48 + (y >> 2),
					SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				197 >> 1, 240 >> 1, 95 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_Options:
		{
			static const char *gamemode_strs[] = {"NORMAL", "TWO PLAYER"};
			static const struct
			{
				enum
				{
					OptType_Boolean,
					OptType_Enum,
				} type;
				const char *text;
				void *value;
				union
				{
					struct
					{
						int a;
					} spec_boolean;
					struct
					{
						s32 max;
						const char **strs;
					} spec_enum;
				} spec;
			} menu_options[] = {
				{OptType_Enum,    "GAMEMODE", &stage.prefs.mode, {.spec_enum = {COUNT_OF(gamemode_strs), gamemode_strs}}},
				{OptType_Boolean, "INTERPOLATION", &stage.prefs.expsync, {.spec_boolean = {0}}},
				{OptType_Boolean, "GHOST TAP", &stage.prefs.ghost, {.spec_boolean = {0}}},
				{OptType_Boolean, "PRACTICE MODE", &stage.prefs.practice, {.spec_boolean = {0}}},
				{OptType_Boolean, "DOWNSCROLL", &stage.prefs.downscroll, {.spec_boolean = {0}}},
				{OptType_Boolean, "BOTPLAY", &stage.prefs.botplay, {.spec_boolean = {0}}},
				{OptType_Boolean, "SHOW SONG TIME", &stage.prefs.songtimer, {.spec_boolean = {0}}},
				{OptType_Boolean, "FLASHING LIGHTS", &stage.prefs.flash, {.spec_boolean = {0}}},
				{OptType_Boolean, "DEBUG MODE", &stage.prefs.debug, {.spec_boolean = {0}}},
			};
			
			//Initialize page
			if (menu.page_swap)
				menu.scroll = COUNT_OF(menu_options) * FIXED_DEC(24 + SCREEN_HEIGHT2,1);
			
			RECT save_src = {0, 121, 55, 7};
			RECT save_dst = {SCREEN_WIDTH / 2 + 30 - (121 / 2), SCREEN_HEIGHT - 30, 53 * 2, 7 * 2};
			Gfx_DrawTex(&menu.tex_story, &save_src, &save_dst);

			//Draw page label
			menu.font_bold.draw(&menu.font_bold,
				"OPTIONS",
				16,
				SCREEN_HEIGHT - 32,
				FontAlign_Left
			);
			
			//Handle option and selection
			if (menu.next_page == menu.page && Trans_Idle())
			{
				//Change option
				if (pad_state.press & PAD_UP)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select > 0)
						menu.select--;
					else
						menu.select = COUNT_OF(menu_options) - 1;
				}
				if (pad_state.press & PAD_DOWN)
				{
					//play scroll sound
                    Audio_PlaySound(Sounds[0], 0x3fff);
					if (menu.select < COUNT_OF(menu_options) - 1)
						menu.select++;
					else
						menu.select = 0;
				}
				
				//Handle option changing
				switch (menu_options[menu.select].type)
				{
					case OptType_Boolean:
						if (pad_state.press & (PAD_CROSS | PAD_LEFT | PAD_RIGHT))
							*((boolean*)menu_options[menu.select].value) ^= 1;
						break;
					case OptType_Enum:
						if (pad_state.press & PAD_LEFT)
							if (--*((s32*)menu_options[menu.select].value) < 0)
								*((s32*)menu_options[menu.select].value) = menu_options[menu.select].spec.spec_enum.max - 1;
						if (pad_state.press & PAD_RIGHT)
							if (++*((s32*)menu_options[menu.select].value) >= menu_options[menu.select].spec.spec_enum.max)
								*((s32*)menu_options[menu.select].value) = 0;
						break;
				}
				
				//save progress
				if (pad_state.press & PAD_SELECT)
					WriteSave();

				//Return to main menu if circle is pressed
				if (pad_state.press & PAD_CIRCLE)
				{
					//play cancel sound
					Audio_PlaySound(Sounds[2], 0x3fff);
					menu.next_page = MenuPage_Main;
					menu.next_select = 2; //Options
					Trans_Start();
				}
			}
			
			//Draw options
			s32 next_scroll = menu.select * FIXED_DEC(24,1);
			menu.scroll += (next_scroll - menu.scroll) >> 4;
			
			for (u8 i = 0; i < COUNT_OF(menu_options); i++)
			{
				//Get position on screen
				s32 y = (i * 24) - 8 - (menu.scroll >> FIXED_SHIFT);
				if (y <= -SCREEN_HEIGHT2 - 8)
					continue;
				if (y >= SCREEN_HEIGHT2 + 8)
					break;
				
				//Draw text
				char text[0x80];
				switch (menu_options[i].type)
				{
					case OptType_Boolean:
						sprintf(text, "%s %s", menu_options[i].text, *((boolean*)menu_options[i].value) ? "ON" : "OFF");
						break;
					case OptType_Enum:
						sprintf(text, "%s %s", menu_options[i].text, menu_options[i].spec.spec_enum.strs[*((s32*)menu_options[i].value)]);
						break;
				}
				menu.font_bold.draw(&menu.font_bold,
					Menu_LowerIf(text, menu.select != i),
					48 + (y >> 2),
					SCREEN_HEIGHT2 + y - 8,
					FontAlign_Left
				);
			}
			
			//Draw background
			Menu_DrawBack(
				true,
				8,
				253 >> 1, 113 >> 1, 155 >> 1,
				0, 0, 0
			);
			break;
		}
		case MenuPage_Stage:
		{
			char discIndicator[32];

			if ((currentDisc == 1) && (menu.page_param.stage.id >= StageId_1_1 && menu.page_param.stage.id <= StageId_1_7))
			{
				if (sprintf(discIndicator, "\\DISC1.ID;1") == 0)
				{
					DisplayMessage("Disc 1 is already inserted. Press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
				}
			}
			else if ((currentDisc == 2) && (menu.page_param.stage.id >= StageId_2_1 && menu.page_param.stage.id <= StageId_3_4))
			{
				if (sprintf(discIndicator, "\\DISC2.ID;1") == 0)
				{
					DisplayMessage("Disc 2 is already inserted. Press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
				}
			}
			else if ((currentDisc == 3) && (menu.page_param.stage.id >= StageId_4_1 && menu.page_param.stage.id <= StageId_8_4))
			{
				if (sprintf(discIndicator, "\\DISC3.ID;1") == 0)
				{
					DisplayMessage("Disc 3 is already inserted. Press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
				}
			}

			if ((currentDisc == 1) && (menu.page_param.stage.id >= StageId_2_1 && menu.page_param.stage.id <= StageId_3_4))
			{
				if (sprintf(discIndicator, "\\DISC2.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 2 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}
			else if ((currentDisc == 1) && (menu.page_param.stage.id >= StageId_4_1 && menu.page_param.stage.id <= StageId_8_4))
			{
				if (sprintf(discIndicator, "\\DISC3.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 3 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}

			if ((currentDisc == 2) && (menu.page_param.stage.id >= StageId_1_1 && menu.page_param.stage.id <= StageId_1_7))
			{
				if (sprintf(discIndicator, "\\DISC1.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 1 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}
			else if ((currentDisc == 2) && (menu.page_param.stage.id >= StageId_4_1 && menu.page_param.stage.id <= StageId_8_4))
			{
				if (sprintf(discIndicator, "\\DISC3.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 3 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}

			if ((currentDisc == 3) && (menu.page_param.stage.id >= StageId_1_1 && menu.page_param.stage.id <= StageId_1_7))
			{
				if (sprintf(discIndicator, "\\DISC1.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 1 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}
			else if ((currentDisc == 3) && (menu.page_param.stage.id >= StageId_2_1 && menu.page_param.stage.id <= StageId_3_4))
			{
				if (sprintf(discIndicator, "\\DISC2.ID;1") != 0)
				{
					DisplayMessage("Please insert disc 2 and press X to continue.");
					while (!(pad_state.press & PAD_CROSS))
					{
						Pad_Update();
					}
					HandleDiscSwap();
				}
			}

			Menu_Unload();
			LoadScr_Start();
			Stage_Load(menu.page_param.stage.id, menu.page_param.stage.diff, menu.page_param.stage.story);
			gameloop = GameLoop_Stage;
			LoadScr_End();
			break;
		}
		default:
			break;
	}
	
	//Clear page swap flag
	menu.page_swap = menu.page != exec_page;
}