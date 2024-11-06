/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"


char backbuf[MAX_Y][MAX_X] = { 0 };
char frontbuf[MAX_Y][MAX_X] = { 0 };

void project(char src[N_LAYER][MAX_Y][MAX_X], char dest[MAX_Y][MAX_X]);
void state_project(char src[STATE_Y][STATE_X], char dest[MAX_Y][MAX_X]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAX_Y][MAX_X]);
void display_state(char state[STATE_Y][STATE_X]);
void display_cursor(CURSOR cursor);

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION state_pos = { 1, 61 };
const POSITION message_pos = { 19, 0 };
const POSITION command_pos = { 19, 61 };


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_Y][MAP_X],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAX_Y][MAX_X], char dest[MAX_Y][MAX_X]) {
	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void state_project(char src[STATE_Y][STATE_X], char dest[MAX_Y][MAX_X]) {
	for (int i = 0; i < MAP_Y; i++) {
		for (int j = 0; j < MAP_X; j++) {
			if (src[i][i] >= 0) {
				dest[i][i] = src[i][j];
			}
		}
	}
}

void display_map(char map[N_LAYER][MAX_Y][MAX_X]) {
	project(map, backbuf);

	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				//아트레이디스 본진
				if ((i == 15 || i == 16) && (j == 1 || j == 2)) {
					map[0][i][j] = 'B';
					mapData[i][j] = ATREIDES_BASE;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 0, 9);
					colorData[i][j] = 9;
					textData[i][j] = 0;
				}
				//하코넨 본진
				else if ((i == 1 || i == 2) && (j == 57 || j == 58)) {
					map[0][i][j] = 'B';
					mapData[i][j] = HARKONNEN_BASE;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 0, 12);
					colorData[i][j] = 12;
					textData[i][j] = 0;
				}
				//아트레이디스 장판
				else if (((i == 15 || i == 16) && (j == 3 || j == 4))) {
					map[0][i][j] = 'P';
					mapData[i][j] = ATREIDES_PLATE;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 0);
					colorData[i][j] = 0;
					textData[i][j] = 15;
				}
				//하코넨 장판
				else if ((i == 1 || i == 2) && (j == 55 || j == 56)) {
					map[0][i][j] = 'P';
					mapData[i][j] = HARKONNEN_PLATE;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 0);
					colorData[i][j] = 0;
					textData[i][j] = 15;
				}
				//스파이스
				else if ((i == 11 && j == 1) || (i == 6 && j == 58)) {
					map[0][i][j] = 'S';
					mapData[i][j] = SPICE;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 4);
					colorData[i][j] = 4;
					textData[i][j] = 15;
				}
				//맵이 아닌 부분은 검정색 배경
				else if ((i == 0 || i > 16 || j == 0 || j > 58)) {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 0);
				}
				//바위
				else if ((i == 5 || i == 6) && (j == 32 || j == 33)) {
					map[0][i][j] = 'R';
					mapData[i][j] = ROCK;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 8);
					colorData[i][j] = 8;
					textData[i][j] = 15;
				}
				else if ((i == 9 || i == 10) && (j == 20 || j == 21)) {
					map[0][i][j] = 'R';
					mapData[i][j] = ROCK;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 8);
					colorData[i][j] = 8;
					textData[i][j] = 15;
				}
				else if ((i == 11 || i == 12) && (j == 46 || j == 47)) {
					map[0][i][j] = 'R';
					mapData[i][j] = ROCK;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 8);
					colorData[i][j] = 8;
					textData[i][j] = 15;
				}
				else if (i == 3 && j == 13) {
					map[0][i][j] = 'R';
					mapData[i][j] = ROCK;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 8);
					colorData[i][j] = 8;
					textData[i][j] = 15;
				}
				else if (i == 8 && j == 40) {
					map[0][i][j] = 'R';
					mapData[i][j] = ROCK;
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 15, 8);
					colorData[i][j] = 8;
					textData[i][j] = 15;
				}
				//맵
				else {
					POSITION pos = { i, j };
					printc(padd(map_pos, pos), backbuf[i][j], 0, 15);
					colorData[i][j] = 15;
					textData[i][j] = 15;
				}
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}

	

}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	// 이전 위치 복원
	int prevBgColor = colorData[prev.row][prev.column];
	int prevTextColor = textData[prev.row][prev.column];
	char prevChar = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), prevChar, prevTextColor, prevBgColor);

	// 현재 위치에 커서 색상 적용
	int cursorBgColor = 8;
	int cursorTextColor = 15;
	char currChar = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), currChar, cursorTextColor, cursorBgColor);
}


void display_state(char state[STATE_Y][STATE_X]) {
	state_project(state, backbuf);

	for (int i = 0; i < STATE_Y; i++) {
		for (int j = 0; j < STATE_X; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(state_pos, pos), backbuf[i][j], COLOR_DEFAULT, 15);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}