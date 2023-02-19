/***********************************************************/
// 다음은 사용자 함수를 호출하는 루틴임 지우지 말것!
/***********************************************************/

#include "device_driver.h"
#include "2440addr.h"
#include <stdlib.h>
#include <math.h>

#define printf 	Uart_Printf

void User_Main(void);

void Main(void)
{
	MMU_Init();
	Led_Init();
	Key_Poll_Init();
	Uart_Init(115200);
	Timer0_Init();
	Timer3_Buzzer_Init();
	Timer4_Init();
	Lcd_Graphic_Init();
	Lcd_Clr_Screen(0xf800);

	Uart_Printf("Welcome GBOX World!\n");
	User_Main();
}

/*===== << 여기부터 사용자 프로그램을 작성함 >> =====*/
#include "./Image/title.h"
#include "./Image/howto0.h"
#include "./Image/howto1.h"
#include "./Image/howto2.h"
#include "./Image/start.h"
#include "./Image/lingo.h"
#include "./Image/ready.h"
#include "./Image/skull.h"
#include "./Image/skull2.h"
#include "./Image/skull3.h"
#include "./Image/skull4.h"
#include "./Image/people.h"
#include "./Image/warning.h"
#include "./Image/skeletons.h"
#include "./Image/duolingo.h"
#include "./Image/duolingo2.h"
#include "./Image/stronger.h"
#include "./Image/enemy_0101.h"
#include "./Image/enemy_0102.h"
#include "./Image/enemy_0103.h"
#include "./Image/enemy_0104.h"
#include "./Image/enemy_0105.h"
#include "./Image/enemy_go.h"
#include "./Image/ruined.h"
#include "./Image/thankyou.h"

/* Name of Images */
const unsigned short * Disp[] = {title, howto0, howto1, howto2, start, lingo, ready, skull, skull2, skull3, skull4,\
		people, warning, skeletons, duolingo, duolingo2, stronger, enemy_0101, enemy_0102, enemy_0103, enemy_0104, enemy_0105,\
		enemy_go, ruined, thankyou};

/* 5:5:5:I Color Definition */
#define BLACK	0x0000
#define WHITE	0xfffe
#define BLUE	0x003e
#define GREEN	0x07c0
#define RED		0xf800

/* Functions */
void Opening();
void Song();
void longSong();
void SoundEffect_01();
void SoundEffect_02();
void SoundEffect_03();
void SoundEffect_04();
void SoundEffect_05();
void SoundEffect_06();
void SoundEffect_07();
void SoundEffect_08();
void SoundEffect_09();
void SoundEffect_10();
void SoundEffect_11();
void Game();
int RTC_Get_Time();
void Move_Lingo();
void Falling_Skull(int level);
void Draw_Object(int level);
int RTC_Get_Time();
void Present(int nextlevel);
void Use_bomb(int level);
void Use_slower();
void Set_enemy(int level);
void Game_Init(int level);
void WinEnding();
void LoseEnding();
void Game();

/* Variables */
int voiceOn = 1; // if 1: sound on(default), 0: sound off
int lifes = 9;
int bomb = 0;
int isBomb; // to prevent using many bombs as pushing button long
int slower = 0;
int slow; // to count time of slowing item.
int level = 1;
const double PI = 3.1415;

/* Main */
void User_Main()
{
		Opening();
		level = 1;
		Game();
}

