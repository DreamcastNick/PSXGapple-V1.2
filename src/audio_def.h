#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File1 file;
	u32 length;
} XA_TrackDef1;

typedef struct
{
	XA_File2 file;
	u32 length;
} XA_TrackDef2;

typedef struct
{
	XA_File2 file;
	u32 length;
} XA_TrackDef3;

static const XA_TrackDef1 xa_tracks_disc1[] = {
	//MENU.XA
	{XA_Menu1, XA_LENGTH(8000)}, //XA_GettinFreaky_Disc1
	{XA_Menu1, XA_LENGTH(3800)},  //XA_GameOver_Disc1
	//1.XA
	{XA_1, XA_LENGTH(14600)}, //XA_Disruption
	{XA_1, XA_LENGTH(30900)}, //XA_Applecore
	//2.XA
	{XA_2, XA_LENGTH(20200)}, //XA_Disability
	{XA_2, XA_LENGTH(20200)}, //XA_Wireframe
	//3.XA
	{XA_3, XA_LENGTH(66100)}, //XA_Algebra
	{XA_3, XA_LENGTH(38900)}, //XA_Deformation
	//4.XA
	{XA_4, XA_LENGTH(92200)}, //XA_Ferocious
};

static const XA_TrackDef2 xa_tracks_disc2[] = {
	//MENU.XA
	{XA_Menu2, XA_LENGTH(8000)}, //XA_GettinFreaky_Disc2
	{XA_Menu2, XA_LENGTH(3800)},  //XA_GameOver_Disc2
	//5.XA
	{XA_5, XA_LENGTH(13500)}, //XA_SugarRush
	{XA_5, XA_LENGTH(14400)}, //XA_GiftCard
	//6.XA
	{XA_6, XA_LENGTH(21400)}, //XA_TheBigDingle
	{XA_6, XA_LENGTH(16400)}, //XA_Dale
	//7.XA
	{XA_7, XA_LENGTH(11400)}, //XA_Origin
	{XA_7, XA_LENGTH(17300)}, //XA_Apprentice
	//8.XA
	{XA_8, XA_LENGTH(23500)}, //XA_Resumed
	{XA_8, XA_LENGTH(13100)}, //XA_ReadyLoud
	//9.XA
	{XA_9, XA_LENGTH(13100)}, //XA_Bookworm
	{XA_9, XA_LENGTH(15200)}, //XA_Cuberoot
	//10.XA
	{XA_10, XA_LENGTH(13800)}, //XA_Alternate
	{XA_10, XA_LENGTH(12700)}, //XA_Unhinged
	//11.XA
	{XA_11, XA_LENGTH(17200)}, //XA_Theft
	{XA_11, XA_LENGTH(12900)}, //XA_TooShiny
	//12.XA
	{XA_12, XA_LENGTH(20900)}, //XA_Tantalum
	{XA_12, XA_LENGTH(11200)}, //XA_Strawberry
	//13.XA
	{XA_13, XA_LENGTH(11500)}, //XA_Keyboard
	{XA_13, XA_LENGTH(9500)}, //XA_Sillier
};

static const XA_TrackDef3 xa_tracks_disc3[] = {
	//MENU.XA
	{XA_Menu3, XA_LENGTH(8000)}, //XA_GettinFreaky_Disc3
	{XA_Menu3, XA_LENGTH(3800)},  //XA_GameOver_Disc3
	//14.XA
	{XA_14, XA_LENGTH(14800)}, //XA_Wheels
	{XA_14, XA_LENGTH(10100)}, //XA_Poopers
	//15.XA
	{XA_15, XA_LENGTH(1400)}, //XA_TheBoopadoopSong
	//16.XA
	{XA_16, XA_LENGTH(23500)}, //XA_OG
	{XA_16, XA_LENGTH(10200)}, //XA_SickTricks
	//17.XA
	{XA_17, XA_LENGTH(11600)}, //XA_Galactic
	//18.XA
	{XA_18, XA_LENGTH(19800)}, //XA_Cell
	{XA_18, XA_LENGTH(17400)}, //XA_Ticking
	//19.XA
	{XA_19, XA_LENGTH(23900)}, //XA_RecoveredProject
	{XA_19, XA_LENGTH(8900)}, //XA_DaveXBambiShippingCute
	//20.XA
	{XA_20, XA_LENGTH(13300)}, //XA_Slices
	//21.XA
	{XA_21, XA_LENGTH(18300)}, //XA_Sunshine
	{XA_21, XA_LENGTH(17300)}, //XA_LeftUnchecked
	//22.XA
	{XA_22, XA_LENGTH(15800)}, //XA_Thunderstorm
	{XA_22, XA_LENGTH(15700)}, //XA_Collision
};

