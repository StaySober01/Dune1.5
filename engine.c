//1), 2) 구현 완료
//3) 샌드웜 이동 구현, 가까운 유닛에게 이동 및 잡아먹기등은 구현 못함
//4) 구현 완료
//5) 구현 완료

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir, int distance);
int get_current_object(POSITION pos);
void on_click_space(int data);
bool afford_spice(int price);
DWORD WINAPI produce_harvester(LPVOID param);
void update_msg(int count, char msg[]);
HARDATA har_next_position(HARVESTER har);
HARDATA har_move(HARVESTER har);
WORMDATA worm_next_position(SANDWORMS worm);
WORMDATA worm_move(SANDWORMS worm);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

#define MOVE_DISTANCE 1
#define FAST_MOVE_DISTANCE 10
#define TIME_THRESHOLD 0.17


/* ================= game data =================== */
char map[N_LAYER][MAX_Y][MAX_X] = { 0 };
char state[STATE_Y][STATE_X] = { 0 };
char message[MESSAGE_Y][MESSAGE_X] = { 0 };
char command[COMMAND_Y][COMMAND_X] = { 0 };

int current_obg = 0;
int msg_count = 0;
int har_count = 0;

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

HARVESTER har1 = {
	.pos = {14, 1},
	.dest = {12, 1},
	.repr = 'H',
	.move_period = 1000,
	.next_move_time = 0
};

SANDWORMS worm1 = {
	.pos = {1, 5},
	.dest = {MAP_Y - 2, MAP_X - 2},
	.tail = {1, 1},
	.mid = {{1, 2}, {1, 3}, {1, 4}},
	.repr = 'W',
	.move_period = 1000,
	.next_move_time = 0
};

SANDWORMS worm2 = {
	.pos = {MAP_Y - 2, 54},
	.dest = {1, 1},
	.tail = {MAP_Y - 2, MAP_X - 2},
	.mid = {{16, 57}, {16, 56}, {16, 55}},
	.repr = 'W',
	.move_period = 1000,
	.next_move_time = 1000
};

HARVESTER hars[MAX_HAR] = { 0 };

RESOURCE p_resource = { 20, 100, 0, 10 };
RESOURCE ai_resource = { 0, 20, 0, 10 };

