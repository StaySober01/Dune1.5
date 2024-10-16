/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"


char backbuf[MAX_HEIGHT][MAX_WIDTH] = { 0 };
char frontbuf[MAX_HEIGHT][MAX_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_Y][MAP_X], char dest[MAX_HEIGHT][MAX_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_Y][MAP_X]);
void display_state(char state[STATE_Y][STATE_X - MAP_Y]);
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
	char state[STATE_Y][STATE_X - MAP_Y],
	char message[MESSAGE_Y - MAP_Y][MESSAGE_X],
	char command[COMMAND_Y - MAP_Y][COMMAND_X - MAP_X],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_state(state);
	display_cursor(cursor);
	// display_system_message()
	// display_object_info()
	// display_commands()
	// ...
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
void project(char src[N_LAYER][MAP_Y][MAP_X], char dest[MAX_HEIGHT][MAX_WIDTH]) {
	for (int i = 0; i < MAP_Y; i++) {
		for (int j = 0; j < MAP_X; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void state_project(char src[STATE_Y][STATE_X - MAP_Y], char dest[MAX_HEIGHT][MAX_WIDTH]) {
	for (int i = 0; i < MAP_Y; i++) {
		for (int j = 0; j < MAP_X; j++) {
			if (src[i][i] >= 0) {
				dest[i][i] = src[i][j];
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_Y][MAP_X]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_Y; i++) {
		for (int j = 0; j < MAP_X; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

void display_state(char state[STATE_Y][STATE_X - MAP_Y]) {
	state_project(state, backbuf);

	for (int i = 0; i < STATE_Y; i++) {
		for (int j = 0; j < STATE_X - MAP_Y; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(state_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}