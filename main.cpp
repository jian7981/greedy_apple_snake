#include "raylib.h"
#include <iostream>
#include <vector>
using namespace std;
//定义纹理
Texture2D Headup, Headdown, Headleft, Headright;
Texture2D bodyhend, bodyver, bodyturn1, bodyturn2, bodyturn3, bodyturn4;
Texture2D tailup, taildown, tailleft, tailright;
Texture2D wall, food, poison, door,trap;
void LoadAllTextures() {
    Headup = LoadTexture("pictures\\headup.png");
    Headdown = LoadTexture("pictures\\headdown.png");
    Headleft = LoadTexture("pictures\\headleft.png");
    Headright = LoadTexture("pictures\\headright.png");
    bodyhend = LoadTexture("pictures\\bodyhend.png");
    bodyver = LoadTexture("pictures\\bodyver.png");
    bodyturn1 = LoadTexture("pictures\\bodyturn1.png");
    bodyturn2 = LoadTexture("pictures\\bodyturn2.png");
    bodyturn3 = LoadTexture("pictures\\bodyturn3.png");
    bodyturn4 = LoadTexture("pictures\\bodyturn4.png");
    tailup = LoadTexture("pictures\\tailup.png");
    taildown = LoadTexture("pictures\\taildown.png");
    tailleft = LoadTexture("pictures\\tailleft.png");
    tailright = LoadTexture("pictures\\tailright.png");
	food = LoadTexture("pictures\\food.png");
	wall = LoadTexture("pictures\\wall.png");
	poison = LoadTexture("pictures\\poison.png");
	door = LoadTexture("pictures\\door.png");
	trap = LoadTexture("pictures\\trap.png");
}//加载所有纹理的函数，集中管理纹理资源，方便维护和修改
typedef enum {
	state_main_menu,   // 主菜单状态
    state_level_select,// 关卡选择状态
	state_gameplay,     // 游戏进行状态
	state_game_over     // 游戏结束状态（可选）
} GameState;

typedef enum{
    UP, DOWN, LEFT, RIGHT
} Direction;

Direction dir =RIGHT; // 初始方向向右

GameState currentState = state_main_menu;
int currentLevel = 0;                  // 当前关卡
bool isDropping = false;// 是否正在掉落（掉落时禁止操作）
bool snake_ispoisoned = false; // 蛇是否中毒
bool snake_isdead = false; // 蛇是否死亡

vector<Vector2> snake_body; // 蛇身坐标列表
vector<vector<Vector2>> snake_body_history; // 断掉的蛇身历史记录，用于中毒后掉落蛇尾的一半
vector<bool> body_history_dropping;   // 每个断块是否正在掉落
vector<float> body_history_drop_timer;// 每个断块掉落计时器
vector<vector<Vector2>> snake_initial_positions = {
    { {6, 10}, {5, 10}, {4, 10} }, // 第一关初始蛇身位置
    { {6, 10}, {5, 10}, {4, 10} }// 第二关初始蛇身位置（可以根据需要修改）
};

//关卡设计,横一共30格，竖一共20格
//关卡的设计底部以x=6，y=13为起点，宽18格，高1格为准
//所以其他墙壁的x要大于等于6，y要小于等于13，宽度要小于等于18，高度（宽度）等于1
//用数组存
int size_square = 40; // 每个格子的大小

