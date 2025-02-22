#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>

/* ================ map data parameters ================== */

#define ATREIDES_BASE 10
#define ATREIDES_PLATE 11
#define ATREIDES_HAR 12

#define HARKONNEN_BASE 20
#define HARKONNEN_PLATE 21

#define SPICE 30

#define SANDWORM 40

#define ROCK 90



/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define MAX_X 100
#define MAX_Y 25

#define N_LAYER 2
#define MAP_X	60
#define MAP_Y	18

#define STATE_X 100
#define STATE_Y 18

#define MESSAGE_X 60
#define MESSAGE_Y 25

#define COMMAND_X 100
#define COMMAND_Y 25

#define MAX_HAR 20

int mapData[MAP_Y][MAP_X];
int unitData[MAP_Y][MAP_X];
int colorData[MAP_Y][MAP_X];
int textData[MAP_Y][MAP_X];
int spiceData[MAP_Y][MAP_X];

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit,
	k_space,
	k_esc,
	k_h, k_s, k_f, k_m,
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;




/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

#define is_command_key(k)   (k_h <= (k) && (k) <= k_m)

#define is_command_obg(o)   ((o) == ATREIDES_BASE)

#define is_unit(u)          ((u) == ATREIDES_HAR || (u) == SANDWORM)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
} OBJECT_SAMPLE;

typedef struct {
	POSITION pos;
	POSITION dest;
	char repr;
	int move_period;
	int next_move_time;
}HARVESTER;

typedef struct {
	POSITION pos;
	POSITION dest;
	POSITION tail;
	POSITION mid[3];
	char repr;
	int move_period;
	int next_move_time;
}SANDWORMS;

POSITION wormpos[2][5];

typedef struct {
	POSITION des;
	POSITION pos;
} HARDATA;

typedef struct {
	POSITION des;
	POSITION pos;
	POSITION mid[3];
	POSITION tail;
}WORMDATA;

#endif