/* Opening, Start, Help, Volume Option */
void Opening()
{
	First:
	Lcd_Draw_BMP(0, 0, title);
	Song();
	Lcd_Printf(110,160,GREEN,BLACK,1,1,"Press 5 to START!");
	Lcd_Printf(110,180,GREEN,BLACK,1,1,"Press 6 to HELP!");
	if (voiceOn == 0)
	{
		Lcd_Printf(110,200,GREEN,BLACK,1,1,"Press 7 to Volume ON");
	}
	if (voiceOn == 1)
	{
		Lcd_Printf(110,200,GREEN,BLACK,1,1,"Press 7 to Volume OFF");
	}

	if (Key_Wait_Key_Pressed()==5) // To start game
	{
		Key_Wait_Key_Released();
		SoundEffect_01();
		Lcd_Draw_BMP(0, 0, start);
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_01();
		Lcd_Clr_Screen(WHITE);
		Lcd_Draw_BMP(0, 0, duolingo);
		Lcd_Printf(110,100,GREEN,BLACK,2,2,"변신!");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_01();
		Game();
	}
	if (Key_Wait_Key_Pressed()==6) // Help (How to play)
	{
		Key_Wait_Key_Released();
		SoundEffect_01();
		Lcd_Draw_BMP(0, 0, howto0);
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_01();
		Lcd_Draw_BMP(0, 0, howto1);
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_01();
		Lcd_Draw_BMP(0, 0, howto2);
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_01();
		goto First;
	}
	if (Key_Wait_Key_Pressed()==7) // Option: use volume or not
	{
		Key_Wait_Key_Released();
		Lcd_Clr_Screen(WHITE);
		if (voiceOn == 1)
		{
			Lcd_Printf(20,100,WHITE,BLACK,1,1,"소리를 끄면");
			Lcd_Printf(20,120,WHITE,BLACK,1,1,"해골군 방어 성공 여부에 관계없이");
			Lcd_Printf(20,140,WHITE,BLACK,1,1,"잠시 멈추는 효과가 사라집니다.");
			voiceOn = 0;
			Key_Wait_Key_Pressed();
			Key_Wait_Key_Released();
			goto First;
		}
		if (voiceOn == 0)
		{
			Lcd_Printf(20,100,WHITE,BLACK,1,1,"소리를 켜면");
			Lcd_Printf(20,120,WHITE,BLACK,1,1,"해골군의 공격과 충돌시");
			Lcd_Printf(20,140,WHITE,BLACK,1,1,"잠시 멈추는 효과가 생깁니다.");
			voiceOn = 1;
			Key_Wait_Key_Pressed();
			Key_Wait_Key_Released();
			goto First;
		}
	}
}

/* Music and Sound */
enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16, N8, N4, N2, N1};
void Song() // main song
{
	if (voiceOn == 0) return;
	const int song[][2] = {{G1,N4},{E1,N4},{D1,N8},{E1,N16},{C1,N16},{C1,N4},{D1,N8},{E1,N8},{F1,N8},{F1_,N8},{G1,N4},{G1,N4}, {0xff, 0xff}};
	int i = 0;
	while(song[i][0] != 0xff)
		{
			Timer3_Buzzer_Beep(song[i][0], song[i][1]);
			i++;
		}
}
void longSong() // win ending
{
	if (voiceOn == 0) return;
	const int song[][2] = {{G1,N4},{E1,N4},{D1,N8},{E1,N16},{C1,N16},{C1,N4},{D1,N8},{E1,N8},{F1,N8},{F1_,N8},{G1,N4},{G1,N4},\
			{A1,N4},{G1,N4},{F1,N8},{G1,N16},{E1,N16},{E1,N4},{D1,N8},{E1,N8},{D1,N8},{E1,N8},{D1,N8},{E1,N16},{D1,N16},{D1,N4},\
			{G1,N4},{E1,N4},{D1,N8},{E1,N16},{C1,N16},{C1,N4},{D1,N8},{E1,N8},{F1,N8},{F1_,N8},{G1,N4},{G1,N4},\
			{A1,N4},{G1,N4},{F1,N8},{G1,N16},{G1,N16},{E1,N4},{G1,N8},{F1,N8},{E1,N8},{D1,N8},{C1,N4},{C2,N16},{0xff, 0xff}};
	int i = 0;
	while(song[i][0] != 0xff)
		{
			Timer3_Buzzer_Beep(song[i][0], song[i][1]);
			i++;
		}
}
void SoundEffect_01() // when button clicked
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(C1,N16);
	Timer3_Buzzer_Beep(D1,N16);
	Timer3_Buzzer_Beep(E1,N16);
}
void SoundEffect_02() // When enemy fires. (Not Used for playing environment)
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(F1,N16);
}
void SoundEffect_03() // When defend successfully
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(C1,N16);
	Timer3_Buzzer_Beep(G1,N16);
	Timer3_Buzzer_Beep(C2,N16);
}
void SoundEffect_04() // When skull crashes on ground
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(D1,N16);
}
void SoundEffect_05() // Before/After each round
{
	if (voiceOn == 0) return;
	const int song[][2] = {{G1,N4},{E1,N4},{D1,N8},{E1,N16},{C1,N16},{C1,N4},{0xff, 0xff}};
	int i = 0;
	while(song[i][0] != 0xff)
	{
		Timer3_Buzzer_Beep(song[i][0], song[i][1]);
		i++;
	}
}
void SoundEffect_06() // Story: Warning
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(C2,N8);
	Timer3_Buzzer_Beep(E2,N8);
	Timer3_Buzzer_Beep(C2,N8);
	Timer3_Buzzer_Beep(E2,N8);
}
void SoundEffect_07() // Story: Power of Lingo
{
	if (voiceOn == 0) return;
	const int song[][2] = {{C2,N4},{C2,N8},{B1,N16},{B1,N1},{A1,N4},{A1,N8},{A1,N16},{B1,N1},{0xff, 0xff}};
	int i = 0;
	while(song[i][0] != 0xff)
	{
		Timer3_Buzzer_Beep(song[i][0], song[i][1]);
		i++;
	}
}
void SoundEffect_08() // Story: LOSE
{
	if (voiceOn == 0) return;
	const int song[][2] = {{G1,N4},{C2,N4},{D2,N4},{D2_,N1},{D2,N1},{C2,N4},{B1,N4},{C2,N4},{0xff, 0xff}};
	int i = 0;
	while(song[i][0] != 0xff)
	{
		Timer3_Buzzer_Beep(song[i][0], song[i][1]);
		i++;
	}
}
void SoundEffect_09() // When use item bomb
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(G1,N16);
	Timer3_Buzzer_Beep(F1,N16);
	Timer3_Buzzer_Beep(E1,N16);
	Timer3_Buzzer_Beep(D1,N16);
	Timer3_Buzzer_Beep(C1,N16);
}
void SoundEffect_10() // When use item slowery
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(G1,N4);
	Timer3_Buzzer_Beep(F1,N4);
	Timer3_Buzzer_Beep(E1,N4);
	Timer3_Buzzer_Beep(D1,N4);
	Timer3_Buzzer_Beep(C1,N4);
}
void SoundEffect_11() // When get present
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(C2,N16);
	Timer3_Buzzer_Beep(C2,N8);
}
void SoundEffect_12() // Game over
{
	if (voiceOn == 0) return;
	Timer3_Buzzer_Beep(C1,N8);
	Timer3_Buzzer_Beep(C1,N4);
}