float dropTimer = 0;
const float DROP_SPEED = 0.1f;   // 掉落速度（越小越快）
// 0表示空地，1表示墙壁，2表示食物，3表示毒药,4表示终点,5表示陷阱，关卡定义
vector<vector<vector<int>>> levels = {    //关卡设计，0表示空地，1表示墙壁，2表示食物，3表示毒药,4表示终点,5表示陷阱
    {    //      5 &      10        15        20        25        30
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=5
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=10
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,0,3,3,0,0,0,4,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0},//*
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=15
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=20

    },
    {
        //      5 &      10        15        20        25        30
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=5
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=10
       {0,0,0,0,0,0,0,0,0,0,0,1,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,4,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//*
       {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=15
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
       {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//y=20
    }
};

vector<vector<int>> current_level = levels[0]; // 当前关卡
void init_snake(vector<Vector2> snake_initial_positions_cur) { // 初始化蛇身,每次关卡开始时调用，重置蛇身位置和状态
    snake_body.clear();
    snake_body.push_back(snake_initial_positions_cur[0]); // 蛇头初始位置
    snake_body.push_back(snake_initial_positions_cur[1]);
    snake_body.push_back(snake_initial_positions_cur[2]);
    dir = RIGHT;
    isDropping = false;
	snake_ispoisoned = false;
	snake_isdead = false;
    // 清空断尾所有状态
    snake_body_history.clear();
    body_history_dropping.clear();
    body_history_drop_timer.clear();
}

void draw_level(const vector<vector<int>>& level) {  //地图绘制函数
    for (int y = 0; y < level.size(); y++) {
        for (int x = 0; x < level[y].size(); x++) {
			if (level[y][x] == 1) DrawTexture(wall, x * size_square, y * size_square, WHITE);
            else if(level[y][x] == 2) DrawTexture(food, x * size_square, y * size_square, WHITE);
            else if(level[y][x] == 3) DrawTexture(poison, x * size_square, y * size_square, WHITE);
            else if(level[y][x] == 4) DrawTexture(door, x * size_square, y * size_square, WHITE);
			else if (level[y][x] == 5) DrawTexture(trap, x * size_square, y * size_square, WHITE);
        }   
    }
}

void draw_snake() {
    // 安全判断：蛇没有身体直接退出
    if (snake_body.empty()) return;

    for (int i = 0; i < snake_body.size(); i++) {
        Vector2 segment = snake_body[i];
        float x = segment.x * size_square;  // 最终绘制X坐标
        float y = segment.y * size_square;  // 最终绘制Y坐标

        // ===================== 绘制蛇头 =====================
        if (i == 0) {
            if (dir == UP)      DrawTexture(Headup, x, y, WHITE);
            else if (dir == DOWN)  DrawTexture(Headdown, x, y, WHITE);
            else if (dir == LEFT)  DrawTexture(Headleft, x, y, WHITE);
            else if (dir == RIGHT) DrawTexture(Headright, x, y, WHITE);
        }

        // ===================== 绘制蛇尾 =====================
        else if (i == snake_body.size() - 1) {
            Vector2 prev = snake_body[i - 1];
            if (segment.x == prev.x) { // 竖直
                if (segment.y < prev.y) DrawTexture(tailup, x, y, WHITE);
                else DrawTexture(taildown, x, y, WHITE);
            }
            else { // 水平
                if (segment.x < prev.x) DrawTexture(tailleft, x, y, WHITE);
                else DrawTexture(tailright, x, y, WHITE);
            }
        }

        // ===================== 绘制蛇身（补全4种转弯！）=====================
        else {
            Vector2 prev = snake_body[i - 1];
            Vector2 next = snake_body[i + 1];

            // 1. 竖直身体
            if (prev.x == next.x) {
                DrawTexture(bodyver, x, y, WHITE);
            }
            // 2. 水平身体
            else if (prev.y == next.y) {
                DrawTexture(bodyhend, x, y, WHITE);
            }
            // 3. 补全 ✅ 4种转弯纹理（你之前只写了1种）
            // 左上转弯 ↖
            else if ((prev.x < segment.x && next.y < segment.y) || (next.x < segment.x && prev.y < segment.y)) {
                DrawTexture(bodyturn1, x, y, WHITE);
            }
            // 右上转弯 ↗
            else if ((prev.x > segment.x && next.y < segment.y) || (next.x > segment.x && prev.y < segment.y)) {
                DrawTexture(bodyturn2, x, y, WHITE);
            }
            // 左下转弯 ↙
            else if ((prev.x < segment.x && next.y > segment.y) || (next.x < segment.x && prev.y > segment.y)) {
                DrawTexture(bodyturn3, x, y, WHITE);
            }
            // 右下转弯 ↘
            else if ((prev.x > segment.x && next.y > segment.y) || (next.x > segment.x && prev.y > segment.y)) {
                DrawTexture(bodyturn4, x, y, WHITE);
            }
        }
    }
}

void draw_snake_history() {
    for (int i = 0; i < snake_body_history.size(); i++) {
        for (int j = 0; j < snake_body_history[i].size(); j++) {
            Vector2 segment = snake_body_history[i][j];
            float x = segment.x * size_square;
            float y = segment.y * size_square;
            DrawTexture(bodyver, x, y, WHITE); // 绘制断掉的蛇身为竖直身体纹理
        }
    }
}

// 判断坐标是否是【断尾】
bool is_body_history(int x, int y) {
    for (auto& piece : snake_body_history) {
        for (auto& seg : piece) {
            if (seg.x == x && seg.y == y) return true;
        }
    }
    return false;
}

bool is_wall(int x, int y) {  // 判断指定坐标是否是墙壁，或者超出地图边界（也算作墙壁）
	if (current_level[y][x] == 1||y>=20||y<=0||x>=30||x<=0 ) return true;
	else return false;
}

bool is_food(int x, int y) {  // 判断指定坐标是否是食物，如果是，返回true
    if (current_level[y][x] == 2|| current_level[y][x] == 3) return true;
    else return false;
}

bool is_body(int x, int y) {   // 判断指定坐标是否是蛇身的某个部分，如果是，返回true
    for (int i = 0; i < snake_body.size(); i++) {
        if (snake_body[i].x == x && snake_body[i].y == y) return true;
    }
    return false;
}

// 判断某个格子正下方有没有支撑：墙 / 食物 / 断尾 都算支撑
// 判断某个格子正下方有没有支撑：墙 / 食物 / 其他断尾
// ignoreBlockIdx：忽略当前自己这一块断尾，避免自己撑自己
// 判断某个格子正下方有没有支撑
// 规则：墙/边界/食物/蛇身/其他断尾 = 支撑
// 唯独：自己这一块断尾本身 = 不算支撑
// ========== 专用1：给【断尾】用的支撑判断（蛇身=支撑）==========
bool has_support_for_tail(int x, int y, int ignoreBlockIdx)
{
    int nx = x;
    int ny = y + 1;  // 正下方格子

    // 1. 墙壁/边界/食物 = 支撑
    if (is_wall(nx, ny) || is_food(nx, ny)) return true;
    // 2. 蛇身 = 支撑（断尾掉在蛇身上，停止下落）
    if (is_body(nx, ny)) return true;
    // 3. 其他断尾 = 支撑（忽略自己，防止判断自己）
    for (int i = 0; i < (int)snake_body_history.size(); i++)
    {
        if (i == ignoreBlockIdx) continue;
        for (auto& seg : snake_body_history[i])
        {
            if (seg.x == nx && seg.y == ny) return true;
        }
    }
    // 无任何支撑
    return false;
}

// ========== 专用2：给【蛇】用的支撑判断（蛇身≠支撑）==========
bool has_support_for_snake(int x, int y)
{
    int nx = x;
    int ny = y + 1;

    // 墙/边界/食物 = 支撑
    if (is_wall(nx, ny) || is_food(nx, ny)) return true;
    // 断尾 = 支撑
    for (auto& piece : snake_body_history)
    {
        for (auto& seg : piece)
        {
            if (seg.x == nx && seg.y == ny) return true;
        }
    }
    // 蛇自己的身体 = 不算支撑！（修复悬空BUG）
    return false;
}

bool is_snake_on_wall(vector<Vector2> body) { //检查蛇身的每个部分下面是否是墙壁，如果有任何一个部分下面不是墙壁，就返回true，表示蛇需要掉落
    // 蛇身任意一节下面有支撑(墙/食物/断尾) → 不会掉
    for (int i = 0; i < body.size(); i++)
    {
        // 蛇用专用判断！
        if (has_support_for_snake(body[i].x, body[i].y))
            return true;
    }
    return false;
}

// ========== 断尾专用：检测是否落地（墙/食物/蛇身/其他断尾=支撑）==========
bool is_tail_on_support(vector<Vector2> tail_piece) {
    for (auto& seg : tail_piece)
    {
        // 用断尾自己的支撑判断！蛇身算支撑
        if (has_support_for_tail(seg.x, seg.y, -1))
            return true;
    }
    return false;
}

void alldrop_snake() { //掉落函数，负责让蛇自动向下掉落
	for (int i = 0; i < snake_body.size(); i++) {
        snake_body[i].y += 1; //整体向下掉落一格
    }
}

// 仿蛇逻辑：单个断尾执行下落一格
// 仿蛇逻辑：单个断尾执行下落一格（下落前检测，不穿模）
void drop_single_history(int index) {
    if (index < 0 || index >= (int)snake_body_history.size()) return;

    // 🔥 关键：先判断能不能下落，再移动！防止重合
    bool canDrop = true;
    for (auto& seg : snake_body_history[index]) {
        if (has_support_for_tail(seg.x, seg.y, index)) {
            canDrop = false;
            break;
        }
    }
    if (!canDrop) {
        body_history_dropping[index] = false;
        return;
    }

    // 无支撑，才下落
    for (auto& seg : snake_body_history[index]) {
        seg.y += 1;
    }
}

// 仿蛇逻辑：检查断尾是否需要掉落
// 任意形状断尾通用检测：只要有任意一格下方有外部支撑就不掉落
void check_history_drop(int index)
{
    if (index < 0 || index >= (int)snake_body_history.size()) return;
    if (body_history_dropping[index]) return;

    bool anyHasSupport = false;
    // 遍历当前断尾每一格
    for (auto& seg : snake_body_history[index])
    {
        // 检测下方支撑，忽略自己这块断尾
        if (has_support_for_tail(seg.x, seg.y, index))
        {
            anyHasSupport = true;
            break; // 只要有一格有支撑，整块都不掉
        }
    }

    // 没有任何一格有外部支撑 → 整块开始掉落
    body_history_dropping[index] = !anyHasSupport;
}

void drop_snake() {
    if (isDropping) return;
	if (is_snake_on_wall(snake_body)) {
        isDropping = false;
        return; 
    }
    isDropping = true;
}

bool out_snake() {
    //检查蛇是否完全掉出地图了，如果蛇的每个部分的y坐标都大于20了，就返回true，表示蛇完全掉出去了
    for (int i = 0; i < snake_body.size(); i++) {
        if (snake_body[i].y >= 19) return true;
    }
    return false;
}

// 【修复版】蛇头撞断尾 → 断尾向移动方向整体被推，阻挡：墙/边界/食物/毒药
// 【终极修复】只推动蛇头碰到的那一个断尾，其他断尾不动
// 【终极修复】推动单个断尾 + 相连断尾联动 + 推动后强制下落
// 【最终修复】联动推动：多个相连断尾一起被推动（推箱子逻辑）
// 递归检测整串相连断尾能否被推动
bool canMoveBlock(int idx, int dx, int dy) {
    if (idx < 0 || idx >= (int)snake_body_history.size()) return true;
    for (auto& seg : snake_body_history[idx]) {
        int nx = seg.x + dx;
        int ny = seg.y + dy;
        // 撞墙/食物毒药 推不动
        if (is_wall(nx, ny) || is_food(nx, ny))
            return false;

        // 撞到其他断尾 递归检测能不能一起推
        for (int i = 0; i < (int)snake_body_history.size(); i++) {
            if (i == idx) continue;
            for (auto& s : snake_body_history[i]) {
                if (s.x == nx && s.y == ny) {
                    if (!canMoveBlock(i, dx, dy))
                        return false;
                }
            }
        }
    }
    return true;
}

// 【修复版】整段断尾掉落 + 掉出地图安全清除（解决卡死）
// 【终极防崩版】断尾自动下落 + 安全删除，永远不闪退
// 【仿蛇原版逻辑】断尾自动掉落 + 安全出界删除，永不卡死

void update_body_history_drop() 
{
    for (int i = 0; i < (int)snake_body_history.size(); i++) {
        if (i >= (int)body_history_dropping.size()) body_history_dropping.push_back(false);
        if (i >= (int)body_history_drop_timer.size()) body_history_drop_timer.push_back(0);

        if (!body_history_dropping[i]) {
            check_history_drop(i);
            continue;
        }

        body_history_drop_timer[i] += GetFrameTime();
        if (body_history_drop_timer[i] >= DROP_SPEED) {
            drop_single_history(i); // 已内置防穿模检测
            body_history_drop_timer[i] = 0;

            // 出界删除
            bool out = false;
            for (auto& seg : snake_body_history[i]) {
                if (seg.y >= 19) { out = true; break; }
            }
            if (out) {
                snake_body_history.erase(snake_body_history.begin() + i);
                body_history_dropping.erase(body_history_dropping.begin() + i);
                body_history_drop_timer.erase(body_history_drop_timer.begin() + i);
                i--;
            }
        }
    }
}


// 递归移动整串相连断尾
void moveBlock(int idx, int dx, int dy) {
    if (idx < 0 || idx >= (int)snake_body_history.size()) return;
    // 先推被顶住的其他断尾
    for (int i = 0; i < (int)snake_body_history.size(); i++) {
        if (i == idx) continue;
        for (auto& s : snake_body_history[i]) {
            for (auto& seg : snake_body_history[idx]) {
                if (s.x == seg.x + dx && s.y == seg.y + dy) {
                    moveBlock(i, dx, dy);
                }
            }
        }
    }
    // 再移动自己这节断尾
    for (auto& seg : snake_body_history[idx]) {
        seg.x += dx;
        seg.y += dy;
    }
    check_history_drop(idx);
}

// 新推动断尾函数
// 新推动断尾函数
// 新推动断尾函数
void push_body_history(int push_dx, int push_dy, int targetX, int targetY) {
    int targetIdx = -1;
    for (int i = 0; i < (int)snake_body_history.size(); i++) {
        for (auto& seg : snake_body_history[i]) {
            if (seg.x == targetX && seg.y == targetY) {
                targetIdx = i;
                break;
            }
        }
        if (targetIdx != -1) break;
    }
    if (targetIdx == -1) return;

    if (canMoveBlock(targetIdx, push_dx, push_dy)) {
        moveBlock(targetIdx, push_dx, push_dy);
    }

    // 重新检测所有断尾支撑
    for (int i = 0; i < (int)snake_body_history.size(); i++)
    {
        check_history_drop(i);
    }
}



void update_drop() {
    //掉落函数，负责让蛇自动向下掉落，直到碰到墙壁为止
    
    if (!isDropping) return;
    dropTimer += GetFrameTime();
    if (dropTimer >= DROP_SPEED) {
        alldrop_snake();
        //drop_body_history();
        dropTimer = 0;
        if(out_snake()){
            snake_body.clear(); //蛇完全掉出地图了，清空蛇身
            snake_ispoisoned = false; //蛇不再中毒了
            isDropping = false; //掉落结束，允许再次调用
			snake_isdead = true; //蛇死亡
			currentState = state_game_over; //游戏结束
            return;
		}
        if (is_snake_on_wall(snake_body)) {
            isDropping = false;
        }
    }; // 掉落结束，允许再次调用
}

bool move_snake(int dx, int dy) {  //真正的移动函数，负责更新蛇身坐标和处理吃食物、吃毒药、到达终点等逻辑
    if (isDropping) return false; // 如果正在掉落，禁止移动

    Vector2 new_head = { snake_body[0].x + dx, snake_body[0].y + dy };
    if (is_wall(new_head.x, new_head.y)) {
        return false; // 如果新位置是墙壁，直接返回，不移动
    }

	if (is_body(new_head.x, new_head.y)) {
		return false; // 如果新位置是蛇身，直接返回，不移动
	}
    // 推断尾
    if (is_body_history(new_head.x, new_head.y)) {
        push_body_history(dx, dy, new_head.x, new_head.y);
        // 🔥 删掉重复的 update_body_history_drop()！
    }

    // 推完还在断尾位置 禁止移动
    if (is_body_history(new_head.x, new_head.y)) {
        return false;
    }
    //判断蛇是否中过毒
    if (snake_ispoisoned) {
        //蛇从中间断开，丢掉蛇尾的一半
        int half_size = (snake_body.size() + 1) / 2;            //超细节处理断掉问题，3节断1节，8节断4节
        vector<Vector2> temp(snake_body.begin() + half_size, snake_body.end()); //把断掉的蛇身存入临时变量
        snake_body_history.push_back(temp); //把断掉的蛇身加入历史记录
        body_history_dropping.push_back(false);  // 初始不掉落
        body_history_drop_timer.push_back(0);    // 计时器清0
        int csize = snake_body.size();
        for (int i = half_size; i < csize; i++) {
            snake_body.pop_back(); //丢掉蛇尾的一半
        }
        snake_ispoisoned = false; //蛇不再中毒了
    }
    if (current_level[new_head.y][new_head.x] == 3) {
        snake_body.insert(snake_body.begin(), new_head); // 在蛇头前插入新位置
        snake_body.pop_back(); // 没有吃到食物，正常移动（尾巴去掉）
        current_level[new_head.y][new_head.x] = 0; // 吃掉毒药，地图上该位置变为空地
        snake_ispoisoned = true; // 吃到毒药，蛇进入中毒状态
    }
    else if (current_level[new_head.y][new_head.x] == 4) {
        currentState = state_game_over; // 到达终点，游戏结束
    }
    else if (current_level[new_head.y][new_head.x] == 2) {
        snake_body.insert(snake_body.begin(), new_head);// 在蛇头前插入新位置
        current_level[new_head.y][new_head.x] = 0; // 吃掉食物，地图上该位置变为空地
    }
    else {
        snake_body.insert(snake_body.begin(), new_head); // 在蛇头前插入新位置
        snake_body.pop_back(); // 没有吃到食物，正常移动（尾巴去掉）
    }
    drop_snake(); // 看看蛇是否需要掉落（如果新位置下面是空地，就让蛇掉落）
	return true; // 移动成功
}

void player_move_snake() {
    if (isDropping) return;

    // 🔥 修复：先移动，移动成功了，再改方向！
    if (IsKeyPressed(KEY_UP) && dir != DOWN) {
        if (move_snake(0, -1)) {  // 只有移动成功
            dir = UP;             // 才修改方向
        }
    }
    else if (IsKeyPressed(KEY_DOWN) && dir != UP) {
        if (move_snake(0, 1)) {
            dir = DOWN;
        }
    }
    else if (IsKeyPressed(KEY_LEFT) && dir != RIGHT) {
        if (move_snake(-1, 0)) {
            dir = LEFT;
        }
    }
    else if (IsKeyPressed(KEY_RIGHT) && dir != LEFT) {
        if (move_snake(+1, 0)) {
            dir = RIGHT;
        }
    }
}

bool Button(Rectangle rect, const char* text) { // 简单的按钮函数，返回是否被点击
    bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
    bool click = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    DrawRectangleRec(rect, hover ? GRAY : LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2, BLACK);
    int tw = MeasureText(text, 24);
    DrawText(text, rect.x + rect.width / 2 - tw / 2, rect.y + 15, 24, BLACK);
    return click;
}

void UpdateMainMenu() {   
    if (Button({ 500, 300, 200, 50 }, "Start Game")) {
        currentState = state_level_select; // 点击开始游戏按钮，进入关卡选择状态
    }
}

void DrawMainMenu() {
    DrawFPS(10, 10); // 显示帧率
    DrawText("Greedy Apple Snake", 400, 200, 48, DARKGREEN);
}

void UpdateLevelSelect() {
    if (Button({ 500, 300, 200, 50 }, "Level 1")) {
        current_level = levels[0];                         // 选择第一关
		currentLevel = 0;                                  // 更新当前关卡索引
        init_snake(snake_initial_positions[0]);            // 初始化蛇身
        currentState = state_gameplay;                     // 进入游戏进行状态
    }
    if (Button({ 500, 360, 200, 50 }, "Level 2")) {
        current_level = levels[1];                        // 选择第二关
		currentLevel = 1;                                 // 更新当前关卡索引
        init_snake(snake_initial_positions [1]);          // 初始化蛇身
        currentState = state_gameplay;                    // 进入游戏进行状态
    }
}

void DrawLevelSelect() {
    DrawFPS(10, 10); // 显示帧率
    DrawText("Select Level", 450, 200, 48, DARKGREEN);
}

void UpdateGameplay(){ // 重开按钮逻辑（点击就重置）
    if (Button({ 1050, 20, 120, 40 }, "Restart")) {
        init_snake(snake_initial_positions[currentLevel]);
        current_level = levels[currentLevel]; // 重置地图
        snake_ispoisoned = false;
        isDropping = false;
		snake_body_history.clear(); //清空断掉的蛇身历史记录
		body_history_dropping.clear();
		body_history_drop_timer.clear();
    }

    // 修复：断尾独立自动掉落！不管蛇动不动、掉不掉，断尾自己每帧检查下落
    update_body_history_drop();

    if (!isDropping) {
        player_move_snake(); // 处理玩家输入，移动蛇
    }
	update_drop(); // 处理蛇的掉落逻辑
}

void DrawGameplay() {
    DrawFPS(10, 10); // 显示帧率
    draw_level(current_level); // 绘制当前关卡
    draw_snake(); // 绘制蛇
	draw_snake_history(); // 绘制断掉的蛇身历史记录
    
}

void UpdateGameOver() {
    if (Button({ 500, 300, 200, 50 }, "Restart")) {
		snake_body_history.clear(); //清空断掉的蛇身历史记录
        snake_body.clear(); //蛇完全掉出地图了，清空蛇身
        snake_ispoisoned = false; //蛇不再中毒了
        isDropping = false; //掉落结束，允许再次调用
		init_snake(snake_initial_positions[currentLevel]); //重新初始化蛇身
		current_level = levels[currentLevel]; // 重置地图
        currentState = state_gameplay; // 点击重启按钮，回到主菜单
    }
	if (Button({ 500, 700, 200, 50 }, "Exit")) {
        CloseWindow(); // 点击退出按钮，关闭窗口
    }
	if (currentLevel >= levels.size() - 1) return; // 如果已经是最后一关了，就不显示下一关按钮了
	if (Button({ 500, 400, 200, 50 }, "Next Level")) {
        
        currentLevel++; // 点击下一关按钮，进入下一关
        if (currentLevel >= levels.size()) {
            currentLevel = 0; // 如果没有下一关了，就回到第一关
            
			DrawText("This is the last level.", 300, 200, 32, GREEN);
			if (Button({ 500, 600, 200, 50 }, "Back to Main Menu")) {
                currentState = state_main_menu; // 点击返回主菜单按钮，回到主菜单
            }
        }
        else {
            switch (currentLevel) {
            case 0: init_snake(snake_initial_positions[0]); break; // 初始化第一关蛇身
            case 1: init_snake(snake_initial_positions[1]); break; // 初始化第二关蛇身
            }
			snake_body_history.clear(); //清空断掉的蛇身历史记录
            current_level = levels[currentLevel]; // 更新当前关卡数据
            currentState = state_gameplay; // 进入游戏进行状态
        }
    }
}

void DrawGameOver() {
    DrawFPS(10, 10); // 显示帧率
    if (currentLevel >= levels.size()-1 && !snake_isdead) {
        DrawText("This is the last level.", 300, 200, 32, GREEN);
        if (Button({ 500, 500, 200, 50 }, "Back to Main Menu")) {
            currentState = state_main_menu; // 点击返回主菜单按钮，回到主菜单
        }
    }
    else {
        DrawText("Game Over", 450, 200, 48, RED);
    }
}

int main()
{
    // 初始化窗口
    InitWindow(1200, 800, "greedy apple snake");
    //SetTargetFPS(120); // 限制帧率
	
	LoadAllTextures(); // 加载所有纹理

    // 主循环
    while (!WindowShouldClose()) {
        // 更新逻辑
        switch (currentState) {
        case state_main_menu: UpdateMainMenu(); break;
        case state_level_select: UpdateLevelSelect(); break;
        case state_gameplay: UpdateGameplay(); break;
		case state_game_over: UpdateGameOver(); break;
        }
        // 绘制
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch (currentState) {
        case state_main_menu: DrawMainMenu(); break;
        case state_level_select: DrawLevelSelect(); break;
        case state_gameplay: DrawGameplay(); break;
		case state_game_over: DrawGameOver(); break;
        }
        EndDrawing();
    }
	// 卸载纹理
	UnloadTexture(Headup);
	UnloadTexture(Headdown);
	UnloadTexture(Headleft);
	UnloadTexture(Headright);
	UnloadTexture(bodyhend);
	UnloadTexture(bodyver);
	UnloadTexture(bodyturn1);
	UnloadTexture(bodyturn2);
	UnloadTexture(bodyturn3);
	UnloadTexture(bodyturn4);
	UnloadTexture(tailup);
	UnloadTexture(taildown);
	UnloadTexture(tailleft);
	UnloadTexture(tailright);
    // 关闭窗口
    CloseWindow();
    return 0;
}
