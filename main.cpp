#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <iostream>
#include <vector>
#include <algorithm> // 添加这个头文件
#include <string>
using namespace std;

// ===================== 全局公共枚举 =====================
//有四个游戏状态：主菜单、关卡选择、游戏中、游戏结束
typedef enum {
    state_main_menu,
    state_level_select,
    state_gameplay,
    state_game_over,
	state_change_skin
} GameState;
//蛇的四个移动方向
typedef enum {
    UP, DOWN, LEFT, RIGHT
} Direction;

// 1. 定义皮肤ID
typedef enum {
    SKIN_DEFAULT = 0,   // 默认皮肤（你现在的）
    SKIN_1 = 1,       // 红色皮肤
    SKIN_2 = 2       // 蓝色皮肤
} SkinID;

class TextureResource
{
public:
    // ===================== 核心改动：单变量 → 皮肤数组 =====================
    // 蛇头（支持3套皮肤）
    Texture2D Headup[3], Headdown[3], Headleft[3], Headright[3];
    // 蛇身
    Texture2D bodyhend[3], bodyver[3];
    Texture2D bodyturn1[3], bodyturn2[3], bodyturn3[3], bodyturn4[3];
    // 蛇尾
    Texture2D tailup[3], taildown[3], tailleft[3], tailright[3];
    // 道具（可选择是否换皮，这里默认通用）
    Texture2D wall, food, poison, door, trap;

    // 当前使用的皮肤ID（默认0：默认皮肤）
    int currentSkin = SKIN_DEFAULT;
    // 总皮肤数量
    const int MAX_SKINS = 3;

