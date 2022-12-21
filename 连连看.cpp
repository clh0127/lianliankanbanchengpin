#include<stdio.h>
#include<time.h>
#include<graphics.h>
#define MAP_SIZE 10 //游戏难度
#define IMG_SIZE 39
#define WIDTH (MAP_SIZE + 2) * IMG_SIZE

int map[MAP_SIZE + 2][MAP_SIZE + 2];
IMAGE img_bk;
IMAGE item[MAP_SIZE + 2];

struct Index {
	int row;
	int col;
}begin = { -1, -1 }, end = { -1, -1 };

enum Cur {
	BEGIN,
	END
};
Cur state = BEGIN;

//数组建立
void show_map() {
	for (int i = 0; i < MAP_SIZE + 2; i++) {
		for (int j = 0; j < MAP_SIZE + 2; j++) {
			printf("%-2d ", map[i][j]);
		}
		printf("\n");
	}
}

//游戏初始化
void game_init() {
	srand((unsigned int)time(NULL));
	for (int i = 1; i <= MAP_SIZE; i++) {
		for (int j = 1; j <= MAP_SIZE; j++) {
			map[i][j] = i;
		}
	}
	
	//载入图片
	loadimage(&img_bk, "./res/bk.jpg", WIDTH, WIDTH);

	loadimage(&item[1], "./res/1.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[2], "./res/2.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[3], "./res/3.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[4], "./res/4.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[5], "./res/5.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[6], "./res/6.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[7], "./res/7.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[8], "./res/8.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[9], "./res/9.png", IMG_SIZE, IMG_SIZE);
	loadimage(&item[10], "./res/10.png", IMG_SIZE, IMG_SIZE);

	//打乱数组
	for (int i = 1; i <= MAP_SIZE; i++) {
		for (int j = 1; j <= MAP_SIZE; j++) {
			int x = rand() % MAP_SIZE + 1;
			int y = rand() % MAP_SIZE + 1;
			int temp = map[i][j];
			map[i][j] = map[x][y];
			map[x][y] = temp;
		}
	}
}

//游戏界面绘制
void game_draw() {
	putimage(0, 0, &img_bk);

	for (int i = 1; i <= MAP_SIZE; i++) {
		for (int j = 1; j <= MAP_SIZE; j++) {
			putimage(j * IMG_SIZE, i * IMG_SIZE, &item[map[i][j]]);
		}
	}
}

//获取两次鼠标
void mouse_event() {
	if (MouseHit()) {
		MOUSEMSG msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN && state == BEGIN) {
			begin.col = msg.x / IMG_SIZE;
			begin.row = msg.y / IMG_SIZE;
			state = END;
		}
		else if (msg.uMsg == WM_LBUTTONDOWN && state == END) {
			end.col = msg.x / IMG_SIZE;
			end.row = msg.y / IMG_SIZE;
			state = BEGIN;
		}
		if (msg.uMsg == WM_LBUTTONDOWN) {
			printf("begin(%d,%d) %d end(%d,%d) %d\n", begin.row, begin.col, map[begin.row][begin.col], end.row, end.col, map[end.row][end.col]);
		}
	}
}

bool isBlock(int row, int col) {
	return map[row][col];
}

//水平消除
bool horizon(struct Index begin, struct Index end) {
	if (map[begin.row][begin.col] != map[end.row][end.col]) {
		return false;
	}
	if (begin.row != end.row) {
		return false;
	}
	int colMin = min(begin.col, end.col);
	int colMax = max(begin.col, end.col);
	for (int cur = colMin + 1; cur < colMax; cur++) {
		if (isBlock(begin.row, cur)) {
			return false;
		}
	}
	return true;
}

//垂直消除
bool vertical(struct Index begin, struct Index end) {
	if (map[begin.row][begin.col] != map[end.row][end.col]) {
		return false;
	}
	if (begin.col != end.col) {
		return false;
	}
	int rowMin = min(begin.row, end.row);
	int rowMax = max(begin.row, end.row);
	for (int cur = rowMin + 1; cur < rowMax; cur++) {
		if (isBlock(cur, begin.col)) {
			return false;
		}
	}
	return true;
}

//一次转角消除
bool turn_once(struct Index begin, struct Index end) {
	if (map[begin.row][begin.col] != map[end.row][end.col]) {
		return false;
	}
	struct Index temp1 = { begin.row, end.col }, temp2 = {end.row, begin.col};
	bool flag = false;
	if (!isBlock(temp1.row, temp1.col)) {
		map[temp1.row][temp1.col] = map[begin.row][begin.col];
		flag |= horizon(begin, temp1) && vertical(end, temp1);
		map[temp1.row][temp1.col] = 0;
	}
	if (!isBlock(temp2.row, temp2.col)) {
		map[temp2.row][temp2.col] = map[begin.row][begin.col];
		flag |= horizon(begin, temp2) && vertical(end, temp2);
		map[temp2.row][temp2.col] = 0;
	}
	return flag;
}


//消除
bool earse(struct Index begin, struct Index end) {
	if (begin.row == end.row && begin.col == end.col) {
		return false;
	}
	int flag = false;
	flag = horizon(begin, end);
	if (flag == true) {
		return true;
	}
	flag = vertical(begin, end);
	if (flag == true) {
		return true;
	}
	flag = turn_once(begin, end);
	if (flag == true) {
		return true;
	}
	return flag;
}

int main() {
	initgraph(WIDTH, WIDTH, SHOWCONSOLE);
	printf("%d\n", WIDTH);
	game_init();
	show_map();
	game_draw();
	while (1) {
		BeginBatchDraw();
		game_draw();
		EndBatchDraw();
		if (earse(begin, end) && begin.row != -1 && end.row != -1) {
			map[begin.row][begin.col] = 0;
			map[end.row][end.col] = 0;
			begin.row = -1;
			begin.col = -1;
			end.row = -1;
			end.col = -1;
		}
		mouse_event();
	}
	getchar();
	return 0;
}