bool collect_flag = false;
bool har_moved_flag = false;
bool har_arrived_flag = false;
bool worm_moved_flag = false;

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);
	display_harvester(har1);
	display_sandworm(worm1);
	display_sandworm(worm2);


	clock_t lastKeyTime = 0;
	int lastKey = 0;

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			clock_t currentTime = clock();

			double term = (double)(currentTime - lastKeyTime) / CLOCKS_PER_SEC;

			int moveDistance = (key == lastKey && term < TIME_THRESHOLD) ? FAST_MOVE_DISTANCE : MOVE_DISTANCE;
			

			cursor_move(ktod(key), moveDistance);
			lastKey = key;
			lastKeyTime = currentTime;
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_space:
				for (int i = 61; i < 99; i++) {
					for (int j = 1; j < 17; j++) {
						map[0][j][i] = ' ';
					}
				}

				for (int i = 18; i < 24; i++) {
					for (int j = 61; j < 99; j++) {
						map[0][i][j] = ' ';
					}
				}
				current_obg = get_current_object(cursor.current);
				on_click_space(current_obg);
				
				break;
			case k_esc:
				for (int i = 61; i < 99; i++) {
					for (int j = 1; j < 17; j++) {
						map[0][j][i] = ' ';
					}
				}
				for (int i = 18; i < 24; i++) {
					for (int j = 61; j < 99; j++) {
						map[0][i][j] = ' ';
					}
				}
				current_obg = 0;
				break;
			case k_h:
				if (is_command_obg(current_obg)) {
					if (afford_spice(5)) {
						int production_time = 5;
						HANDLE harvester;
						harvester = CreateThread(NULL, 0, produce_harvester, &production_time, 0, NULL);
						if (harvester == NULL) {
							return 1;
						}

						break;
					}
					else {
						char msg[] = "Not enough spice.";
						
						update_msg(msg_count, msg);
						break;
					}
				}
			case k_none:
			case k_undef:
			default: break;
			}
		}

		display(p_resource, map, cursor);

		for (int i = 0; i < har_count; i++) {
			HARDATA har_pos_data;
			har_pos_data = har_move(hars[i]);
			hars[i].pos = har_pos_data.pos;

			if (har_moved_flag) {
				hars[i].next_move_time = sys_clock + hars[i].move_period;
			}

			if (har_arrived_flag) {
				hars[i].dest = har_pos_data.des;
				har_arrived_flag = false;
			}
			display_harvester(hars[i]);
		}

		WORMDATA worm1_data;
		worm1_data = worm_move(worm1);
		worm1.pos = worm1_data.pos;
		for (int i = 0; i < 3; i++) {
			worm1.mid[i] = worm1_data.mid[i];
		}
		worm1.tail = worm1_data.tail;

		if (worm_moved_flag) {
			worm1.next_move_time = sys_clock + worm1.move_period;
		}
		display_sandworm(worm1);

		// 화면 출력
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	printf("####*******=$#################==#########==#################$*!*=$#####################=*******=####\n");
	printf("###$;::::::::;################;:$########~:################;~~~:::*##################=:::::::::*####\n");
	printf("####$$=$$$$=:~;###############:~=$#####$=~~###############;~:*$$*;:!################*~~!=$$$$=$$####\n");
	printf("######$$$$$$$:~!##############:~==#####$$~~##############!:;$$$$$$;:=##############$~~=$$$$$$$######\n");
	printf("############=#~~##############:~$=#####$$~~#############$:;#=####=$:;##############!~!$$############\n");
	printf("#############$;~$#############:~$$#####$$::#############$:*$#######!;##############::$=##*##########\n");
	printf("#############=!~=#############;:$$#####$#::#############=:=$######$!;##############::$$#=;$#########\n");
	printf("############$$!~$#############!:==#####=$:;#############=:==######$!;##############::$=##=##########\n");
	printf("############=#;;##############=:*$$###$$!:*#############=:==######$!:##############!:*$$############\n");
	printf("######=====$$;:*###############;:*#$$$#=;;##############=:==######$!:##############$;;=$=====$######\n");
	printf("####$$=$$$$*;:!################=;:!=$=!:;=##############=:*$######$!;###############*::*=$$$$=$$####\n");
	printf("###$::::::::;!##################$;::::::$###############=:=########!;################=;::::::::*####\n");
	printf("####!!!!!!!=$#####################$*!*$#################$=$########$=##################$*!!!!!!=####\n");
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	printf("####################################################################################################\n");
	Sleep(500);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	// MAP 영역 그리기
	for (int j = 0; j < MAX_X; j++) {
		map[0][0][j] = '#'; // 위쪽 경계
		map[0][MAP_Y - 1][j] = '#'; // 아래쪽 경계
		map[0][MAX_Y - 1][j] = '#';
	}

	for (int i = 1; i < MAX_Y - 1; i++) {
		map[0][i][0] = '#'; // 왼쪽 경계
		map[0][i][MAP_X] = '#';
		map[0][i][MAX_X - 1] = '#';
		if (i < MAP_Y - 1) {
			for (int j = 1; j < MAP_X - 1; j++) {
				map[0][i][j] = ' '; // 내부는 빈 공간
			}
			for (int j = MAP_X + 1; j < MAX_X - 1; j++) {
				map[0][i][j] = ' ';
			}
		}
		
		else if (i >= MAP_Y) {
			for (int j = 1; j < MAP_X; j++) {
				map[0][i][j] = ' ';
			}
			for (int j = MAP_X + 1; j < MAX_X - 1; j++) {
				map[0][i][j] = ' ';
			}
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			map[1][i][j] = -1;
		}
	}

	spiceData[11][1] = 9;
	spiceData[6][58] = 9;

	hars[0] = har1;
	har_count++;

	map[1][har1.pos.row][har1.pos.column] = har1.repr;

	map[1][worm1.pos.row][worm1.pos.column] = worm1.repr;
	for (int i = 0; i < 3; i++) {
		map[1][worm1.mid[i].row][worm1.mid[i].column] = worm1.repr;
	}
	map[1][worm1.tail.row][worm1.tail.column] = worm1.repr;

	map[1][worm2.pos.row][worm2.pos.column] = worm2.repr;
	for (int i = 0; i < 3; i++) {
		map[1][worm2.mid[i].row][worm2.mid[i].column] = worm2.repr;
	}
	map[1][worm2.tail.row][worm2.tail.column] = worm2.repr;
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir, int distance) {
	for (int i = 0; i < distance; i++) {
		POSITION curr = cursor.current;
		POSITION new_pos = pmove(curr, dir);

		// validation check
		if (1 <= new_pos.row && new_pos.row <= MAP_Y - 2 && \
			1 <= new_pos.column && new_pos.column <= MAP_X - 2) {

			display_cursor(cursor);

			cursor.previous = cursor.current;
			cursor.current = new_pos;

			display_cursor(cursor);
		}
	}
}