/* Objects: Lingo, Skulls */
struct Object
{
	int timer;
	int move_flag;
	int pos[2];
	int pos_init[2];
	int pos_back[2];
	int size[2];
	int speed_step;
	int move_step;
	int upper;
};
struct Object Lingo = {0,1,{0,105},{0,105},{0,105}, {30, 30}, 3, 8};
struct Object Skull_01 = {0,0,{250,120},{250,120},{250,120}, {20, 20}, 11, 2};
struct Object Skull_02 = {0,0,{250,180},{250,180},{250,180}, {20, 20}, 9, 2};
struct Object Skull_03 = {0,0,{250,70},{250,70},{250,70}, {20, 20}, 10, 2};
struct Object Skull_04 = {0,0,{250,140},{250,140},{250,140}, {20, 20}, 11, 2, 150};

/* Functions setting location of objects */
void Move_Lingo()
{
	if(Timer0_Check_Expired())
	{
		Lingo.timer++;

		if(Lingo.timer >= Lingo.speed_step)
		{
			Lingo.timer = 0;
			Lingo.pos_back[1] = Lingo.pos[1];
			if (Key_Get_Pressed()==1)
			{
				Lingo.pos[1] = Lingo.pos[1] - Lingo.move_step;
				Lingo.move_flag = 1;
			}
			if (Key_Get_Pressed()==3)
			{
				Lingo.pos[1] = Lingo.pos[1] + Lingo.move_step;
				Lingo.move_flag = 1;
			}
			if (Key_Get_Pressed()==2)
			{
				Lingo.pos[0] = Lingo.pos[0] - Lingo.move_step;
				Lingo.move_flag = 1;
			}
			if (Key_Get_Pressed()==4)
			{
				Lingo.pos[0] = Lingo.pos[0] + Lingo.move_step;
				Lingo.move_flag = 1;
			}
			if (Key_Get_Pressed()==5)
			{
				Use_bomb(level);
			}
			if (Key_Get_Pressed()==6)
			{
				Use_slower();
			}
		}
	}
}
void Falling_Skull(int level)
{
	Skull_01.timer++;
	Skull_02.timer++;
	Skull_03.timer++;
	Skull_04.timer++;
	if(Skull_01.timer >= Skull_01.speed_step)
	{
		Skull_01.timer = 0;
		Skull_01.pos_back[0] = Skull_01.pos[0];
		Skull_01.pos_back[1] = Skull_01.pos[1];
		Skull_01.pos[0] = Skull_01.pos[0] - Skull_01.move_step;
		Skull_01.move_flag = 1;
	}
	if(Skull_03.timer >= Skull_03.speed_step)
	{
		Skull_03.timer = 0;
		Skull_03.pos_back[0] = Skull_03.pos[0];
		Skull_03.pos_back[1] = Skull_03.pos[1];
		Skull_03.pos[0] = Skull_03.pos[0] - Skull_03.move_step;
		Skull_03.move_flag = 1;
	}
	if (level >= 2) // two more skull as level 2 starts
	{
		if(Skull_02.timer >= Skull_02.speed_step)
		{
			Skull_02.timer = 0;
			Skull_02.pos_back[0] = Skull_02.pos[0];
			Skull_02.pos_back[1] = Skull_02.pos[1];
			Skull_02.pos[0] = Skull_02.pos[0] - Skull_02.move_step;
			Skull_02.move_flag = 1;
		}
		if(Skull_04.timer >= Skull_04.speed_step)
		{
			Skull_04.timer = 0;
			Skull_04.pos_back[0] = Skull_04.pos[0];
			Skull_04.pos_back[1] = Skull_04.pos[1];
			Skull_04.pos[0] = Skull_04.pos[0] - Skull_04.move_step;
			Skull_04.pos[1] = sin(2*Skull_04.pos[0]*PI/180)*60+Skull_04.upper;
			Skull_04.move_flag = 1;
		}
	}
}

