/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

//XA enumerations
typedef enum
{
	XA_Menu1,   //MENU.XA
	XA_1, //1.XA
	XA_2, //2.XA
	XA_3, //3.XA
	XA_4, //4.XA

	XA_Max1,
} XA_File1;

typedef enum
{
	XA_Menu2,	//MENU.XA
	XA_5, //5.XA
	XA_6, //6.XA
	XA_7, //7.XA
	XA_8, //8.XA
	XA_9, //9.XA
	XA_10, //10.XA
	XA_11, //11.XA
	XA_12, //12.XA
	XA_13, //13.XA

	XA_Max2,
} XA_File2;

typedef enum
{
	XA_Menu3,	//MENU.XA
	XA_14, //14.XA
	XA_15, //15.XA
	XA_16, //16.XA
	XA_17, //17.XA
	XA_18, //18.XA
	XA_19, //19.XA
	XA_20, //20.XA
	XA_21, //21.XA
	XA_22, //22.XA

	XA_Max3,
} XA_File3;

typedef enum
{
	//MENU.XA
	XA_GettinFreaky_Disc1, //Gettin' Freaky
	XA_GameOver_Disc1,     //Game Over
	//1.XA
	XA_Disruption, //Disruption
	XA_Applecore, //Applecore
	//2.XA
	XA_Disability, //Disability
	XA_Wireframe, //Wireframe
	//3.XA
	XA_Algebra, //Algebra
	XA_Deformation, //Deformation
	//4.XA
	XA_Ferocious, //Ferocious
	
	XA_TrackMax1,
} XA_Track1;

typedef enum
{
	//MENU.XA
	XA_GettinFreaky_Disc2, //Gettin' Freaky
	XA_GameOver_Disc2,     //Game Over
	//5.XA
	XA_SugarRush, //Sugar Rush
	XA_GiftCard, //Gift Card
	//6.XA
	XA_TheBigDingle, //The Big Dingle
	XA_Dale, //Dale
	//7.XA
	XA_Origin, //Origin
	XA_Apprentice, //Apprentice
	//8.XA
	XA_Resumed, //Resumed
	XA_ReadyLoud, //Ready Loud
	//9.XA
	XA_Bookworm, //Bookworm
	XA_Cuberoot, //Cuberoot
	//10.XA
	XA_Alternate, //Alternate
	XA_Unhinged, //Unhinged
	//11.XA
	XA_Theft, //Theft
	XA_TooShiny, //Too Shiny
	//12.XA
	XA_Tantalum, //Tantalum
	XA_Strawberry, //Strawberry
	//13.XA
	XA_Keyboard, //Keyboard
	XA_Sillier, //Sillier
	
	XA_TrackMax2,
} XA_Track2;

typedef enum
{
	//MENU.XA
	XA_GettinFreaky_Disc3, //Gettin' Freaky
	XA_GameOver_Disc3,     //Game Over
	//14.XA
	XA_Wheels, //Wheels
	XA_Poopers, //Poopers
	//15.XA
	XA_TheBoopadoopSong, //The Boopadoop Song
	XA_OG, //OG
	//16.XA
	XA_SickTricks, //Sick Tricks
	//17.XA
	XA_Galactic, //Galactic
	//18.XA
	XA_Cell, //Cell
	XA_Ticking, //Ticking
	//19.XA
	XA_RecoveredProject, //Recovered Project
	XA_DaveXBambiShippingCute, //Dave X Bambi Shipping Cute
	//20.XA
	XA_Slices, //Slices
	//21.XA
	XA_Sunshine, //Sunshine
	XA_LeftUnchecked, //Left Unchecked
	//22.XA
	XA_Thunderstorm, //Thunderstorm
	XA_Collision, //Collision
	
	XA_TrackMax3,
} XA_Track3;

//Audio functions
u32 Audio_GetLengthDisc1(XA_Track1 lengthtrack);
u32 Audio_GetLengthDisc2(XA_Track2 lengthtrack);
u32 Audio_GetLengthDisc3(XA_Track3 lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_Reset(void);
void Audio_PlayXA_TrackDisc1(XA_Track1 track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_TrackDisc1(XA_Track1 track);
void Audio_PlayXA_TrackDisc2(XA_Track2 track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_TrackDisc2(XA_Track2 track);
void Audio_PlayXA_TrackDisc3(XA_Track3 track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_TrackDisc3(XA_Track3 track);
void Audio_PauseXA(void);
void Audio_ResumeXA(void);
void Audio_StopXA(void);
void Audio_ChannelXA(u8 channel);
s32 Audio_TellXA_Sector(void);
s32 Audio_TellXA_Milli(void);
boolean Audio_PlayingXA(void);
void Audio_WaitPlayXA(void);
void Audio_ProcessXA(void);
void findFreeChannel(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
u32 VAG_IsPlaying(u32 channel);
void Audio_ClearAlloc(void);

#endif