HARDATA har_next_position(HARVESTER har) {
	POSITION diff = psub(har.dest, har.pos);
	DIRECTION dir;

	HARDATA return_data;

	

	if (diff.row == 0 && diff.column == 0) {
		har_arrived_flag = true;
		if (!collect_flag) {
			collect_flag = true;
			spiceData[11][1] -= 1;
			map[0][11][1] = (char)(spiceData[11][1] + '0');
			
			POSITION new_dest = { 14, 1 };
			return_data.des = new_dest;
		}
		else if (p_resource.spice_max - p_resource.spice >= 5 && collect_flag) {
			p_resource.spice += 5;
			collect_flag = false;
			POSITION new_dest = { 12, 1 };
			return_data.des = new_dest;
		}
		return_data.pos = har.pos;
		return return_data;
	}

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove(har.pos, dir);
	return_data.des = har.dest;
	return_data.pos = next_pos;
	return return_data;
}

HARDATA har_move(HARVESTER har) {
	HARDATA har_data;

	if (sys_clock <= har.next_move_time) {
		har_moved_flag = false;
		har_data.pos = har.pos;
		return har_data;
	}

	map[1][har.pos.row][har.pos.column] = -1;
	unitData[har.pos.row][har.pos.column] = -1;
	colorData[har.pos.row][har.pos.column] = 15;
	textData[har.pos.row][har.pos.column] = 0;
	har_data = har_next_position(har);
	har.pos = har_data.pos;
	map[1][har.pos.row][har.pos.column] = har.repr;

	har_moved_flag = true;

	return har_data;
}

WORMDATA worm_next_position(SANDWORMS worm) {
	POSITION diff = psub(worm.dest, worm.pos);
	DIRECTION dir;

	WORMDATA return_data;

	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	POSITION next_pos = pmove(worm.pos, dir);
	return_data.des = worm.dest;
	return_data.pos = next_pos;

	return_data.tail = worm.mid[0];
	return_data.mid[0] = worm.mid[1];
	return_data.mid[1] = worm.mid[2];
	return_data.mid[2] = worm.pos;

	return return_data;
}

WORMDATA worm_move(SANDWORMS worm) {
	WORMDATA worm_data;

	if (sys_clock <= worm.next_move_time) {
		worm_moved_flag = false;
		worm_data.pos = worm.pos;
		for (int i = 0; i < 3; i++) {
			worm_data.mid[i] = worm.mid[i];
		}
		worm_data.tail = worm.tail;
		return worm_data;
	}

	map[1][worm.tail.row][worm.tail.column] = -1;
	unitData[worm.tail.row][worm.tail.column] = -1;
	colorData[worm.tail.row][worm.tail.column] = 15;
	textData[worm.tail.row][worm.tail.column] = 0;

	worm_data = worm_next_position(worm);
	worm.pos = worm_data.pos;

	map[1][worm.pos.row][worm.pos.column] = worm.repr;
	for (int i = 0; i < 3; i++) {
		map[1][worm_data.mid[i].row][worm_data.mid[i].column] = worm.repr;
	}
	map[1][worm_data.tail.row][worm_data.tail.column] = worm.repr;

	worm_moved_flag = true;

	return worm_data;
}