/* Function drawing objects with their location information */
void Draw_Object(int level)
{
	int Skull_next_01[] = {60, 90, 120, 150, 180, 210}; // After falling down, candidates of next location of skull01
	int Skull_next_02[] = {30, 50, 80, 110, 140, 170}; // candidates of next location of skull02
	int Skull_next_03[] = {120, 60, 210, 180, 150, 90}; // candidates of next location of skull03
	static int Skull_next_04[] = {60, 80, 100, 120, 150, 180}; // candidates of next location of skull03
	/* limitation of Lingo's movement */
	if(Lingo.pos[0] > 0)
	{
		Lingo.pos[0] = 0;
	}
	if(Lingo.pos[0] < 0)
		{
			Lingo.pos[0] = 0;
		}
	if(Lingo.pos[1] > 240-Lingo.size[1])
		{
			Lingo.pos[1] = 240-Lingo.size[1];
		}
	if(Lingo.pos[1] < 20)
		{
			Lingo.pos[1] = 20;
		}
	/* remove objects and draw new objects with new location information */
	if(Lingo.move_flag != 0)
	{
		Lcd_Draw_Bar(Lingo.pos_back[0], Lingo.pos_back[1], Lingo.pos_back[0]+Lingo.size[0], Lingo.pos_back[1]+Lingo.size[1], WHITE);
		Lcd_Draw_BMP(Lingo.pos[0], Lingo.pos[1], lingo);
		Lingo.move_flag = 0;
	}
	if(Skull_01.move_flag != 0)
	{
		Lcd_Draw_Bar(Skull_01.pos_back[0], Skull_01.pos_back[1], Skull_01.pos_back[0]+Skull_01.size[0], Skull_01.pos_back[1]+Skull_01.size[1], WHITE);
		Lcd_Draw_BMP(Skull_01.pos[0], Skull_01.pos[1], skull);
		Skull_01.move_flag = 0;
	}
	if(Skull_03.move_flag != 0)
	{
		Lcd_Draw_Bar(Skull_03.pos_back[0], Skull_03.pos_back[1], Skull_03.pos_back[0]+Skull_03.size[0], Skull_03.pos_back[1]+Skull_03.size[1], WHITE);
		Lcd_Draw_BMP(Skull_03.pos[0], Skull_03.pos[1], skull3);
		Skull_03.move_flag = 0;
	}
	if (level >= 2)
	{
		if(Skull_02.move_flag != 0)
		{
			Lcd_Draw_Bar(Skull_02.pos_back[0], Skull_02.pos_back[1], Skull_02.pos_back[0]+Skull_02.size[0], Skull_02.pos_back[1]+Skull_02.size[1], WHITE);
			Lcd_Draw_BMP(Skull_02.pos[0], Skull_02.pos[1], skull2);
			Skull_02.move_flag = 0;
		}
		if(Skull_04.move_flag != 0)
		{
			Lcd_Draw_Bar(Skull_04.pos_back[0], Skull_04.pos_back[1], Skull_04.pos_back[0]+Skull_04.size[0], Skull_04.pos_back[1]+Skull_04.size[1], WHITE);
			Lcd_Draw_BMP(Skull_04.pos[0], Skull_04.pos[1], skull4);
			Skull_04.move_flag = 0;
		}
	}
	/* When Skull crashes on Lingo */
	if(Skull_01.pos[0] <= Lingo.pos[0]+Lingo.size[0] && (Skull_01.pos[1]+Skull_01.size[1] > Lingo.pos[1] && Lingo.pos[1]+Lingo.size[1] > Skull_01.pos[1]))
	{
		Lcd_Draw_Bar(Skull_01.pos[0], Skull_01.pos[1], Skull_01.pos[0]+Skull_01.size[0], Skull_01.pos[1]+Skull_01.size[1], WHITE);
		SoundEffect_03();
		if (level <= 2)
		{
			Skull_01.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_01.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_01.pos[0] = 150;
		}
		Skull_01.pos[1] = Skull_next_01[RTC_Get_Time()%6];
		Skull_01.timer = -300;
	}
	if(Skull_02.pos[0] <= Lingo.pos[0]+Lingo.size[0] && (Skull_02.pos[1]+Skull_02.size[1] > Lingo.pos[1] && Lingo.pos[1]+Lingo.size[1] > Skull_02.pos[1]))
	{
		Lcd_Draw_Bar(Skull_02.pos[0], Skull_02.pos[1], Skull_02.pos[0]+Skull_02.size[0], Skull_02.pos[1]+Skull_02.size[1], WHITE);
		SoundEffect_03();
		if (level <= 2)
		{
			Skull_02.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_02.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_02.pos[0] = 150;
		}
		Skull_02.pos[1] = Skull_next_02[RTC_Get_Time()%6];
		Skull_02.timer = -500;
	}
	if(Skull_03.pos[0] <= Lingo.pos[0]+Lingo.size[0] && (Skull_03.pos[1]+Skull_03.size[1] > Lingo.pos[1] && Lingo.pos[1]+Lingo.size[1] > Skull_03.pos[1]))
	{
		Lcd_Draw_Bar(Skull_03.pos[0], Skull_03.pos[1], Skull_03.pos[0]+Skull_03.size[0], Skull_03.pos[1]+Skull_03.size[1], WHITE);
		SoundEffect_03();
		if (level <= 2)
		{
			Skull_03.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_03.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_03.pos[0] = 150;
		}
		Skull_03.pos[1] = Skull_next_03[RTC_Get_Time()%6];
		Skull_03.timer = -150;
	}
	if(Skull_04.pos[0] <= Lingo.pos[0]+Lingo.size[0] && (Skull_04.pos[1]+Skull_04.size[1] > Lingo.pos[1] && Lingo.pos[1]+Lingo.size[1] > Skull_04.pos[1]))
	{
		Lcd_Draw_Bar(Skull_04.pos[0], Skull_04.pos[1], Skull_04.pos[0]+Skull_04.size[0], Skull_04.pos[1]+Skull_04.size[1], WHITE);
		SoundEffect_03();
		if (level <= 2)
		{
			Skull_04.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_04.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_04.pos[0] = 150;
		}
		Skull_04.upper = Skull_next_04[RTC_Get_Time()%6];
		Skull_04.timer = -2000;
	}
	/* When Skull crashes on floor */
	if(Skull_01.pos[0] == 0)
	{
		Lcd_Draw_Bar(Skull_01.pos[0], Skull_01.pos[1], Skull_01.pos[0]+Skull_01.size[0], Skull_01.pos[1]+Skull_01.size[1], WHITE);
		SoundEffect_04();
		if (level <= 2)
		{
			Skull_01.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_01.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_01.pos[0] = 150;
		}
		Skull_01.pos[1] = Skull_next_01[RTC_Get_Time()%6];
		Skull_01.timer = -300;
		lifes--;
	}
	if(Skull_02.pos[0] == 0)
	{
		Lcd_Draw_Bar(Skull_02.pos[0], Skull_02.pos[1], Skull_02.pos[0]+Skull_02.size[0], Skull_02.pos[1]+Skull_02.size[1], WHITE);
		SoundEffect_04();
		if (level <= 2)
		{
			Skull_02.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_02.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_02.pos[0] = 150;
		}
		Skull_02.pos[1] = Skull_next_02[RTC_Get_Time()%6];
		Skull_02.timer = -500;
		lifes--;
	}
	if(Skull_03.pos[0] == 0)
	{
		Lcd_Draw_Bar(Skull_03.pos[0], Skull_03.pos[1], Skull_03.pos[0]+Skull_03.size[0], Skull_03.pos[1]+Skull_03.size[1], WHITE);
		SoundEffect_04();
		if (level <= 2)
		{
			Skull_03.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_03.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_03.pos[0] = 150;
		}
		Skull_03.pos[1] = Skull_next_03[RTC_Get_Time()%6];
		Skull_03.timer = -200;
		lifes--;
	}
	if(Skull_04.pos[0] == 0)
	{
		Lcd_Draw_Bar(Skull_04.pos[0], Skull_04.pos[1], Skull_04.pos[0]+Skull_04.size[0], Skull_04.pos[1]+Skull_04.size[1], WHITE);
		SoundEffect_04();
		if (level <= 2)
		{
			Skull_04.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_04.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_04.pos[0] = 150;
		}
		Skull_04.upper = Skull_next_04[RTC_Get_Time()%6];
		Skull_04.timer = -2000;
		lifes--;
		}
	/* print status of life, bomb, slower items */
	Lcd_Printf(2,2,BLACK,WHITE,1,1,"life:%d", lifes);
	Lcd_Printf(80,2,BLACK,WHITE,1,1,"bombs:%d", bomb);
	Lcd_Printf(160,2,BLACK,WHITE,1,1,"slowers:%d", slower);
}

