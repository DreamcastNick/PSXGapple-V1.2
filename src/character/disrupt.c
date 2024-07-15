/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "disrupt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"
#include "../mutil.h"
#include "../random.h"

//Disrupt character structure
enum
{
	Disrupt_ArcMain_Idle0,
	Disrupt_ArcMain_Idle1,
	Disrupt_ArcMain_Idle2,
	Disrupt_ArcMain_Idle3,
	Disrupt_ArcMain_Idle4,
	Disrupt_ArcMain_Left0,
	Disrupt_ArcMain_Left1,
	Disrupt_ArcMain_Left2,
	Disrupt_ArcMain_Left3,
	Disrupt_ArcMain_Down0,
	Disrupt_ArcMain_Down1,
	Disrupt_ArcMain_Down2,
	Disrupt_ArcMain_Down3,
	Disrupt_ArcMain_Up0,
	Disrupt_ArcMain_Up1,
	Disrupt_ArcMain_Up2,
	Disrupt_ArcMain_Up3,
	Disrupt_ArcMain_Right0,
	Disrupt_ArcMain_Right1,
	Disrupt_ArcMain_Right2,
	Disrupt_ArcMain_Right3,
	
	Disrupt_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Disrupt_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Disrupt;

s32 float_opponenty;

static const u16 char_disrupt_icons[2][4] = {
	{72,0,36,36},
	{108,0,36,36}
};

//Disrupt character definitions
static const CharFrame char_disrupt_frame[] = {
	{Disrupt_ArcMain_Idle0, {  0,  0,140,188}, {176,184}}, //0 idle 1
	{Disrupt_ArcMain_Idle1, {  0,  0,120,184}, {158,181}}, //1 idle 2
	{Disrupt_ArcMain_Idle1, {120,  0,124,184}, {160,176}}, //2 idle 3
	{Disrupt_ArcMain_Idle2, {  0,  0,128,172}, {162,168}}, //3 idle 4
	{Disrupt_ArcMain_Idle3, {  0,  0,132,172}, {166,168}}, //4 idle 5
	{Disrupt_ArcMain_Idle4, {  0,  0,136,172}, {167,167}}, //5 idle 6

	{Disrupt_ArcMain_Left0, {  0,  0,160,180}, {218,198}}, //6 left 1
	{Disrupt_ArcMain_Left1, {  0,  0,160,183}, {220,198}}, //7 left 2
	{Disrupt_ArcMain_Left2, {  0,  0,160,200}, {220,196}}, //8 left 3
	{Disrupt_ArcMain_Left3, {  0,  0,148,200}, {202,190}}, //9 left 4

	{Disrupt_ArcMain_Down0, {  0,  0,156,188}, {179,149}}, //10 down 1
	{Disrupt_ArcMain_Down1, {  0,  0,160,188}, {181,150}}, //11 down 2
	{Disrupt_ArcMain_Down2, {  0,  0,160,183}, {187,154}}, //12 down 3
	{Disrupt_ArcMain_Down3, {  0,  0,160,188}, {186,161}}, //13 down 4

	{Disrupt_ArcMain_Up0, {  0,  0,136,192}, {166,210}}, //14 up 1
	{Disrupt_ArcMain_Up1, {  0,  0,140,192}, {167,206}}, //15 up 2
	{Disrupt_ArcMain_Up2, {  0,  0,156,192}, {178,202}}, //16 up 3
	{Disrupt_ArcMain_Up3, {  0,  0,152,192}, {177,191}}, //17 up 4

	{Disrupt_ArcMain_Right0, {  0,  0,200,183}, {190,172}}, //18 right 1
	{Disrupt_ArcMain_Right1, {  0,  0,200,183}, {189,172}}, //19 right 2
	{Disrupt_ArcMain_Right2, {  0,  0,188,184}, {180,173}}, //20 right 3
	{Disrupt_ArcMain_Right3, {  0,  0,160,188}, {172,181}}, //21 right 4
};

static const Animation char_disrupt_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  0,  1,  2,  3,  4,  5,  4,  3,  2,  1,  0,  0, ASCR_BACK, 0}},			//CharAnim_Idle
	{2, (const u8[]){ 6,  7,  8,  9, ASCR_BACK, 0}},         										//CharAnim_Left
	{2, (const u8[]){ 6,  7,  8,  9, ASCR_BACK, 0}},         										//CharAnim_LeftAlt
	{2, (const u8[]){10, 11, 12, 13, ASCR_BACK, 0}},         										//CharAnim_Down
	{2, (const u8[]){10, 11, 12, 13, ASCR_BACK, 0}},         										//CharAnim_DownAlt
	{2, (const u8[]){14, 15, 16, 17, ASCR_BACK, 0}},         										//CharAnim_Up
	{2, (const u8[]){14, 15, 16, 17, ASCR_BACK, 0}},         										//CharAnim_UpAlt
	{2, (const u8[]){18, 19, 20, 21, ASCR_BACK, 0}},         										//CharAnim_Right
	{2, (const u8[]){18, 19, 20, 21, ASCR_BACK, 0}},         										//CharAnim_RightAlt
};

//Disrupt character functions
void Char_Disrupt_SetFrame(void *user, u8 frame)
{
	Char_Disrupt *this = (Char_Disrupt*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_disrupt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Disrupt_Tick(Character *character) {
    Char_Disrupt *this = (Char_Disrupt*)character;
	
	float_opponenty += 1;
	character->y += MUtil_Sin(float_opponenty) * 2;

    // Perform idle dance
    if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
	{
        Character_PerformIdle(character);
		stage.noteshake = false;
	}

    // Animate and draw
    Animatable_Animate(&character->animatable, (void*)this, Char_Disrupt_SetFrame);
    Character_Draw(character, &this->tex, &char_disrupt_frame[this->frame]);
}

void Char_Disrupt_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Disrupt_Free(Character *character)
{
	Char_Disrupt *this = (Char_Disrupt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Disrupt_New(fixed_t x, fixed_t y)
{
	//Allocate disrupt object
	Char_Disrupt *this = Mem_Alloc(sizeof(Char_Disrupt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Disrupt_New] Failed to allocate disrupt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Disrupt_Tick;
	this->character.set_anim = Char_Disrupt_SetAnim;
	this->character.free = Char_Disrupt_Free;
	
	Animatable_Init(&this->character.animatable, char_disrupt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	memcpy(this->character.health_i, char_disrupt_icons, sizeof(char_disrupt_icons));

	//health bar color
	this->character.health_bar = 0xFFFF0000;
	
	this->character.focus_x = FIXED_DEC(-100,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(100,100);
	
	this->character.size = FIXED_DEC(100,100);
	
	//Load art
	this->arc_main = IO_Read("\\OCHAR\\DISRUPT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim",
		"idle1.tim",
		"idle2.tim",
		"idle3.tim",
		"idle4.tim",
		"left0.tim",
		"left1.tim",
		"left2.tim",
		"left3.tim",
		"down0.tim",
		"down1.tim",
		"down2.tim",
		"down3.tim",
		"up0.tim",
		"up1.tim",
		"up2.tim",
		"up3.tim",
		"right0.tim",
		"right1.tim",
		"right2.tim",
		"right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}