int get_current_object(POSITION pos) {
	if (is_unit(unitData[pos.row][pos.column])) {
		return unitData[pos.row][pos.column];
	}
	else {
		return mapData[pos.row][pos.column];
	}
}

void on_click_space(int data) {
	
	switch (data) {
		case ATREIDES_BASE: {
			char msg[] = "Base of Atreides.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}

			char msg2[] = "It can produce harvesters.";
			int msg2_len = strlen(msg2);

			for (int i = 0; i < msg2_len; i++) {
				map[0][2][i + 61] = msg2[i];
			}

			char msg3[] = "H : Produce a harvester";
			int msg3_len = strlen(msg3);

			for (int i = 0; i < msg3_len; i++) {
				map[0][18][i + 61] = msg3[i];
			}

			char msg4[] = "(takes 5 seconds)";
			int msg4_len = strlen(msg4);

			for (int i = 0; i < msg4_len; i++) {
				map[0][19][i + 61] = msg4[i];
			}
			break;
		}

		case ATREIDES_PLATE: {
			char msg3[] = "Plate of Atreides.";
			int msg3_len = strlen(msg3);

			for (int i = 0; i < msg3_len; i++) {
				map[0][1][i + 61] = msg3[i];
			}
			break;
		}

		case ATREIDES_HAR: {
			char msg[] = "Harvester of Atreides.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			break;
		}

		case HARKONNEN_BASE: {
			char msg[] = "Base of Harkonnen.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}

			char msg2[] = "It can produce harvesters.";
			int msg2_len = strlen(msg2);

			for (int i = 0; i < msg2_len; i++) {
				map[0][2][i + 61] = msg2[i];
			}
			break;
		}

		case HARKONNEN_PLATE: {
			char msg3[] = "Plate of Atreides.";
			int msg3_len = strlen(msg3);

			for (int i = 0; i < msg3_len; i++) {
				map[0][1][i + 61] = msg3[i];
			}
			break;
		}

		case SPICE: {
			char msg[] = "The most basic resource.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			break;
		}

		case SANDWORM: {
			char msg[] = "Planet's neutral unit.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}

			char msg2[] = "It eats the unit it encounters.";
			int msg2_len = strlen(msg2);

			for (int i = 0; i < msg2_len; i++) {
				map[0][2][i + 61] = msg2[i];
			}
			break;
		}

		case ROCK: {
			char msg[] = "A rock that can't go through.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			break;
		}

		default: {//기본 지형(건설불가)
			char msg[] = "The topography is bad,";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			char msg2[] = "so you can't build a building.";
			int msg2_len = strlen(msg2);

			for (int i = 0; i < msg2_len; i++) {
				map[0][2][i + 61] = msg2[i];
			}
			break;
		}
	}
}

bool afford_spice(int price) {
	if (price <= p_resource.spice) {
		p_resource.spice -= price;
		return true;
	}
	else {
		return false;
	}
}

void update_msg(int count, char msg[]) {
	int msg_len = strlen(msg);

	if (count < 6) {
		for (int i = 0; i < msg_len; i++) {
			map[0][count + 18][i + 1] = msg[i];
		}
		msg_count++;
	}
	else {
		for (int i = 19; i < 24; i++) {
			for (int j = 1; j < 60; j++) {
				map[0][i - 1][j] = map[0][i][j];
			}
		}

		for (int i = 1; i < 60; i++) {
			map[0][23][i] = ' ';
		}

		for (int i = 0; i < msg_len; i++) {
			map[0][23][i + 1] = msg[i];
		}
	}
}

DWORD WINAPI produce_harvester(LPVOID param) {
	int production_time = *(int*)param;

	char msg[] = "Producing harvester... It will takes 5 seconds.";
	update_msg(msg_count, msg);

	Sleep(production_time * 1000);

	char msg2[] = "Harvester produced.";

	update_msg(msg_count, msg2);

	HARVESTER har = {
		.pos = {14, 1},
		.dest = {12, 1},
		.repr = 'H',
		.move_period = 1000,
		.next_move_time = 1000
	};
	display_harvester(har);
	hars[har_count] = har;
	har_count++;

	return 0;
}