static const char *xa_paths_disc1[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu1
	"\\MUSIC\\1.XA;1", //XA_1
	"\\MUSIC\\2.XA;1", //XA_2
	"\\MUSIC\\3.XA;1", //XA_3
	"\\MUSIC\\4.XA;1", //XA_4
	NULL,
};

static const char *xa_paths_disc2[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu2
	"\\MUSIC\\5.XA;1", //XA_5
	"\\MUSIC\\6.XA;1", //XA_6
	"\\MUSIC\\7.XA;1", //XA_7
	"\\MUSIC\\8.XA;1", //XA_8
	"\\MUSIC\\9.XA;1", //XA_9
	"\\MUSIC\\10.XA;1", //XA_10
	"\\MUSIC\\11.XA;1", //XA_11
	"\\MUSIC\\12.XA;1", //XA_12
	"\\MUSIC\\13.XA;1", //XA_13
	NULL,
};

static const char *xa_paths_disc3[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu3
	"\\MUSIC\\14.XA;1", //XA_14
	"\\MUSIC\\15.XA;1", //XA_15
	"\\MUSIC\\16.XA;1", //XA_16
	"\\MUSIC\\17.XA;1", //XA_17
	"\\MUSIC\\18.XA;1", //XA_18
	"\\MUSIC\\19.XA;1", //XA_19
	"\\MUSIC\\20.XA;1", //XA_20
	"\\MUSIC\\21.XA;1", //XA_21
	"\\MUSIC\\22.XA;1", //XA_22
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3_1;

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3_2;

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3_3;

static const XA_Mp3_1 xa_mp3s1[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky_Disc1
	{"gameover", false}, //XA_GameOver_Disc1
	//1.XA
	{"disruption", true}, //XA_Disruption
	{"applecore", true}, //XA_Applecore
	//2.XA
	{"disability", true}, //XA_Disability
	{"wireframe", true}, //XA_Wireframe
	//3.XA
	{"algebra", true}, //XA_Algebra
	{"deformation", true}, //XA_Deformation
	//4.XA
	{"ferocious", true}, //XA_Ferocious

	{NULL, false}
};

static const XA_Mp3_2 xa_mp3s2[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky_Disc2
	{"gameover", false}, //XA_GameOver_Disc2
	//5.XA
	{"sugar", true}, //XA_SugarRush
	{"giftcard", true}, //XA_GiftCard
	//6.XA
	{"the-big-dingle", true}, //XA_TheBigDingle
	{"dale", true}, //XA_Dale
	//7.XA
	{"origin", true}, //XA_Origin
	{"apprentice", true}, //XA_Apprentice
	//8.XA
	{"resumed", true}, //XA_Resumed
	{"ready-loud", true}, //XA_ReadyLoud
	//9.XA
	{"bookworm", true}, //XA_Bookworm
	{"cuberoot", true}, //XA_Cuberoot
	//10.XA
	{"alternate", true}, //XA_Alternate
	{"unhinged", true}, //XA_Unhinged
	//11.XA
	{"theft", true}, //XA_Theft
	{"too-shiny", true}, //XA_TooShiny
	//12.XA
	{"tantalum", true}, //XA_Tantalum
	{"strawberry", true}, //XA_Strawberry
	//13.XA
	{"keyboard", true}, //XA_Keyboard
	{"sillier", true}, //XA_Sillier

	{NULL, false}
};

static const XA_Mp3_3 xa_mp3s3[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky_Disc3
	{"gameover", false}, //XA_GameOver_Disc3
	//14.XA
	{"wheels", true}, //XA_Wheels
	{"poopers", true}, //XA_Poopers
	//15.XA
	{"the-boopadoop-song", true}, //XA_TheBoopadoopSong
	//16.XA
	{"og", true}, //XA_OG
	{"sick-tricks", true}, //XA_SickTricks
	//17.XA
	{"galactic", true}, //XA_Galactic
	//18.XA
	{"cell", true}, //XA_Cell
	{"ticking", true}, //XA_Ticking
	//19.XA
	{"recovered-project", true}, //XA_RecoveredProject
	{"dave-x-bambi-shipping-cute", true}, //XA_DaveXBambiShippingCute
	//20.XA
	{"slices", true}, //XA_Slices
	//21.XA
	{"sunshine", true}, //XA_Sunshine
	{"left-unchecked", true}, //XA_LeftUnchecked
	//22.XA
	{"thunderstorm", true}, //XA_Thunderstorm
	{"collision", true}, //XA_Collision

	{NULL, false}
};