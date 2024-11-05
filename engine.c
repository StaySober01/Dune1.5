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
void sample_obj_move(void);
int get_current_object(POSITION pos);
void on_click_space(int data);
POSITION sample_obj_next_position(void);

/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

#define MOVE_DISTANCE 1
#define FAST_MOVE_DISTANCE 10
#define TIME_THRESHOLD 0.2


/* ================= game data =================== */
char map[N_LAYER][MAX_Y][MAX_X] = { 0 };
char state[STATE_Y][STATE_X] = { 0 };
char message[MESSAGE_Y][MESSAGE_X] = { 0 };
char command[COMMAND_Y][COMMAND_X] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_Y - 2, MAP_X - 2},
	.repr = 'o',
	.move_period = 300,
	.next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	clock_t lastKeyTime = 0;
	int lastKey = 0;

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		// Ű �Է��� ������ ó��
		if (is_arrow_key(key)) {
			clock_t currentTime = clock();

			double term = (double)(currentTime - lastKeyTime) / CLOCKS_PER_SEC;

			int moveDistance = (key == lastKey && term < TIME_THRESHOLD) ? FAST_MOVE_DISTANCE : MOVE_DISTANCE;
			

			cursor_move(ktod(key), moveDistance);
			lastKey = key;
			lastKeyTime = currentTime;
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_quit: outro();
			case k_space:
				for (int i = 61; i < 99; i++) {
					for (int j = 1; j < 17; j++) {
						map[0][j][i] = ' ';
					}
				}
				on_click_space(get_current_object(cursor.current));
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// ���� ������Ʈ ����
		sample_obj_move();

		// ȭ�� ���
		display(resource, map, cursor);
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
	// layer 0(map[0])�� ���� ����
	// MAP ���� �׸���
	for (int j = 0; j < MAX_X; j++) {
		map[0][0][j] = '#'; // ���� ���
		map[0][MAP_Y - 1][j] = '#'; // �Ʒ��� ���
		map[0][MAX_Y - 1][j] = '#';
	}

	for (int i = 1; i < MAX_Y - 1; i++) {
		map[0][i][0] = '#'; // ���� ���
		map[0][i][MAP_X] = '#';
		map[0][i][MAX_X - 1] = '#';
		if (i < MAP_Y - 1) {
			for (int j = 1; j < MAP_X - 1; j++) {
				map[0][i][j] = ' '; // ���δ� �� ����
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

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAX_Y; i++) {
		for (int j = 0; j < MAX_X; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (�����ϴٸ�) ������ �������� Ŀ�� �̵�
void cursor_move(DIRECTION dir, int distance) {
	for (int i = 0; i < distance; i++) {
		POSITION curr = cursor.current;
		POSITION new_pos = pmove(curr, dir);

		// validation check
		if (1 <= new_pos.row && new_pos.row <= MAP_Y - 2 && \
			1 <= new_pos.column && new_pos.column <= MAP_X - 2) {

			cursor.previous = cursor.current;
			cursor.current = new_pos;
		}
	}
	
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_Y - 2, MAP_X - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_Y - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_X - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.move_period;
}

int get_current_object(POSITION pos) {
	return mapData[pos.row][pos.column];
}

void on_click_space(int data) {
	
	switch (data) {
		case 10: {
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
			break;
		}

		case 11: {
			char msg3[] = "Plate of Atreides.";
			int msg3_len = strlen(msg3);

			for (int i = 0; i < msg3_len; i++) {
				map[0][1][i + 61] = msg3[i];
			}
			break;
		}

		case 20: {
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

		case 21: {
			char msg3[] = "Plate of Atreides.";
			int msg3_len = strlen(msg3);

			for (int i = 0; i < msg3_len; i++) {
				map[0][1][i + 61] = msg3[i];
			}
			break;
		}

		case 30: {
			char msg[] = "The most basic resource.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			break;
		}

		case 90: {
			char msg[] = "A rock that can't go through.";
			int msg_len = strlen(msg);

			for (int i = 0; i < msg_len; i++) {
				map[0][1][i + 61] = msg[i];
			}
			break;
		}

		default: {
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