/* to use random number */
typedef struct
{
	int day;
	int year;
	int mon;
	int date;
	int hour;
	int min;
	int sec;
}RTC_TIME;
int RTC_Get_Time()
{
	RTC_TIME a;
	a.hour = (int)rBCDHOUR;
	a.min = (int)rBCDMIN;
	a.sec = (int)rBCDSEC;
	return  ((a.hour >> 4) & 0xf) * 10 + (a.hour & 0xf) * 360 + ((a.min >> 4) & 0xf) * 10 + (a.min & 0xf) * 60 + ((a.sec >> 4) & 0xf) * 10 + (a.sec & 0xf);
}

/* After each round, people gives present to Lingo */
/* and also this function presents story of each stage */
void Present(int level)
{
	/* Get Present */
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(10,100,BLACK,WHITE,1,1,"적의 공격을 성공적으로 막아냈습니다.");
	SoundEffect_05();
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Printf(10,130,BLACK,WHITE,1,1,"마을 사람들이 랜덤박스를 가져왔습니다.");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Draw_BMP(0, 0, people);
	int result = RTC_Get_Time()%5;
	SoundEffect_11();
	if (result==0)
	{
		slower += 2;
		Lcd_Printf(100,100,BLACK,WHITE,1,1,"느리개 2개 획득!");
	}
	else if (result==1)
	{
		slower += 2;
		Lcd_Printf(100,100,BLACK,WHITE,1,1,"느리개 2개 획득!");
	}
	else if (result==2)
	{
		bomb += 1;
		Lcd_Printf(100,100,BLACK,WHITE,1,1,"폭탄 1개 획득!");
	}
	else if (result==3)
	{
		slower += 1;
		Lcd_Printf(100,100,BLACK,WHITE,1,1,"느리개 1개 획득!");
	}
	else if (result==4)
	{
		bomb += 2;
		Lcd_Printf(100,100,BLACK,WHITE,1,1,"폭탄 2개 획득!");
	}
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(100,100,BLACK,RED,2,2,"LEVEL %d!",level);
	SoundEffect_05();
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();

	/* story of each stage */
	if (level ==2)
	{
		Lcd_Clr_Screen(WHITE);
		SoundEffect_06();
		Lcd_Printf(100,100,BLACK,RED,2,2,"WARNING!!");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Clr_Screen(WHITE);
		Lcd_Draw_BMP(0, 0, warning);
		Lcd_Printf(165,100,BLACK,WHITE,1,1,"더 많은");
		Lcd_Printf(165,120,BLACK,WHITE,1,1,"해골이 온다고 해요!");
		Lcd_Printf(165,140,BLACK,WHITE,1,1,"조심하세요!");
		Key_Wait_Key_Pressed();
	}
	if (level==3)
	{
		Lcd_Clr_Screen(WHITE);
		SoundEffect_06();
		Lcd_Printf(100,100,BLACK,RED,2,2,"WARNING!!");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Draw_BMP(0, 0, skeletons);
		Lcd_Printf(150,100,BLACK,WHITE,1,1,"계속 뛰어다니더니");
		Lcd_Printf(150,120,BLACK,WHITE,1,1,"지친 모양이군....");
		Lcd_Printf(150,140,BLACK,WHITE,2,2,"돌격하자!!");
		Key_Wait_Key_Pressed();
	}
	if (level==4)
	{
		Lcd_Clr_Screen(WHITE);
		SoundEffect_06();
		Lcd_Draw_BMP(0, 0, duolingo2);
		Lcd_Printf(10,20,BLACK,WHITE,1,1,"이래서는 마을을");
		Lcd_Printf(10,40,BLACK,WHITE,1,1,"지킬수가 없어");
		Lcd_Printf(10,60,BLACK,WHITE,2,2,"변신!!");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		SoundEffect_07();
		Lcd_Draw_BMP(0, 5, stronger);
		Key_Wait_Key_Pressed();
	}
}