    // ===================== 加载所有皮肤纹理 =====================
    void LoadAll()
    {
        // 循环加载每一套皮肤
        for (int skin = 0; skin < MAX_SKINS; skin++) {
            // 拼接路径：pictures/skin0/xxx.png
            char path[128];

            // 加载蛇头
            sprintf(path, "pictures\\skin%d\\headup.png", skin);
            Headup[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headdown.png", skin);
            Headdown[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headleft.png", skin);
            Headleft[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headright.png", skin);
            Headright[skin] = LoadTexture(path);

            // 加载蛇身
            sprintf(path, "pictures\\skin%d\\bodyhend.png", skin);
            bodyhend[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\bodyver.png", skin);
            bodyver[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\bodyturn1.png", skin);
            bodyturn1[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\bodyturn2.png", skin);
            bodyturn2[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\bodyturn3.png", skin);
            bodyturn3[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\bodyturn4.png", skin);
            bodyturn4[skin] = LoadTexture(path);

            // 加载蛇尾
            sprintf(path, "pictures\\skin%d\\tailup.png", skin);
            tailup[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\taildown.png", skin);
            taildown[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\tailleft.png", skin);
            tailleft[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\tailright.png", skin);
            tailright[skin] = LoadTexture(path);
        }

        // 道具通用，只加载一次
        wall = LoadTexture("pictures\\wall.png");
        food = LoadTexture("pictures\\food.png");
        poison = LoadTexture("pictures\\poison.png");
        door = LoadTexture("pictures\\door.png");
        trap = LoadTexture("pictures\\trap.png");
    }

    // ===================== 卸载所有皮肤 =====================
    void UnloadAll()
    {
        for (int skin = 0; skin < MAX_SKINS; skin++) {
            UnloadTexture(Headup[skin]);
            UnloadTexture(Headdown[skin]);
            UnloadTexture(Headleft[skin]);
            UnloadTexture(Headright[skin]);
            UnloadTexture(bodyhend[skin]);
            UnloadTexture(bodyver[skin]);
            UnloadTexture(bodyturn1[skin]);
            UnloadTexture(bodyturn2[skin]);
            UnloadTexture(bodyturn3[skin]);
            UnloadTexture(bodyturn4[skin]);
            UnloadTexture(tailup[skin]);
            UnloadTexture(taildown[skin]);
            UnloadTexture(tailleft[skin]);
            UnloadTexture(tailright[skin]);
        }
        UnloadTexture(wall);
        UnloadTexture(food);
        UnloadTexture(poison);
        UnloadTexture(door);
        UnloadTexture(trap);
    }

    // ===================== 【核心功能】切换皮肤 =====================
    void SwitchSkin(int newSkinId)
    {
        // 合法性校验
        if (newSkinId >= 0 && newSkinId < MAX_SKINS) {
            currentSkin = newSkinId;
            printf("切换皮肤成功：%d\n", currentSkin);
        }
    }
};

// ===================== 2. 蛇实体类（只管蛇所有逻辑：移动/掉落/中毒/断尾/绘制） =====================
class Snake
{
private:
    // 基础属性
    vector<Vector2> body;
    Direction dir;
    bool isDropping;
    bool isPoisoned;
    bool isDead;

    // 断尾数据
    vector<vector<Vector2>> breakBodyHistory;
    vector<bool> breakDropState;
    vector<float> breakDropTimer;

    // 常量
    const float DROP_SPEED = 0.1f;
    float dropTimer = 0.0f;

    // 依赖资源
    TextureResource* texRes;

public:
    Snake(TextureResource* res) : texRes(res) {}

    // 初始化蛇
    void Init(vector<Vector2> initPos)
    {
        body.clear();
        body = initPos;
        dir = RIGHT;
        isDropping = false;
        isPoisoned = false;
        isDead = false;
        breakBodyHistory.clear();
        breakDropState.clear();
        breakDropTimer.clear();
        dropTimer = 0.0f;
    }

    // 获取蛇头坐标
    Vector2 GetHeadPos() { return body[0]; }
    // 判断坐标是否是蛇身
    bool IsInBody(int x, int y)
    {
        for (auto& seg : body)
            if (seg.x == x && seg.y == y) return true;
        return false;
    }
    // 判断坐标是否是断掉的蛇尾
    bool IsInBreakBody(int x, int y)
    {
        for (auto& piece : breakBodyHistory)
            for (auto& seg : piece)
                if (seg.x == x && seg.y == y) return true;
        return false;
    }
    void SwapSnakeHeadTail()
    {
        if (body.size() < 2) return;
        std::reverse(body.begin(), body.end());
		if (body[0].x < body[1].x) dir = LEFT;
		else if (body[0].x > body[1].x) dir = RIGHT;
		else if (body[0].y < body[1].y) dir = UP;
		else dir = DOWN;
    }
    // 掉落支撑判断-蛇本体
    bool SnakeHasSupport(int x, int y, class GameMap* map);
    // 掉落支撑判断-断尾
    bool BreakHasSupport(int x, int y, int ignoreIdx, class GameMap* map);

    // 检测蛇是否需要整体掉落
    bool CheckNeedDrop(class GameMap* map);
    // 蛇整体下落一格
    void AllDrop() { for (auto& seg : body) seg.y += 1; }
    // 判断蛇是否完全掉出地图
    bool IsOutMap()
    {
        for (auto& seg : body)
            if (seg.y >= 19) return true;
        return false;
    }
    // 执行蛇整体掉落逻辑
    void UpdateSnakeDrop(class GameMap* map);

    // 断尾单块下落
    void DropSingleBreak(int idx, class GameMap* map);
    // 检测单块断尾是否开始掉落
    void CheckBreakDrop(int idx, class GameMap* map);
    // 更新所有断尾自动下落
    void UpdateAllBreakDrop(class GameMap* map);

    // 推箱子：检测断尾能否被推动
    bool CanPushBreak(int idx, int dx, int dy, class GameMap* map);
    // 递归推动相连断尾
    void PushBreakBlock(int idx, int dx, int dy, class GameMap* map);
    // 外部调用推动断尾
    void PushBreak(int dx, int dy, int tarX, int tarY, class GameMap* map);

    // 蛇移动核心逻辑
    bool Move(int dx, int dy, class GameMap* map);
    // 玩家方向按键控制
    void PlayerControl(class GameMap* map);

    // 绘制蛇本体
    void Draw(int cellSize);
    // 绘制所有断掉的蛇尾
    void DrawBreakBody(int cellSize);

    // 状态读写
    bool GetIsDropping() { return isDropping; }
    void SetDropping(bool b) { isDropping = b; }
    bool GetIsDead() { return isDead; }
    void SetDead(bool b) { isDead = b; }
    bool GetIsPoisoned() { return isPoisoned; }
    void SetPoisoned(bool b) { isPoisoned = b; }
    Direction GetDir() { return dir; }
    void SetDir(Direction d) { dir = d; }
};

// ===================== 3. 地图关卡类（只管地图/关卡/碰撞/食物毒药终点） =====================
class GameMap
{
private:
    vector<vector<vector<int>>> levelDatas;
    vector<vector<int>> curLevelData;
    vector<vector<Vector2>> snakeInitPos;
    int curLevelIdx;
    const int cellSize = 40;

public:
    GameMap()
    {
        // 原版完整关卡数据原样写入
        levelDatas = {
            {
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,0,3,3,0,0,0,4,0,0,0,0,0,0},
                {0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            },
            {
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,1,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,4,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            }
        };
        // 蛇初始位置
        snakeInitPos = {
            {{6,10},{5,10},{4,10}},
            {{6,10},{5,10},{4,10}}
        };
        curLevelIdx = 0;
        curLevelData = levelDatas[0];
    }

    // 切换关卡
    void SwitchLevel(int idx)
    {
        curLevelIdx = idx;
        curLevelData = levelDatas[idx];
    }
    // 获取当前关卡索引
    int GetCurLevel() { return curLevelIdx; }
    // 获取关卡总数
    int GetLevelCount() { return levelDatas.size(); }
    // 获取当前关卡地图数据
    vector<vector<int>>& GetCurLevelData() { return curLevelData; }
    // 获取对应关卡蛇初始坐标
    vector<Vector2> GetSnakeInitPos(int idx) { return snakeInitPos[idx]; }
    // 清空格子内容
    void ClearCell(int x, int y) { curLevelData[y][x] = 0; }

    // 碰撞判断：是否墙壁/出界
    bool IsWall(int x, int y)
    {
        if(y < 0 || y >= 20 || x < 0 || x >=30) return true;
        return curLevelData[y][x] == 1;
    }
    // 是否食物/毒药
    bool IsFoodOrPoison(int x, int y)
    {
        if(y<0||y>=20||x<0||x>=30) return false;
        return curLevelData[y][x]==2 || curLevelData[y][x]==3;
    }
    // 是否毒药
    bool IsPoison(int x, int y)
    {
        if(y<0||y>=20||x<0||x>=30) return false;
        return curLevelData[y][x]==3;
    }
    // 是否终点
    bool IsDoor(int x, int y)
    {
        if(y<0||y>=20||x<0||x>=30) return false;
        return curLevelData[y][x]==4;
    }

    // 绘制整张地图
    void DrawMap(TextureResource& tex)
    {
        for (int y = 0; y < curLevelData.size(); y++)
        {
            for (int x = 0; x < curLevelData[y].size(); x++)
            {
                int px = x * cellSize;
                int py = y * cellSize;
                if(curLevelData[y][x]==1) DrawTexture(tex.wall,px,py,WHITE);
                else if(curLevelData[y][x]==2) DrawTexture(tex.food,px,py,WHITE);
                else if(curLevelData[y][x]==3) DrawTexture(tex.poison,px,py,WHITE);
                else if(curLevelData[y][x]==4) DrawTexture(tex.door,px,py,WHITE);
                else if(curLevelData[y][x]==5) DrawTexture(tex.trap,px,py,WHITE);
            }
        }
    }
    int GetCellSize() { return cellSize; }
};

// ===================== 4. UI工具类（按钮+菜单文字） =====================
class GameUI
{
public:
    // 绘制按钮，返回是否点击
    bool DrawButton(Rectangle rect, const char* text)
    {
        bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
        bool click = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        DrawRectangleRec(rect, hover ? GRAY : LIGHTGRAY);
        DrawRectangleLinesEx(rect,2,BLACK);
        int tw = MeasureText(text,24);
        DrawText(text, rect.x+rect.width/2-tw/2, rect.y+15,24,BLACK);
        return click;
    }

    void DrawTextTip(const char* str, int x, int y, int fontSize, Color color)
    {
        DrawText(str,x,y,fontSize,color);
    }
};

// ===================== 蛇类成员函数实现（全部原版逻辑） =====================
bool Snake::SnakeHasSupport(int x, int y, GameMap* map)
{
    int nx = x;
    int ny = y+1;
    if(map->IsWall(nx,ny) || map->IsFoodOrPoison(nx,ny)) return true;
    for(auto& piece : breakBodyHistory)
        for(auto& seg : piece)
            if(seg.x==nx&&seg.y==ny) return true;
    return false;
}

bool Snake::BreakHasSupport(int x, int y, int ignoreIdx, GameMap* map)
{
    int nx = x;
    int ny = y+1;
    if(map->IsWall(nx,ny) || map->IsFoodOrPoison(nx,ny)) return true;
    if(IsInBody(nx,ny)) return true;
    for(int i=0;i<(int)breakBodyHistory.size();i++)
    {
        if(i==ignoreIdx) continue;
        for(auto& seg : breakBodyHistory[i])
            if(seg.x==nx&&seg.y==ny) return true;
    }
    return false;
}

bool Snake::CheckNeedDrop(GameMap* map)
{
    for(auto& seg : body)
        if(SnakeHasSupport(seg.x, seg.y, map)) return true;
    return false;
}

void Snake::UpdateSnakeDrop(GameMap* map)
{
    if(!isDropping) return;
    dropTimer += GetFrameTime();
    if(dropTimer >= DROP_SPEED)
    {
        AllDrop();
        dropTimer = 0;
        if(IsOutMap())
        {
            body.clear();
            isPoisoned = false;
            isDropping = false;
            isDead = true;
            return;
        }
        if(CheckNeedDrop(map)) isDropping = false;
    }
}

void Snake::DropSingleBreak(int idx, GameMap* map)
{
    if(idx<0 || idx>=(int)breakBodyHistory.size()) return;
    bool canDrop = true;
    for(auto& seg : breakBodyHistory[idx])
    {
        if(BreakHasSupport(seg.x,seg.y,idx,map))
        {
            canDrop = false;
            break;
        }
    }
    if(!canDrop)
    {
        breakDropState[idx] = false;
        return;
    }
    for(auto& seg : breakBodyHistory[idx]) seg.y +=1;
}

void Snake::CheckBreakDrop(int idx, GameMap* map)
{
    if(idx<0||idx>=(int)breakBodyHistory.size()) return;
    if(breakDropState[idx]) return;
    bool hasSup = false;
    for(auto& seg : breakBodyHistory[idx])
    {
        if(BreakHasSupport(seg.x,seg.y,idx,map))
        {
            hasSup = true;
            break;
        }
    }
    breakDropState[idx] = !hasSup;
}

void Snake::UpdateAllBreakDrop(GameMap* map)
{
    for(int i=0;i<(int)breakBodyHistory.size();i++)
    {
        if(i>=(int)breakDropState.size()) breakDropState.push_back(false);
        if(i>=(int)breakDropTimer.size()) breakDropTimer.push_back(0);
        if(!breakDropState[i])
        {
            CheckBreakDrop(i,map);
            continue;
        }
        breakDropTimer[i] += GetFrameTime();
        if(breakDropTimer[i]>=DROP_SPEED)
        {
            DropSingleBreak(i,map);
            breakDropTimer[i] = 0;
            bool out = false;
            for(auto& seg : breakBodyHistory[i])
                if(seg.y>=19) {out=true;break;}
            if(out)
            {
                breakBodyHistory.erase(breakBodyHistory.begin()+i);
                breakDropState.erase(breakDropState.begin()+i);
                breakDropTimer.erase(breakDropTimer.begin()+i);
                i--;
            }
        }
    }
}

bool Snake::CanPushBreak(int idx, int dx, int dy, GameMap* map)
{
    if(idx<0||idx>=(int)breakBodyHistory.size()) return true;
    for(auto& seg : breakBodyHistory[idx])
    {
        int nx = seg.x+dx;
        int ny = seg.y+dy;
        if(map->IsWall(nx,ny) || map->IsFoodOrPoison(nx,ny)) return false;
        for(int i=0;i<(int)breakBodyHistory.size();i++)
        {
            if(i==idx) continue;
            for(auto& s : breakBodyHistory[i])
            {
                if(s.x==nx&&s.y==ny)
                    if(!CanPushBreak(i,dx,dy,map)) return false;
            }
        }
    }
    return true;
}

void Snake::PushBreakBlock(int idx, int dx, int dy, GameMap* map)
{
    if(idx<0||idx>=(int)breakBodyHistory.size()) return;
    for(int i=0;i<(int)breakBodyHistory.size();i++)
    {
        if(i==idx) continue;
        for(auto& s : breakBodyHistory[i])
        {
            for(auto& seg : breakBodyHistory[idx])
            {
                if(s.x == seg.x+dx && s.y == seg.y+dy)
                    PushBreakBlock(i,dx,dy,map);
            }
        }
    }
    for(auto& seg : breakBodyHistory[idx])
    {
        seg.x += dx;
        seg.y += dy;
    }
    CheckBreakDrop(idx,map);
}

void Snake::PushBreak(int dx, int dy, int tarX, int tarY, GameMap* map)
{
    int tarIdx = -1;
    for(int i=0;i<(int)breakBodyHistory.size();i++)
    {
        for(auto& seg : breakBodyHistory[i])
        {
            if(seg.x==tarX&&seg.y==tarY)
            {
                tarIdx = i;
                break;
            }
        }
        if(tarIdx!=-1) break;
    }
    if(tarIdx==-1) return;
    if(CanPushBreak(tarIdx,dx,dy,map))
        PushBreakBlock(tarIdx,dx,dy,map);
    for(int i=0;i<(int)breakBodyHistory.size();i++)
        CheckBreakDrop(i,map);
}

bool Snake::Move(int dx, int dy, GameMap* map)
{
    if(isDropping) return false;
    Vector2 head = GetHeadPos();
    Vector2 newHead = {head.x+dx, head.y+dy};
    if(map->IsWall((int)newHead.x,(int)newHead.y)) return false;
    if(IsInBody((int)newHead.x,(int)newHead.y)) return false;

    if(IsInBreakBody((int)newHead.x,(int)newHead.y))
        PushBreak(dx,dy,(int)newHead.x,(int)newHead.y,map);
    if(IsInBreakBody((int)newHead.x,(int)newHead.y)) return false;

    // 中毒断尾
    if(isPoisoned)
    {
        int half = (body.size()+1)/2;
        vector<Vector2> cut(body.begin()+half, body.end());
        breakBodyHistory.push_back(cut);
        breakDropState.push_back(false);
        breakDropTimer.push_back(0);
        int len = body.size();
        for(int i=half;i<len;i++) body.pop_back();
        isPoisoned = false;
    }

    if(map->IsPoison((int)newHead.x,(int)newHead.y))
    {
        body.insert(body.begin(),newHead);
        body.pop_back();
        map->ClearCell((int)newHead.x,(int)newHead.y);
        isPoisoned = true;
    }
    else if(map->IsDoor((int)newHead.x,(int)newHead.y))
    {
        return true;
    }
    else if(map->IsFoodOrPoison((int)newHead.x,(int)newHead.y))
    {
        body.insert(body.begin(),newHead);
        map->ClearCell((int)newHead.x,(int)newHead.y);
    }
    else
    {
        body.insert(body.begin(),newHead);
        body.pop_back();
    }

    if(!CheckNeedDrop(map)) isDropping = true;
    return true;
}

void Snake::PlayerControl(GameMap* map)
{
    if(isDropping) return;
    if(IsKeyPressed(KEY_UP) && dir != DOWN)
    {
        if(Move(0,-1,map)) dir = UP;
    }
    else if(IsKeyPressed(KEY_DOWN) && dir != UP)
    {
        if(Move(0,1,map)) dir = DOWN;
    }
    else if(IsKeyPressed(KEY_LEFT) && dir != RIGHT)
    {
        if(Move(-1,0,map)) dir = LEFT;
    }
    else if(IsKeyPressed(KEY_RIGHT) && dir != LEFT)
    {
        if(Move(1,0,map)) dir = RIGHT;
    }
}

void Snake::Draw(int cellSize)
{
    if (body.empty()) return;
    for (int i = 0; i < (int)body.size(); i++)
    {
        Vector2 seg = body[i];
        float x = seg.x * cellSize;
        float y = seg.y * cellSize;
        if (i == 0)
        {
            // 蛇头：添加皮肤索引 [texRes->currentSkin]
            if (dir == UP) DrawTexture(texRes->Headup[texRes->currentSkin], x, y, WHITE);
            else if (dir == DOWN) DrawTexture(texRes->Headdown[texRes->currentSkin], x, y, WHITE);
            else if (dir == LEFT) DrawTexture(texRes->Headleft[texRes->currentSkin], x, y, WHITE);
            else DrawTexture(texRes->Headright[texRes->currentSkin], x, y, WHITE);
        }
        else if (i == (int)body.size() - 1)
        {
            Vector2 prev = body[i - 1];
            // 蛇尾：添加皮肤索引 [texRes->currentSkin]
            if (seg.x == prev.x)
            {
                if (seg.y < prev.y) DrawTexture(texRes->tailup[texRes->currentSkin], x, y, WHITE);
                else DrawTexture(texRes->taildown[texRes->currentSkin], x, y, WHITE);
            }
            else
            {
                if (seg.x < prev.x) DrawTexture(texRes->tailleft[texRes->currentSkin], x, y, WHITE);
                else DrawTexture(texRes->tailright[texRes->currentSkin], x, y, WHITE);
            }
        }
        else
        {
            Vector2 prev = body[i - 1];
            Vector2 next = body[i + 1];
            // 蛇身：添加皮肤索引 [texRes->currentSkin]
            if (prev.x == next.x)
                DrawTexture(texRes->bodyver[texRes->currentSkin], x, y, WHITE);
            else if (prev.y == next.y)
                DrawTexture(texRes->bodyhend[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x < seg.x && next.y < seg.y) || (next.x < seg.x && prev.y < seg.y))
                DrawTexture(texRes->bodyturn1[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x > seg.x && next.y < seg.y) || (next.x > seg.x && prev.y < seg.y))
                DrawTexture(texRes->bodyturn2[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x<seg.x && next.y>seg.y) || (next.x<seg.x && prev.y>seg.y))
                DrawTexture(texRes->bodyturn3[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x > seg.x && next.y > seg.y) || (next.x > seg.x && prev.y > seg.y))
                DrawTexture(texRes->bodyturn4[texRes->currentSkin], x, y, WHITE);
        }
    }
}

void Snake::DrawBreakBody(int cellSize)
{
    // 断尾渲染：同样添加皮肤索引
    for (auto& piece : breakBodyHistory)
        for (auto& seg : piece)
            DrawTexture(texRes->bodyver[texRes->currentSkin], seg.x * cellSize, seg.y * cellSize, WHITE);
}

// ===================== 5. 游戏主控类（统筹所有模块+主循环） =====================
class SnakeGameMain
{
private:
    GameState gameState;
    TextureResource texRes;
    GameMap gameMap;
    Snake snake;
    GameUI ui;

public:
    SnakeGameMain() : snake(&texRes)
    {
        InitWindow(1200,800,"greedy apple snake");
        gameState = state_main_menu;
        texRes.LoadAll();
    }

    ~SnakeGameMain()
    {
        texRes.UnloadAll();
        CloseWindow();
    }

    void Run()
    {
        while(!WindowShouldClose())
        {
            UpdateLogic();
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawRender();
            EndDrawing();
        }
    }

    void UpdateLogic()
    {
        switch(gameState)
        {
            case state_main_menu: UpdateMainMenu(); break;
            case state_level_select: UpdateLevelSelect(); break;
            case state_gameplay: UpdateGamePlay(); break;
            case state_game_over: UpdateGameOver(); break;
			case state_change_skin: UpdateChangeSkin(); break;
        }
    }

    void DrawRender()
    {
        switch(gameState)
        {
            case state_main_menu: DrawMainMenu(); break;
            case state_level_select: DrawLevelSelect(); break;
            case state_gameplay: DrawGamePlay(); break;
            case state_game_over: DrawGameOver(); break;
			case state_change_skin: DrawChangeSkin(); break;
        }
    }

    // 主菜单逻辑
    void UpdateMainMenu()
    {
		// 开始游戏
        if (ui.DrawButton({ 500,300,200,50 }, "Start Game")) {
            gameState = state_level_select;
        }
		// 换皮肤
        if (ui.DrawButton({ 480,360,240,50 }, "Change Skin"))
        {
            gameState = state_change_skin;
        }
		// 退出游戏
		if (ui.DrawButton({ 500,420,200,50 }, "Exit Game")) {
			CloseWindow();
		}
    }

	// 主菜单绘制
    void DrawMainMenu()
    {
        DrawFPS(10,10);
        ui.DrawTextTip("Greedy Apple Snake",400,200,48,DARKGREEN);
    }

    // 关卡选择
    void UpdateLevelSelect()
    {
        if(ui.DrawButton({500,300,200,50},"Level 1"))
        {
			gameMap.SwitchLevel(0);  // 切换到第1关
			snake.Init(gameMap.GetSnakeInitPos(0));// 初始化蛇位置
            gameState = state_gameplay;
        }
        if(ui.DrawButton({500,360,200,50},"Level 2"))
        {
            gameMap.SwitchLevel(1);
            snake.Init(gameMap.GetSnakeInitPos(1));
            gameState = state_gameplay;
        }
		// 回到主菜单
        if (ui.DrawButton({ 480,420,240,50 }, "Back to Main Menu")) {
            gameState = state_main_menu;
        }

    }
    void DrawLevelSelect()
    {
        DrawFPS(10,10);
        ui.DrawTextTip("Select Level",450,200,48,DARKGREEN);
    }

    // 游戏进行中
    void UpdateGamePlay()
    {
        // 重启按钮
        if(ui.DrawButton({1050,20,120,40},"Restart"))
        {
            int cur = gameMap.GetCurLevel();
            gameMap.SwitchLevel(cur);
            snake.Init(gameMap.GetSnakeInitPos(cur));
        }
        if (ui.DrawButton({ 1000,70,200,40 }, "change head")) {
            snake.SwapSnakeHeadTail();
        }
		//回到主菜单
		if (ui.DrawButton({ 960,120,240,40 }, "Back to Main Menu")) {
			// 重置游戏状态
			int cur = gameMap.GetCurLevel();
			gameMap.SwitchLevel(cur);
			snake.Init(gameMap.GetSnakeInitPos(cur));
			gameState = state_main_menu;
		}
        // 断尾自动下落
        snake.UpdateAllBreakDrop(&gameMap);
        // 蛇整体掉落
        snake.UpdateSnakeDrop(&gameMap);
        // 玩家操控
        snake.PlayerControl(&gameMap);

        // 到达终点跳转结束界面
        Vector2 head = snake.GetHeadPos();
        if(gameMap.IsDoor((int)head.x,(int)head.y))
            gameState = state_game_over;
        // 蛇死亡跳转
        if(snake.GetIsDead())
            gameState = state_game_over;
    }
    void DrawGamePlay()
    {
        DrawFPS(10,10);
        gameMap.DrawMap(texRes);
        snake.Draw(gameMap.GetCellSize());
        snake.DrawBreakBody(gameMap.GetCellSize());
    }

    // 游戏结束界面
    void UpdateGameOver()
    {
        int curLv = gameMap.GetCurLevel();
        // 重启
        if(ui.DrawButton({500,300,200,50},"Restart"))
        {
            gameMap.SwitchLevel(curLv);
            snake.Init(gameMap.GetSnakeInitPos(curLv));
            gameState = state_gameplay;
        }
        // 退出
        if(ui.DrawButton({500,700,200,50},"Exit"))
            CloseWindow();
        // 下一关
        if(curLv >= gameMap.GetLevelCount()-1) return;
        if(ui.DrawButton({500,400,200,50},"Next Level"))
        {
            curLv++;
            if(curLv >= gameMap.GetLevelCount()) curLv=0;
            gameMap.SwitchLevel(curLv);
            snake.Init(gameMap.GetSnakeInitPos(curLv));
            gameState = state_gameplay;
        }
    }
    void DrawGameOver()
    {
        DrawFPS(10,10);
        int cur = gameMap.GetCurLevel();
        if(cur >= gameMap.GetLevelCount()-1 && !snake.GetIsDead())
        {
            ui.DrawTextTip("This is the last level.",300,200,32,GREEN);
            if(ui.DrawButton({500,500,200,50},"Back to Main Menu"))
                gameState = state_main_menu;
        }
        else
        {
            ui.DrawTextTip("Game Over",450,200,48,RED);
        }
    }
	// 换皮肤界面
	void UpdateChangeSkin()
	{
		ui.DrawTextTip("Select Snake Skin", 400, 200, 48, DARKGREEN);
		for (int i = 0; i < texRes.MAX_SKINS; i++)
		{
			if (ui.DrawButton({ 500,300.0f + i * 60,200,50 }, (string("Skin ") + to_string(i + 1)).c_str()))
			{
				texRes.currentSkin = i;
				gameState = state_main_menu;
			}
		}
		if (ui.DrawButton({ 480,300.0f + texRes.MAX_SKINS * 60,240,50 }, "Back to Main Menu")) {
			gameState = state_main_menu;
		}
	}
	// 换皮肤界面绘制
    void DrawChangeSkin()
    {
        DrawFPS(10, 10);
        ui.DrawTextTip("Select Snake Skin", 400, 200, 48, DARKGREEN);
        // 只绘制按钮，不判断点击！
        for (int i = 0; i < texRes.MAX_SKINS; i++)
        {   
            ui.DrawButton({ 500.0f, 300.0f + i * 60.0f, 200.0f, 50.0f }, (string("Skin ") + to_string(i + 1)).c_str());
			//给每个按钮添加预览图,一共三格，分别是蛇头、蛇身、蛇尾，预览图显示一条长三格且向右的蛇，且根据皮肤索引切换
			DrawTexture(texRes.Headright[i], 830, 300.0f + i * 60.0f, WHITE);
			DrawTexture(texRes.bodyhend[i], 790, 300.0f + i * 60.0f, WHITE);
			DrawTexture(texRes.tailleft[i], 750, 300.0f + i * 60.0f, WHITE);
        }
        ui.DrawButton({ 480.0f, 300.0f + texRes.MAX_SKINS * 60.0f, 240.0f, 50.0f }, "Back to Main Menu");
    }
};

// ===================== 主函数（极度简洁） =====================
int main()
{
    SnakeGameMain game;
    game.Run();
    return 0;
}