/* Function to use items */
void Use_bomb(int level)
{
	if (bomb>0 && isBomb==0) // check if Lingo has bomb & prevent double using at one click
	{
		/* remove skulls after using bomb */
		SoundEffect_09();
		Lcd_Draw_Bar(Skull_01.pos[0], Skull_01.pos[1], Skull_01.pos[0]+Skull_01.size[0], Skull_01.pos[1]+Skull_01.size[1], WHITE);
		Lcd_Draw_Bar(Skull_02.pos[0], Skull_02.pos[1], Skull_02.pos[0]+Skull_02.size[0], Skull_02.pos[1]+Skull_02.size[1], WHITE);
		Lcd_Draw_Bar(Skull_03.pos[0], Skull_03.pos[1], Skull_03.pos[0]+Skull_03.size[0], Skull_03.pos[1]+Skull_03.size[1], WHITE);
		Lcd_Draw_Bar(Skull_04.pos[0], Skull_04.pos[1], Skull_04.pos[0]+Skull_04.size[0], Skull_04.pos[1]+Skull_04.size[1], WHITE);
		/* delay next skulls */
		Skull_01.timer = -500;
		Skull_02.timer = -300;
		Skull_03.timer = -700;
		Skull_04.timer = -2500;
		/* set location of next skulls */
		if (level <= 2)
		{
			Skull_01.pos[0] = 250;
			Skull_02.pos[0] = 250;
			Skull_03.pos[0] = 250;
			Skull_04.pos[0] = 250;
		}
		if (level == 3)
		{
			Skull_01.pos[0] = 200;
			Skull_02.pos[0] = 200;
			Skull_03.pos[0] = 200;
			Skull_04.pos[0] = 200;
		}
		if (level == 4)
		{
			Skull_01.pos[0] = 150;
			Skull_02.pos[0] = 150;
			Skull_03.pos[0] = 150;
			Skull_04.pos[0] = 150;
		}
		bomb--;
		isBomb = 1;
	}
	return;
}
void Use_slower()
{
	if (slower>0 && Skull_01.speed_step!=20) // check if Lingo has item & item used before has effect yet
	{
		SoundEffect_10();
		Skull_01.speed_step = 20;
		Skull_02.speed_step = 20;
		Skull_03.speed_step = 20;
		Skull_04.speed_step = 20;
		slow = 1; // check now there is slow down situation
		slower--;
	}
	return;
}

/* Set enemy's location according to input level */
void Set_enemy(int level)
{
	Lcd_Clr_Screen(WHITE);
	Lcd_Draw_BMP(270, 0, enemy_0101);
	Lcd_Draw_BMP(280, 60, enemy_0102);
	Lcd_Draw_BMP(280, 110, enemy_0103);
	Lcd_Draw_BMP(275, 160, enemy_0104);
	Lcd_Draw_BMP(280, 200, enemy_0105);

	if (level >= 3)
	{
		Lcd_Draw_BMP(220, 0, enemy_0101);
		Lcd_Draw_BMP(230, 60, enemy_0102);
		Lcd_Draw_BMP(230, 110, enemy_0103);
		Lcd_Draw_BMP(225, 160, enemy_0104);
		Lcd_Draw_BMP(230, 200, enemy_0105);
		Skull_01.pos[0] = 170;
		Skull_02.pos[0] = 170;
		Skull_03.pos[0] = 170;
		Skull_04.pos[0] = 170;
	}
	if (level == 4)
	{
		Lcd_Draw_BMP(170, 0, enemy_0101);
		Lcd_Draw_BMP(180, 60, enemy_0102);
		Lcd_Draw_BMP(180, 110, enemy_0103);
		Lcd_Draw_BMP(175, 160, enemy_0104);
		Lcd_Draw_BMP(180, 200, enemy_0105);
		Skull_01.pos[0] = 150;
		Skull_02.pos[0] = 150;
		Skull_03.pos[0] = 150;
		Skull_04.pos[0] = 150;
	}
	Lingo.pos[0] = 0; Lingo.pos[1] = 120;
}

/* Initialization each stage */
void Game_Init(int level)
{
	Set_enemy(level);
	isBomb = 0;
	slow = 0;
	Skull_01.speed_step = 11;
	Skull_02.speed_step = 9;
	Skull_03.speed_step = 10;
	Skull_04.speed_step = 11;
	Timer0_Repeat(20);
	Draw_Object(level);
}

/* Ending: if Lingo defenses all the stages with at least one life */
void WinEnding()
{
	Lcd_Clr_Screen(WHITE);
	SoundEffect_05();
	Lcd_Printf(10,100,BLACK,WHITE,1,1,"적의 공격을 성공적으로 막아냈습니다.");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Printf(10,130,BLACK,WHITE,1,1,"해골군대가 퇴각합니다!");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Draw_BMP(0, 0, enemy_go);
	Lcd_Printf(20,130,BLACK,WHITE,1,1,"어차피 저 근육새 못 이기는데");
	Lcd_Printf(20,150,BLACK,WHITE,1,1,"집에 보내줘요..");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Draw_BMP(0, 0, people);
	Lcd_Printf(20,100,BLACK,WHITE,1,1,"링고,당신의 노력이 우리를 구했어요.");
	Lcd_Printf(20,120,BLACK,WHITE,1,1,"싸우지 않고도 적을 물리치다니..");
	Lcd_Printf(20,140,BLACK,WHITE,1,1,"덕분에 마을에 피해도 전혀 없었어요.");
	longSong();
	Lcd_Printf(20,180,BLACK,WHITE,2,2,"고마워요!!");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Printf(100,200,BLACK,RED,2,2,"YOU WIN!");
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Lcd_Draw_BMP(0, 0, thankyou);
	Key_Wait_Key_Pressed();
	Key_Wait_Key_Released();
	Opening();
}

/* Ending: if Lingo fails to defense all the stages with at least one life */
void LoseEnding()
{
		SoundEffect_12();
		Lcd_Printf(100,100,BLACK,RED,2,2,"YOU LOSED!");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Draw_BMP(0, 0, ruined);
		Lcd_Printf(20,100,BLACK,WHITE,1,1,"으하하하 이 마을은 이제 우리꺼야!");
		SoundEffect_08();
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Draw_BMP(0, 0, people);
		Lcd_Printf(20,100,BLACK,WHITE,1,1,"링고.. 자네 괜찮나?");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Printf(20,120,BLACK,WHITE,1,1,"우린 괜찮네.. 너무 수고 많았어..");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Printf(20,140,BLACK,WHITE,1,1,"비록 우리는 침략당했지만..");
		Lcd_Printf(20,160,BLACK,WHITE,1,1,"발명 할아범이 타임머신을 만들었다네..");
		Lcd_Printf(20,180,BLACK,WHITE,1,1,"준비가 된다면 언제든 과거로 돌아가서");
		Lcd_Printf(20,200,BLACK,WHITE,1,1,"우리를 지켜주게..믿고있겠네");
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Lcd_Draw_BMP(0, 0, thankyou);
		Key_Wait_Key_Pressed();
		Key_Wait_Key_Released();
		Opening();
}

/* Gathering all the parts of game */
void Game()
{
	Game_Init(level);
	Lcd_Draw_BMP(Lingo.pos[0], Lingo.pos[1], lingo);
	int i=0;
	while (i<8000)
	{
		Move_Lingo();
		Falling_Skull(level);
		Draw_Object(level);
		if (lifes==0)
		{
			goto No_Life;
		}
		if (slow>0) // After using slower, write time it effects
		{
			slow++;
		}
		if (slow==1500) // After 1500 iterations(about 1/5 of a stage), stop effect of slower item
		{
			Skull_01.speed_step = 11;
			Skull_02.speed_step = 9;
			Skull_03.speed_step = 10;
			Skull_04.speed_step = 11;
			slow=0;
		}
		if (isBomb>0)
		{
			isBomb++;
		}
		if (isBomb==300)
		{
			isBomb = 0;
		}
		i++;
	}
	level++;
	if (level==5)
	{
		WinEnding();
		return;
	}
	Present(level);
	Game();
	No_Life: LoseEnding();
	return;
}
