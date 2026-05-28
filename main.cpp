// 禁用VS编译器对不安全C标准库函数的警告（如sprintf、strcpy等）
#define _CRT_SECURE_NO_WARNINGS

#include "raylib.h"
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include "mysql_db.h"
using namespace std;

//=================================== 全局枚举定义 ===================================
/**
 游戏状态枚举
 控制游戏当前处于哪个界面，是游戏流程的核心状态机
 */
typedef enum {
    state_login,        // 登录界面
    state_register,     // 注册界面
    state_main_menu,    // 主菜单界面
    state_level_select, // 关卡选择界面
    state_gameplay,     // 游戏进行中界面
    state_game_over,    // 游戏结束/通关界面
    state_change_skin   // 皮肤更换界面
} GameState;

/**
 蛇的移动方向枚举
 定义蛇头可以朝向的四个方向
 */
typedef enum {
    UP,    // 向上
    DOWN,  // 向下
    LEFT,  // 向左
    RIGHT  // 向右
} Direction;

/**
  皮肤ID枚举
 定义游戏中可用的蛇皮肤编号
 */
typedef enum {
    SKIN_DEFAULT = 0, // 默认皮肤
    SKIN_1 = 1,       // 皮肤1
    SKIN_2 = 2        // 皮肤2
} SkinID;

//=================================== 皮肤资源管理类 ===================================
/**
 纹理资源管理类
 负责加载、卸载、切换所有游戏纹理资源，包括蛇的各个部位和地图元素
 */
class TextureResource
{
public:
    // 蛇头纹理数组，索引对应皮肤ID
    Texture2D Headup[3], Headdown[3], Headleft[3], Headright[3];
    // 蛇身体纹理数组，索引对应皮肤ID（水平、垂直、转弯四种）
    Texture2D bodyhend[3], bodyver[3];
    Texture2D bodyturn1[3], bodyturn2[3], bodyturn3[3], bodyturn4[3];
    // 蛇尾巴纹理数组，索引对应皮肤ID
    Texture2D tailup[3], taildown[3], tailleft[3], tailright[3];
    // 蛇断裂身体纹理数组，索引对应皮肤ID
    Texture2D dockleft[3], dockright[3], dockup[3], dockdown[3], dock1[3];
    // 地图元素纹理
    Texture2D wall, food, poison, door, trap;

    int currentSkin = SKIN_DEFAULT; // 当前使用的皮肤ID
    const int MAX_SKINS = 3;        // 游戏支持的最大皮肤数量

    /**
     * @brief 加载所有游戏纹理资源
     * 从pictures目录下按皮肤编号加载所有蛇皮肤和地图元素纹理
     */
    void LoadAll()
    {
        // 循环加载所有皮肤的纹理
        for (int skin = 0; skin < MAX_SKINS; skin++) {
            char path[128];
            // 加载蛇头各个方向的纹理
            sprintf(path, "pictures\\skin%d\\headup.png", skin);
            Headup[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headdown.png", skin);
            Headdown[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headleft.png", skin);
            Headleft[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\headright.png", skin);
            Headright[skin] = LoadTexture(path);

            // 加载蛇身体纹理
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

            // 加载蛇断裂身体纹理
            sprintf(path, "pictures\\skin%d\\dock1.png", skin);
            dock1[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\dockup.png", skin);
            dockup[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\dockdown.png", skin);
            dockdown[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\dockleft.png", skin);
            dockleft[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\dockright.png", skin);
            dockright[skin] = LoadTexture(path);

            // 加载蛇尾巴纹理
            sprintf(path, "pictures\\skin%d\\tailup.png", skin);
            tailup[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\taildown.png", skin);
            taildown[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\tailleft.png", skin);
            tailleft[skin] = LoadTexture(path);
            sprintf(path, "pictures\\skin%d\\tailright.png", skin);
            tailright[skin] = LoadTexture(path);
        }
        // 加载地图元素纹理
        wall = LoadTexture("pictures\\wall.png");
        food = LoadTexture("pictures\\food.png");
        poison = LoadTexture("pictures\\poison.png");
        door = LoadTexture("pictures\\door.png");
        trap = LoadTexture("pictures\\trap.png");
    }

    /**
     * @brief 卸载所有游戏纹理资源
     * 游戏退出时调用，释放显卡内存
     */
    void UnloadAll()
    {
        // 循环卸载所有皮肤的纹理
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
            UnloadTexture(dock1[skin]);
            UnloadTexture(dockup[skin]);
            UnloadTexture(dockdown[skin]);
            UnloadTexture(dockleft[skin]);
            UnloadTexture(dockright[skin]);
        }
        // 卸载地图元素纹理
        UnloadTexture(wall);
        UnloadTexture(food);
        UnloadTexture(poison);
        UnloadTexture(door);
        UnloadTexture(trap);
    }

    /**
     * @brief 切换蛇的皮肤
     * @param newSkinId 要切换到的皮肤ID
     */
    void SwitchSkin(int newSkinId)
    {
        if (newSkinId >= 0 && newSkinId < MAX_SKINS) {
            currentSkin = newSkinId;
            printf("切换皮肤成功：%d\n", currentSkin);
        }
    }
};

//=================================== 蛇实体类 ===================================
/**
 * @brief 蛇实体类
 * 封装蛇的所有属性和行为，包括移动、碰撞检测、中毒、身体断裂、掉落等逻辑
 */
class Snake
{
private:
    vector<Vector2> body;               // 蛇的身体坐标数组，第一个元素是蛇头
    Direction dir;                      // 蛇当前的移动方向
    bool isDropping;                    // 蛇是否正在掉落
    bool isPoisoned;                    // 蛇是否中毒（下一次移动会断裂身体）
    bool isDead;                        // 蛇是否死亡
    vector<vector<Vector2>> breakBodyHistory; // 所有断裂的身体段数组
    vector<bool> breakDropState;        // 每个断裂身体段是否正在掉落
    vector<float> breakDropTimer;       // 每个断裂身体段的掉落计时器
    const float DROP_SPEED = 0.1f;      // 掉落速度（秒/格）
    float dropTimer = 0.0f;             // 蛇整体掉落计时器
    TextureResource* texRes;            // 指向纹理资源管理器的指针

public:
    /**
     * @brief 蛇类构造函数
     * @param res 纹理资源管理器指针
     */
    Snake(TextureResource* res) : texRes(res) {}

    /**
     * @brief 初始化蛇
     * @param initPos 蛇的初始身体坐标数组
     */
    void Init(vector<Vector2> initPos)
    {
        body.clear();
        body = initPos;
        dir = RIGHT; // 默认向右移动
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

    /**
     * @brief 检查坐标是否在蛇的身体上
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 坐标在身体上，false 不在
     */
    bool IsInBody(int x, int y)
    {
        for (auto& seg : body)
            if (seg.x == x && seg.y == y) return true;
        return false;
    }

    /**
     * @brief 检查坐标是否在断裂的身体上
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 坐标在断裂身体上，false 不在
     */
    bool IsInBreakBody(int x, int y)
    {
        for (auto& piece : breakBodyHistory)
            for (auto& seg : piece)
                if (seg.x == x && seg.y == y) return true;
        return false;
    }

    /**
     * @brief 检查蛇是否踩到陷阱
     * @param map 地图对象指针
     */
    void IsOnTrap(class GameMap* map);

    /**
     * @brief 交换蛇头和蛇尾（游戏特殊功能）
     */
    void SwapSnakeHeadTail()
    {
        if (body.size() < 2) return;
        reverse(body.begin(), body.end());

        // 正确逻辑：蛇头朝向远离身体的方向
        // 新蛇头是原来的尾巴，身体在它的后方
        if (body[0].x < body[1].x)       dir = LEFT;   // 蛇头在左，身体在右 → 朝左
        else if (body[0].x > body[1].x)  dir = RIGHT;  // 蛇头在右，身体在左 → 朝右
        else if (body[0].y < body[1].y)  dir = UP;     // 蛇头在上，身体在下 → 朝上
        else                             dir = DOWN;   // 蛇头在下，身体在上 → 朝下
    }

    /**
     * @brief 检查蛇的某个位置是否有支撑（不会掉落）
     * @param x 横坐标
     * @param y 纵坐标
     * @param map 地图对象指针
     * @return true 有支撑，false 没有支撑
     */
    bool SnakeHasSupport(int x, int y, class GameMap* map);

    /**
     * @brief 检查断裂身体的某个位置是否有支撑
     * @param x 横坐标
     * @param y 纵坐标
     * @param ignoreIdx 要忽略的断裂身体段索引
     * @param map 地图对象指针
     * @return true 有支撑，false 没有支撑
     */
    bool BreakHasSupport(int x, int y, int ignoreIdx, class GameMap* map);

    /**
     * @brief 检查蛇是否需要掉落
     * @param map 地图对象指针
     * @return true 需要掉落，false 不需要
     */
    bool CheckNeedDrop(class GameMap* map);

    // 让蛇整体向下移动一格
    void AllDrop() { for (auto& seg : body) seg.y += 1; }

    /**
     * @brief 检查蛇是否掉出地图
     * @return true 掉出地图，false 没有
     */
    bool IsOutMap()
    {
        for (auto& seg : body)
            if (seg.y >= 19) return true;
        return false;
    }

    /**
     * @brief 更新蛇的掉落逻辑
     * @param map 地图对象指针
     */
    void UpdateSnakeDrop(class GameMap* map);

    /**
     * @brief 让单个断裂身体段向下掉落一格
     * @param idx 断裂身体段索引
     * @param map 地图对象指针
     */
    void DropSingleBreak(int idx, class GameMap* map);

    /**
     * @brief 检查单个断裂身体段是否需要掉落
     * @param idx 断裂身体段索引
     * @param map 地图对象指针
     */
    void CheckBreakDrop(int idx, class GameMap* map);

    /**
     * @brief 更新所有断裂身体段的掉落逻辑
     * @param map 地图对象指针
     */
    void UpdateAllBreakDrop(class GameMap* map);

    /**
     * @brief 检查是否可以推动某个断裂身体段
     * @param idx 断裂身体段索引
     * @param dx 水平推动方向
     * @param dy 垂直推动方向
     * @param map 地图对象指针
     * @return true 可以推动，false 不可以
     */
    bool CanPushBreak(int idx, int dx, int dy, class GameMap* map);

    /**
     * @brief 推动单个断裂身体段（递归处理连锁推动）
     * @param idx 断裂身体段索引
     * @param dx 水平推动方向
     * @param dy 垂直推动方向
     * @param map 地图对象指针
     */
    void PushBreakBlock(int idx, int dx, int dy, class GameMap* map);

    /**
     * @brief 推动指定坐标的断裂身体段
     * @param dx 水平推动方向
     * @param dy 垂直推动方向
     * @param tarX 目标横坐标
     * @param tarY 目标纵坐标
     * @param map 地图对象指针
     */
    void PushBreak(int dx, int dy, int tarX, int tarY, class GameMap* map);

    /**
     * @brief 蛇移动核心逻辑
     * @param dx 水平移动方向
     * @param dy 垂直移动方向
     * @param map 地图对象指针
     * @return true 移动成功（到达门），false 移动失败或普通移动
     */
    bool Move(int dx, int dy, class GameMap* map);

    /**
     * @brief 处理玩家键盘输入控制蛇移动
     * @param map 地图对象指针
     */
    void PlayerControl(class GameMap* map);

    /**
     * @brief 绘制蛇
     * @param cellSize 地图格子大小
     */
    void Draw(int cellSize);

    /**
     * @brief 绘制所有断裂的身体段
     * @param cellSize 地图格子大小
     */
    void DrawBreakBody(int cellSize);

    // 获取蛇是否正在掉落
    bool GetIsDropping() { return isDropping; }
    // 设置蛇是否正在掉落
    void SetDropping(bool b) { isDropping = b; }
    // 获取蛇是否死亡
    bool GetIsDead() { return isDead; }
    // 设置蛇是否死亡
    void SetDead(bool b) { isDead = b; }
    // 获取蛇是否中毒
    bool GetIsPoisoned() { return isPoisoned; }
    // 设置蛇是否中毒
    void SetPoisoned(bool b) { isPoisoned = b; }
    // 获取蛇当前移动方向
    Direction GetDir() { return dir; }
    // 设置蛇当前移动方向
    void SetDir(Direction d) { dir = d; }
};

//=================================== 地图关卡类 ===================================
/**
 * @brief 地图关卡管理类
 * 存储所有关卡数据，管理当前关卡，提供地图元素查询和绘制功能
 */
class GameMap
{
private:
    vector<vector<vector<int>>> levelDatas; // 所有关卡数据，三维数组：[关卡索引][行][列]
    vector<vector<int>> curLevelData;       // 当前关卡数据
    vector<vector<Vector2>> snakeInitPos;   // 每个关卡蛇的初始位置
    int curLevelIdx;                        // 当前关卡索引
    const int cellSize = 40;                // 地图每个格子的像素大小

public:
    /**
     * @brief 地图类构造函数
     * 初始化所有关卡数据和蛇的初始位置
     * 地图元素编码：0=空地 1=墙 2=食物 3=毒药 4=门 5=陷阱
     */
    GameMap()
    {
        // 初始化4个关卡的地图数据
        levelDatas = {
            // 关卡1
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
                {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,3,0,0,0,0,0,0,0,0,0,0,0},
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
            // 关卡2
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
            },
            // 关卡3
            {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
{1,1,1,1,1,1,0,1,0,0,1,1,5,5,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,1,0,1,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            },
            // 关卡4（居中优化版）
            {
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,3,0,1,1,1,1,1,1,1,1,4,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,0,0,0,1,1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,2,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,2,5,5,5,5,5,5,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,2,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,0,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,0,0,0,5,5,5,5,5,5,5,5,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
}            

             ,
            {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,1,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,2,2,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,2,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
},             
            {//5             x              x             x             x
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//10
    {0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,5,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//15
    {0,0,0,0,0,0,2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//20
},
             {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,4,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
             },
        };
        // 初始化每个关卡蛇的初始位置
        snakeInitPos = {
            {{6,11},{5,11},{4,11}},  // 关卡1初始位置
            {{6,10},{5,10},{4,10}},  // 关卡2初始位置
            {{2,9},{1,9},{0,9}},     // 关卡3初始位置
            {{8,5},{7,5},{6,5}},      // 关卡4初始位置
            {{8,11},{7,11}},
            {{10,8},{9,8},{8,8}},
            {{14,10},{13,10}}
        };
        curLevelIdx = 0; // 默认从第0关开始
        curLevelData = levelDatas[0];
    }

    /**
     * @brief 切换到指定关卡
     * @param idx 关卡索引
     */
    void SwitchLevel(int idx)
    {
        curLevelIdx = idx;
        curLevelData = levelDatas[idx];
    }

    // 获取当前关卡索引
    int GetCurLevel() { return curLevelIdx; }
    // 获取总关卡数
    int GetLevelCount() { return levelDatas.size(); }
    // 获取当前关卡数据
    vector<vector<int>>& GetCurLevelData() { return curLevelData; }
    // 获取指定关卡蛇的初始位置
    vector<Vector2> GetSnakeInitPos(int idx) { return snakeInitPos[idx]; }

    /**
     * @brief 清空地图指定坐标的元素
     * @param x 横坐标
     * @param y 纵坐标
     */
    void ClearCell(int x, int y) { curLevelData[y][x] = 0; }

    /**
     * @brief 检查指定坐标是否是墙
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 是墙，false 不是
     */
    bool IsWall(int x, int y)
    {
        if (y < 0 || y >= 20 || x < 0 || x >= 30) return true; // 超出地图边界视为墙
        return curLevelData[y][x] == 1;
    }

    /**
     * @brief 检查指定坐标是否是食物或毒药
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 是食物或毒药，false 不是
     */
    bool IsFoodOrPoison(int x, int y)
    {
        if (y < 0 || y >= 20 || x < 0 || x >= 30) return false;
        return curLevelData[y][x] == 2 || curLevelData[y][x] == 3;
    }

    /**
     * @brief 检查指定坐标是否是毒药
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 是毒药，false 不是
     */
    bool IsPoison(int x, int y)
    {
        if (y < 0 || y >= 20 || x < 0 || x >= 30) return false;
        return curLevelData[y][x] == 3;
    }

    /**
     * @brief 检查指定坐标是否是门（通关点）
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 是门，false 不是
     */
    bool IsDoor(int x, int y)
    {
        if (y < 0 || y >= 20 || x < 0 || x >= 30) return false;
        return curLevelData[y][x] == 4;
    }

    /**
     * @brief 检查指定坐标是否是陷阱
     * @param x 横坐标
     * @param y 纵坐标
     * @return true 是陷阱，false 不是
     */
    bool IsTrap(int x, int y)
    {
        if (y < 0 || y >= 20 || x < 0 || x >= 30) return false;
        return curLevelData[y][x] == 5;
    }

    /**
     * @brief 绘制当前关卡地图
     * @param tex 纹理资源管理器引用
     */
    void DrawMap(TextureResource& tex)
    {
        for (int y = 0; y < curLevelData.size(); y++)
        {
            for (int x = 0; x < curLevelData[y].size(); x++)
            {
                int px = x * cellSize; // 转换为屏幕像素坐标
                int py = y * cellSize;
                // 根据地图元素编码绘制对应纹理
                if (curLevelData[y][x] == 1) DrawTexture(tex.wall, px, py, WHITE);
                else if (curLevelData[y][x] == 2) DrawTexture(tex.food, px, py, WHITE);
                else if (curLevelData[y][x] == 3) DrawTexture(tex.poison, px, py, WHITE);
                else if (curLevelData[y][x] == 4) DrawTexture(tex.door, px, py, WHITE);
                else if (curLevelData[y][x] == 5) DrawTexture(tex.trap, px, py, WHITE);
            }
        }
    }

    // 获取地图格子大小
    int GetCellSize() { return cellSize; }
};

//=================================== UI工具类 ===================================
/**
 * @brief UI工具类
 * 提供通用的UI绘制功能，包括按钮、文本提示、输入框
 */
class GameUI
{
public:
    /**
     * @brief 绘制按钮并检测点击
     * @param rect 按钮矩形区域
     * @param text 按钮显示文字
     * @return true 按钮被点击，false 没有被点击
     */
    bool DrawButton(Rectangle rect, const char* text)
    {
        bool hover = CheckCollisionPointRec(GetMousePosition(), rect); // 检测鼠标是否悬停
        bool click = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT); // 检测鼠标左键点击
        // 绘制按钮背景（悬停时变深灰色）
        DrawRectangleRec(rect, hover ? GRAY : LIGHTGRAY);
        // 绘制按钮边框
        DrawRectangleLinesEx(rect, 2, BLACK);
        // 绘制按钮文字（居中显示）
        int tw = MeasureText(text, 24);
        DrawText(text, rect.x + rect.width / 2 - tw / 2, rect.y + 15, 24, BLACK);
        return click;
    }

    /**
     * @brief 绘制文本提示
     * @param str 提示文字
     * @param x 横坐标
     * @param y 纵坐标
     * @param fontSize 字体大小
     * @param color 文字颜色
     */
    void DrawTextTip(const char* str, int x, int y, int fontSize, Color color)
    {
        DrawText(str, x, y, fontSize, color);
    }

    /**
     * @brief 绘制带焦点高亮的输入框
     * @param box 输入框矩形区域
     * @param inputStr 输入框绑定的字符串
     * @param fontSize 字体大小
     * @param isFocus 是否获得焦点
     */
    void DrawInputBox(Rectangle box, string& inputStr, int fontSize, bool isFocus = false)
    {
        // 激活的输入框显示蓝色粗边框
        if (isFocus)
        {
            DrawRectangleRec(box, WHITE);
            DrawRectangleLinesEx(box, 3, BLUE);
        }
        else
        {
            DrawRectangleRec(box, WHITE);
            DrawRectangleLinesEx(box, 2, BLACK);
        }
        // 绘制输入框内的文字
        DrawText(inputStr.c_str(), box.x + 10, box.y + 8, fontSize, BLACK);
    }
};

//=================================== 蛇类函数实现 ===================================
bool Snake::SnakeHasSupport(int x, int y, GameMap* map)
{
    int nx = x;
    int ny = y + 1; // 检查正下方是否有支撑
    // 墙、食物、毒药都可以作为支撑
    if (map->IsWall(nx, ny) || map->IsFoodOrPoison(nx, ny)) return true;
    // 其他断裂的身体段也可以作为支撑
    for (auto& piece : breakBodyHistory)
        for (auto& seg : piece)
            if (seg.x == nx && seg.y == ny) return true;
    return false;
}

void Snake::IsOnTrap(GameMap* map)
{
    for (auto& seg : body)
        if (map->IsTrap((int)seg.x, (int)seg.y))
        {
            isDead = true; // 踩到陷阱直接死亡
            return;
        }
}

bool Snake::BreakHasSupport(int x, int y, int ignoreIdx, GameMap* map)
{
    int nx = x;
    int ny = y + 1;
    if (map->IsWall(nx, ny) || map->IsFoodOrPoison(nx, ny)) return true;
    if (IsInBody(nx, ny)) return true; // 蛇的身体可以作为支撑
    // 其他断裂的身体段可以作为支撑（忽略自身）
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
    {
        if (i == ignoreIdx) continue;
        for (auto& seg : breakBodyHistory[i])
            if (seg.x == nx && seg.y == ny) return true;
    }
    return false;
}

bool Snake::CheckNeedDrop(GameMap* map)
{
    // 只要有一个身体段有支撑，就不需要掉落
    for (auto& seg : body)
        if (SnakeHasSupport((int)seg.x, (int)seg.y, map)) return true;
    return false;
}

void Snake::UpdateSnakeDrop(GameMap* map)
{
    if (!isDropping) return;
    dropTimer += GetFrameTime();
    // 达到掉落速度阈值时向下移动一格
    if (dropTimer >= DROP_SPEED)
    {
        AllDrop();
        dropTimer = 0;
        // 掉出地图则死亡
        if (IsOutMap())
        {
            body.clear();
            isPoisoned = false;
            isDropping = false;
            isDead = true;
            return;
        }
        // 检查是否还有支撑，没有则继续掉落
        if (CheckNeedDrop(map)) isDropping = false;
    }
}

void Snake::DropSingleBreak(int idx, GameMap* map)
{
    if (idx < 0 || idx >= (int)breakBodyHistory.size()) return;
    bool canDrop = true;
    // 检查断裂身体段所有格子是否都没有支撑
    for (auto& seg : breakBodyHistory[idx])
    {
        if (BreakHasSupport((int)seg.x, (int)seg.y, idx, map))
        {
            canDrop = false;
            break;
        }
    }
    if (!canDrop)
    {
        breakDropState[idx] = false;
        return;
    }
    // 所有格子都没有支撑，向下移动一格
    for (auto& seg : breakBodyHistory[idx]) seg.y += 1;
}

void Snake::CheckBreakDrop(int idx, GameMap* map)
{
    if (idx < 0 || idx >= (int)breakBodyHistory.size()) return;
    if (breakDropState[idx]) return;
    bool hasSup = false;
    // 检查是否有任何一个格子有支撑
    for (auto& seg : breakBodyHistory[idx])
    {
        if (BreakHasSupport((int)seg.x, (int)seg.y, idx, map))
        {
            hasSup = true;
            break;
        }
    }
    breakDropState[idx] = !hasSup; // 没有支撑则标记为需要掉落
}

void Snake::UpdateAllBreakDrop(GameMap* map)
{
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
    {
        // 初始化断裂身体段的状态和计时器
        if (i >= (int)breakDropState.size()) breakDropState.push_back(false);
        if (i >= (int)breakDropTimer.size()) breakDropTimer.push_back(0);
        if (!breakDropState[i])
        {
            CheckBreakDrop(i, map);
            continue;
        }
        // 更新掉落计时器
        breakDropTimer[i] += GetFrameTime();
        if (breakDropTimer[i] >= DROP_SPEED)
        {
            DropSingleBreak(i, map);
            breakDropTimer[i] = 0;
            // 检查是否掉出地图
            bool out = false;
            for (auto& seg : breakBodyHistory[i])
                if (seg.y >= 19) { out = true; break; }
            // 掉出地图则删除该断裂身体段
            if (out)
            {
                breakBodyHistory.erase(breakBodyHistory.begin() + i);
                breakDropState.erase(breakDropState.begin() + i);
                breakDropTimer.erase(breakDropTimer.begin() + i);
                i--; // 删除后索引回退
            }
        }
    }
}

bool Snake::CanPushBreak(int idx, int dx, int dy, GameMap* map)
{
    if (idx < 0 || idx >= (int)breakBodyHistory.size()) return true;
    for (auto& seg : breakBodyHistory[idx])
    {
        int nx = seg.x + dx;
        int ny = seg.y + dy;
        // 墙、食物、毒药,蛇身挡住不能被推动
        if (map->IsWall(nx, ny) || map->IsFoodOrPoison(nx, ny) || IsInBody(nx, ny)) return false;
        // 递归检查被挡住的其他断裂身体段是否可以被推动
        for (int i = 0; i < (int)breakBodyHistory.size(); i++)
        {
            if (i == idx) continue;
            for (auto& s : breakBodyHistory[i])
            {
                if (s.x == nx && s.y == ny)
                    if (!CanPushBreak(i, dx, dy, map)) return false;
            }
        }
    }
    return true;
}

void Snake::PushBreakBlock(int idx, int dx, int dy, GameMap* map)
{
    if (idx < 0 || idx >= (int)breakBodyHistory.size()) return;
    // 先推动所有被挡住的断裂身体段（递归）
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
    {
        if (i == idx) continue;
        for (auto& s : breakBodyHistory[i])
        {
            for (auto& seg : breakBodyHistory[idx])
            {
                if (s.x == seg.x + dx && s.y == seg.y + dy)
                    PushBreakBlock(i, dx, dy, map);
            }
        }
    }
    // 推动当前断裂身体段
    for (auto& seg : breakBodyHistory[idx])
    {
        seg.x += dx;
        seg.y += dy;
    }
    // 推动后检查是否需要掉落
    CheckBreakDrop(idx, map);
}

void Snake::PushBreak(int dx, int dy, int tarX, int tarY, GameMap* map)
{
    int tarIdx = -1;
    // 找到目标坐标所在的断裂身体段
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
    {
        for (auto& seg : breakBodyHistory[i])
        {
            if (seg.x == tarX && seg.y == tarY)
            {
                tarIdx = i;
                break;
            }
        }
        if (tarIdx != -1) break;
    }
    if (tarIdx == -1) return;
    // 如果可以推动则执行推动
    if (CanPushBreak(tarIdx, dx, dy, map))
        PushBreakBlock(tarIdx, dx, dy, map);
    // 推动后检查所有断裂身体段是否需要掉落
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
        CheckBreakDrop(i, map);
}

bool Snake::Move(int dx, int dy, GameMap* map)
{
    if (isDropping) return false; // 掉落时不能移动
    Vector2 head = GetHeadPos();
    Vector2 newHead = { head.x + dx, head.y + dy }; // 计算新蛇头位置

    // 碰撞检测：撞墙或撞自己身体则移动失败
    if (map->IsWall((int)newHead.x, (int)newHead.y)) return false;
    if (IsInBody((int)newHead.x, (int)newHead.y)) return false;

    // 如果撞到断裂身体段，尝试推动它
    if (IsInBreakBody((int)newHead.x, (int)newHead.y))
        PushBreak(dx, dy, (int)newHead.x, (int)newHead.y, map);
    // 推动后如果还是撞到断裂身体段，则移动失败
    if (IsInBreakBody((int)newHead.x, (int)newHead.y)) return false;

    // 中毒处理：移动时如果中毒，身体会断裂一半,
    if (isPoisoned)
    {
        //body.push_back(newHead); // 末尾再增加一个格子保持长度不变（因为后面会插入新头）
        int half = (body.size()) / 2;
        vector<Vector2> cut(body.begin() + half, body.end()); // 切割后半部分身体
        breakBodyHistory.push_back(cut); // 添加到断裂身体列表
        breakDropState.push_back(false);
        breakDropTimer.push_back(0);
        int len = body.size();
        for (int i = half+1; i < len; i++) body.pop_back(); // 删除后半部分身体，然后变长一格
        
        isPoisoned = false; // 清除中毒状态
    }

    // 根据吃到的东西处理不同逻辑
    if (map->IsPoison((int)newHead.x, (int)newHead.y))
    {
        // 吃到毒药：进入中毒状态
        body.insert(body.begin(), newHead);
        body.pop_back();
        map->ClearCell((int)newHead.x, (int)newHead.y);
        isPoisoned = true;
    }
    else if (map->IsDoor((int)newHead.x, (int)newHead.y))
    {
        // 到达门：通关
        body.insert(body.begin(), newHead);
        body.pop_back();
        return true;
    }
    else if (map->IsFoodOrPoison((int)newHead.x, (int)newHead.y))
    {
        // 吃到食物：身体变长
        body.insert(body.begin(), newHead);
        
        map->ClearCell((int)newHead.x, (int)newHead.y);
    }
    else
    {
        
            body.insert(body.begin(), newHead);
            body.pop_back();
        
        
    }

    // 移动后检查是否需要掉落
    if (!CheckNeedDrop(map)) isDropping = true;
    return false;
}

void Snake::PlayerControl(GameMap* map)
{
    if (isDropping) return;

    Direction targetDir = dir;
    int dx = 0, dy = 0;

    // 检测按键，筛选合法转向，不立刻修改当前方向
    if (IsKeyPressed(KEY_UP) && dir != DOWN)
    {
        targetDir = UP;
        dy = -1;
    }
    else if (IsKeyPressed(KEY_DOWN) && dir != UP)
    {
        targetDir = DOWN;
        dy = 1;
    }
    else if (IsKeyPressed(KEY_LEFT) && dir != RIGHT)
    {
        targetDir = LEFT;
        dx = -1;
    }
    else if (IsKeyPressed(KEY_RIGHT) && dir != LEFT)
    {
        targetDir = RIGHT;
        dx = 1;
    }

    // 无按键变更，直接退出
    if (dx == 0 && dy == 0)
        return;

    // 记录移动前蛇头位置
    Vector2 oldHead = GetHeadPos();
    // 执行移动逻辑
    Move(dx, dy, map);
    // 记录移动后蛇头位置
    Vector2 newHead = GetHeadPos();

    // 实际产生位移，再更新蛇头朝向
    if (oldHead.x != newHead.x || oldHead.y != newHead.y)
    {
        dir = targetDir;
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
            // 绘制蛇头（根据方向选择对应纹理）
            if (dir == UP) DrawTexture(texRes->Headup[texRes->currentSkin], x, y, WHITE);
            else if (dir == DOWN) DrawTexture(texRes->Headdown[texRes->currentSkin], x, y, WHITE);
            else if (dir == LEFT) DrawTexture(texRes->Headleft[texRes->currentSkin], x, y, WHITE);
            else DrawTexture(texRes->Headright[texRes->currentSkin], x, y, WHITE);
        }
        else if (i == (int)body.size() - 1)
        {
            // 绘制蛇尾巴（根据前一个身体段的位置选择对应纹理）
            Vector2 prev = body[i - 1];
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
            // 绘制蛇身体（根据前后身体段的位置选择水平、垂直或转弯纹理）
            Vector2 prev = body[i - 1];
            Vector2 next = body[i + 1];
            if (prev.x == next.x)
                DrawTexture(texRes->bodyver[texRes->currentSkin], x, y, WHITE);
            else if (prev.y == next.y)
                DrawTexture(texRes->bodyhend[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x < seg.x && next.y < seg.y) || (next.x < seg.x && prev.y < seg.y))
                DrawTexture(texRes->bodyturn1[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x > seg.x && next.y < seg.y) || (next.x > seg.x && prev.y < seg.y))
                DrawTexture(texRes->bodyturn2[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x < seg.x && next.y > seg.y) || (next.x < seg.x && prev.y > seg.y))
                DrawTexture(texRes->bodyturn3[texRes->currentSkin], x, y, WHITE);
            else if ((prev.x > seg.x && next.y > seg.y) || (next.x > seg.x && prev.y > seg.y))
                DrawTexture(texRes->bodyturn4[texRes->currentSkin], x, y, WHITE);
        }
    }
}

void Snake::DrawBreakBody(int cellSize)
{
    int skinIdx = texRes->currentSkin;
    for (int i = 0; i < (int)breakBodyHistory.size(); i++)
    {
        auto& segList = breakBodyHistory[i];
        for (int j = 0; j < (int)segList.size(); j++)
        {
            Vector2 seg = segList[j];
            float x = seg.x * cellSize;
            float y = seg.y * cellSize;
            if (segList.size() == 1)
            {
                // 单个格子的断裂身体
                DrawTexture(texRes->dock1[skinIdx], x, y, WHITE);
                continue;
            }
            if (j == 0)
            {
                // 断裂身体段的头部
                Vector2 next = segList[j + 1];
                float dx = next.x - seg.x;
                float dy = next.y - seg.y;
                if (dx > 0)
                    DrawTexture(texRes->dockleft[skinIdx], x, y, WHITE);
                else if (dx < 0)
                    DrawTexture(texRes->dockright[skinIdx], x, y, WHITE);
                else if (dy > 0)
                    DrawTexture(texRes->dockup[skinIdx], x, y, WHITE);
                else if (dy < 0)
                    DrawTexture(texRes->dockdown[skinIdx], x, y, WHITE);
            }
            else if (j == segList.size() - 1)
            {
                // 断裂身体段的尾部
                Vector2 prev = segList[j - 1];
                float dx = prev.x - seg.x;
                float dy = prev.y - seg.y;
                if (dx > 0)
                    DrawTexture(texRes->dockleft[skinIdx], x, y, WHITE);
                else if (dx < 0)
                    DrawTexture(texRes->dockright[skinIdx], x, y, WHITE);
                else if (dy > 0)
                    DrawTexture(texRes->dockup[skinIdx], x, y, WHITE);
                else if (dy < 0)
                    DrawTexture(texRes->dockdown[skinIdx], x, y, WHITE);
            }
            else
            {
                // 断裂身体段的中间部分
                Vector2 prev = segList[j - 1];
                Vector2 next = segList[j + 1];
                if (prev.x == next.x)
                    DrawTexture(texRes->bodyver[skinIdx], x, y, WHITE);
                else if (prev.y == next.y)
                    DrawTexture(texRes->bodyhend[skinIdx], x, y, WHITE);
                else if ((prev.x < seg.x && next.y < seg.y) || (next.x < seg.x && prev.y < seg.y))
                    DrawTexture(texRes->bodyturn1[skinIdx], x, y, WHITE);
                else if ((prev.x > seg.x && next.y < seg.y) || (next.x > seg.x && prev.y < seg.y))
                    DrawTexture(texRes->bodyturn2[skinIdx], x, y, WHITE);
                else if ((prev.x < seg.x && next.y > seg.y) || (next.x < seg.x && prev.y > seg.y))
                    DrawTexture(texRes->bodyturn3[skinIdx], x, y, WHITE);
                else if ((prev.x > seg.x && next.y > seg.y) || (next.x > seg.x && prev.y > seg.y))
                    DrawTexture(texRes->bodyturn4[skinIdx], x, y, WHITE);
            }
        }
    }
}

//=================================== 游戏主控类 ===================================
/**
 * @brief 游戏主控类
 * 游戏的核心控制类，管理游戏状态机、数据库连接、逻辑更新和画面渲染
 */
class SnakeGameMain
{
private:
    GameState gameState;       // 当前游戏状态
    TextureResource texRes;    // 纹理资源管理器
    GameMap gameMap;           // 地图管理器
    Snake snake;               // 蛇实体
    GameUI ui;                 // UI工具
    MysqlDB db;                // MySQL数据库操作对象

    // 登录注册输入缓存
    string loginUser, loginPwd;
    string regUser, regPwd;
    string tipMsg;             // 提示文字
    float tipTime;             // 提示文字显示倒计时

    // 通关计时相关变量
    int currentUserId;         // 当前登录用户ID
    float gameStartTime;       // 本局游戏开始时间
    float gameCostTime;        // 本局通关耗时
    float bestTime;            // 本关最好成绩
    int inputFocus;            // 输入框焦点（0=用户名，1=密码）
    bool isRecordSaved;        // 每局游戏是否已保存记录（防止重复插入数据库）

public:
    /**
     * @brief 游戏主控类构造函数
     * 初始化游戏窗口、数据库连接、加载所有纹理资源
     */
    SnakeGameMain() : snake(&texRes)
    {
        InitWindow(1200, 800, "Greedy Apple Snake"); // 创建1200x800的游戏窗口
        SetTargetFPS(60); // 设置游戏帧率为60FPS
        gameState = state_login; // 初始状态为登录界面
        db.Connect(); // 连接MySQL数据库
        texRes.LoadAll(); // 加载所有游戏纹理
        tipTime = 0;
        currentUserId = -1; // 初始用户ID为-1（未登录）
        inputFocus = 0; // 默认焦点在用户名输入框
    }

    /**
     * @brief 游戏主控类析构函数
     * 关闭数据库连接、卸载所有纹理资源、关闭游戏窗口
     */
    ~SnakeGameMain()
    {
        db.Close();
        texRes.UnloadAll();
        CloseWindow();
    }

    /**
     * @brief 游戏主循环
     * 游戏的核心循环，每帧执行逻辑更新和画面渲染
     */
    void Run()
    {
        while (!WindowShouldClose()) // 直到用户关闭窗口
        {
            UpdateLogic(); // 更新游戏逻辑
            BeginDrawing(); // 开始绘制
            ClearBackground(RAYWHITE); // 清空背景为白色
            DrawRender(); // 绘制游戏画面
            EndDrawing(); // 结束绘制
        }
    }

    /**
     * @brief 更新游戏逻辑
     * 根据当前游戏状态调用对应的逻辑更新函数
     */
    void UpdateLogic()
    {
        tipTime -= GetFrameTime(); // 提示文字倒计时
        switch (gameState)
        {
        case state_login: UpdateLogin(); break;
        case state_register: UpdateRegister(); break;
        case state_main_menu: UpdateMainMenu(); break;
        case state_level_select: UpdateLevelSelect(); break;
        case state_gameplay: UpdateGamePlay(); break;
        case state_game_over: UpdateGameOver(); break;
        case state_change_skin: UpdateChangeSkin(); break;
        }
    }

    /**
     * @brief 绘制游戏画面
     * 根据当前游戏状态调用对应的绘制函数
     */
    void DrawRender()
    {
        switch (gameState)
        {
        case state_login: DrawLogin(); break;
        case state_register: DrawRegister(); break;
        case state_main_menu: DrawMainMenu(); break;
        case state_level_select: DrawLevelSelect(); break;
        case state_gameplay: DrawGamePlay(); break;
        case state_game_over: DrawGameOver(); break;
        case state_change_skin: DrawChangeSkin(); break;
        }
    }

    /**
     * @brief 键盘输入处理函数
     * @param str 绑定的输入字符串
     * @param maxLen 最大输入长度
     */
    void InputHandle(string& str, int maxLen = 20)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if (str.size() < maxLen)
                str += (char)key;
            key = GetCharPressed();
        }
        // 退格键删除最后一个字符
        if (IsKeyPressed(KEY_BACKSPACE) && !str.empty())
            str.pop_back();
    }

    //========= 登录界面逻辑 ==========
    void UpdateLogin()
    {
        // 1. 鼠标点击切换输入框焦点
        Vector2 mouse = GetMousePosition();
        Rectangle userBox = { 450, 275, 300, 40 };
        Rectangle pwdBox = { 450, 325, 300, 40 };
		// 点击输入框时切换焦点，点击其他区域时取消焦点
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mouse, userBox)) inputFocus = 0;
            else if (CheckCollisionPointRec(mouse, pwdBox)) inputFocus = 1;
        }

        // 2. 只给当前焦点的输入框处理输入
        if (inputFocus == 0)
            InputHandle(loginUser);
        else
            InputHandle(loginPwd);

        // 3. 登录按钮逻辑
        if (ui.DrawButton({ 450, 400, 300, 50 }, "Login"))
        {
            if (loginUser.empty() || loginPwd.empty())
            {
                tipMsg = "Username and password cannot be empty!";
                tipTime = 2;
            }
            else if (db.LoginCheck(loginUser, loginPwd))
            {
                tipMsg = "Login successful!";
                tipTime = 1;
                currentUserId = db.GetUserIdByName(loginUser); // 获取当前用户ID
                gameState = state_main_menu; // 登录成功跳转到主菜单
            }
            else
            {
                tipMsg = "Incorrect username or password!";
                tipTime = 2;
            }
        }
        // 跳转注册界面
        if (ui.DrawButton({ 450, 470, 300, 50 }, "Register Account"))
        {
            gameState = state_register;
            loginUser.clear();
            loginPwd.clear();
            inputFocus = 0; // 重置焦点
        }
    }

    // 登录界面绘制
    void DrawLogin()
    {
        DrawFPS(10, 10); // 绘制帧率
        ui.DrawTextTip("Snake Game Login", 400, 180, 50, DARKBLUE);
        ui.DrawTextTip("Username:", 280, 280, 30, BLACK);
        // 绘制用户名输入框（带焦点高亮）
        ui.DrawInputBox({ 450, 275, 300, 40 }, loginUser, 28, inputFocus == 0);
        ui.DrawTextTip("Password:", 280, 330, 30, BLACK);
        // 绘制密码输入框（带焦点高亮）
        ui.DrawInputBox({ 450, 325, 300, 40 }, loginPwd, 28, inputFocus == 1);
        // 绘制提示文字
        if (tipTime > 0)
            ui.DrawTextTip(tipMsg.c_str(), 480, 550, 28, RED);
    }

    //========= 注册界面逻辑 ==========
    void UpdateRegister()
    {
        // 1. 鼠标点击切换注册界面输入框焦点
        Vector2 mouse = GetMousePosition();
        Rectangle regUserBox = { 450, 275, 300, 40 };
        Rectangle regPwdBox = { 450, 325, 300, 40 };

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (CheckCollisionPointRec(mouse, regUserBox)) inputFocus = 0;
            else if (CheckCollisionPointRec(mouse, regPwdBox)) inputFocus = 1;
        }

        // 2. 只给当前焦点的输入框处理输入
        if (inputFocus == 0)
            InputHandle(regUser);
        else
            InputHandle(regPwd);

        // 提交注册逻辑
        if (ui.DrawButton({ 450, 400, 300, 50 }, "Submit Register"))
        {
			if (regUser.empty() || regPwd.size() < 6)
            {
                tipMsg = "Username or password is invalid!";
                tipTime = 2;
            }
            else if (db.CheckUserExist(regUser))
            {
                tipMsg = "Username already exists!";
                tipTime = 2;
            }
            else if (db.Register(regUser, regPwd))
            {
                tipMsg = "Registration successful! Please login.";
                tipTime = 2;
                regUser.clear();
                regPwd.clear();
                inputFocus = 0; // 重置焦点
                gameState = state_login; // 注册成功跳转到登录界面
            }
            else
            {
                tipMsg = "Registration failed!";
                tipTime = 2;
            }
        }
        // 返回登录界面
        if (ui.DrawButton({ 450, 470, 300, 50 }, "Back To Login"))
        {
            gameState = state_login;
            regUser.clear();
            regPwd.clear();
            inputFocus = 0; // 重置焦点
        }
    }

    // 注册界面绘制
    void DrawRegister()
    {
        DrawFPS(10, 10);
        ui.DrawTextTip("Account Register", 400, 180, 50, DARKBLUE);
        ui.DrawTextTip("New Username:", 220, 280, 30, BLACK);
        ui.DrawInputBox({ 450, 275, 300, 40 }, regUser, 28, inputFocus == 0);
        ui.DrawTextTip("New Password:", 220, 330, 30, BLACK);
        ui.DrawInputBox({ 450, 325, 300, 40 }, regPwd, 28, inputFocus == 1);
        if (tipTime > 0)
            ui.DrawTextTip(tipMsg.c_str(), 480, 550, 28, RED);
    }

    //========= 主菜单界面逻辑 ==========
    void UpdateMainMenu()
    {
        if (ui.DrawButton({ 500,300,200,50 }, "Start Game")) {
            gameState = state_level_select; // 跳转到关卡选择界面
        }
        if (ui.DrawButton({ 480,360,240,50 }, "Change Skin"))
        {
            gameState = state_change_skin; // 跳转到皮肤更换界面
        }
        if (ui.DrawButton({ 500,420,200,50 }, "Exit Game")) {
            CloseWindow(); // 退出游戏
        }
    }

    // 主菜单界面绘制
    void DrawMainMenu()
    {
        DrawFPS(10, 10);
        ui.DrawTextTip("Greedy Apple Snake", 400, 200, 48, DARKGREEN);
    }

    //========= 关卡选择界面逻辑 ==========
    void UpdateLevelSelect()
    {
        int levelCnt = gameMap.GetLevelCount();
        float startY = 280.0f;
		float startX = 300.0f;
        float btnGap = 60.0f;
		// 动态生成关卡按钮,先横着排放满再换行
        for (int i = 0; i < levelCnt; i++)
        {
			float btnY = startY + (i % 5) * btnGap; // 每5个按钮换一行
			float btnX = startX + (i / 5) * 250; // 每5个按钮换一列
            string btnText = "Level " + to_string(i + 1);
            if (ui.DrawButton({ btnX, btnY, 200, 50 }, btnText.c_str()))
            {
                gameMap.SwitchLevel(i); // 切换到选中的关卡
                snake.Init(gameMap.GetSnakeInitPos(i)); // 初始化蛇
                gameStartTime = GetTime(); // 记录游戏开始时间
                isRecordSaved = false; // 重置记录保存标记
                gameState = state_gameplay; // 进入游戏界面
            }
        }
        // 返回主菜单按钮
        float backBtnY = startY + levelCnt * btnGap + 20;
        if (ui.DrawButton({ 480, backBtnY, 240, 50 }, "Back to Main Menu"))
        {
            gameState = state_main_menu;
        }
    }

    // 关卡选择界面绘制
    void DrawLevelSelect()
    {
        DrawFPS(10, 10);
        ui.DrawTextTip("Select Level", 450, 200, 48, DARKGREEN);
        int levelCnt = gameMap.GetLevelCount();
        float startY = 280.0f;
		float startX = 300.0f;
        float btnGap = 60.0f;
        // 绘制所有关卡按钮
        for (int i = 0; i < levelCnt; i++)
        {
            float btnY = startY + (i % 5) * btnGap; // 每5个按钮换一行
			float btnX = startX + (i / 5) * 250; // 每5个按钮换一列
            string btnText = "Level " + to_string(i + 1);
            ui.DrawButton({ btnX, btnY, 200, 50 }, btnText.c_str());
        }
        float backBtnY = startY + levelCnt * btnGap + 20;
        ui.DrawButton({ 480, backBtnY, 240, 50 }, "Back to Main Menu");
    }

    //========= 游戏进行中界面逻辑 ==========
    void UpdateGamePlay()
    {
        // 右上角功能按钮
        if (ui.DrawButton({ 1050,20,120,40 }, "Restart"))
        {
            int cur = gameMap.GetCurLevel();
            gameMap.SwitchLevel(cur);
            snake.Init(gameMap.GetSnakeInitPos(cur));
        }
        if (ui.DrawButton({ 1000,70,200,40 }, "change head")) {
            snake.SwapSnakeHeadTail(); // 交换蛇头蛇尾
        }
        if (ui.DrawButton({ 960,120,240,40 }, "Back to Main Menu")) {
            int cur = gameMap.GetCurLevel();
            gameMap.SwitchLevel(cur);
            snake.Init(gameMap.GetSnakeInitPos(cur));
            gameState = state_main_menu;
        }

        // 更新游戏逻辑
        snake.UpdateAllBreakDrop(&gameMap); // 更新断裂身体掉落
        snake.UpdateSnakeDrop(&gameMap);    // 更新蛇整体掉落
        snake.PlayerControl(&gameMap);      // 处理玩家输入
        snake.IsOnTrap(&gameMap);           // 检查是否踩到陷阱

        // 检查游戏结束条件
        Vector2 head = snake.GetHeadPos();
        if (gameMap.IsDoor((int)head.x, (int)head.y))
            gameState = state_game_over; // 到达门，通关
        if (snake.GetIsDead())
            gameState = state_game_over; // 蛇死亡
    }

    // 游戏进行中界面绘制
    void DrawGamePlay()
    {
        DrawFPS(10, 10);
        gameMap.DrawMap(texRes); // 绘制地图
        snake.Draw(gameMap.GetCellSize()); // 绘制蛇
        snake.DrawBreakBody(gameMap.GetCellSize()); // 绘制断裂的身体
    }

    //========= 游戏结束界面逻辑 ==========
    void UpdateGameOver()
    {
        // 只有第一次进入结束界面、并且是通关（不是死亡）时，才保存记录
        if (!isRecordSaved && !snake.GetIsDead())
        {
            int curLevel = gameMap.GetCurLevel() + 1; // 关卡号从1开始
            // 只计算一次通关耗时，之后不再改变
            gameCostTime = GetTime() - gameStartTime;
			// 只插入一次数据库记录,比最好成绩好才更新
            if(gameCostTime < db.GetUserBestTime(currentUserId, curLevel))
                db.InsertLevelRecord(currentUserId, curLevel, gameCostTime);
            // 只查询一次最好成绩
            bestTime = db.GetUserBestTime(currentUserId, curLevel);
            // 标记为已保存，之后再也不会执行这段代码
            isRecordSaved = true;
        }

        int curLv = gameMap.GetCurLevel();
        // 重新开始按钮
        if (ui.DrawButton({ 500,300,200,50 }, "Restart"))
        {
            gameMap.SwitchLevel(curLv);
            snake.Init(gameMap.GetSnakeInitPos(curLv));
            gameStartTime = GetTime(); // 重新开始时重置开始时间
            isRecordSaved = false; // 重置保存标记
            gameState = state_gameplay;
        }
        // 返回选关界面
        if (ui.DrawButton({ 500,500,200,50 }, "Level Select"))
        {
            gameState = state_level_select;
        }
        // 退出游戏按钮
        if (ui.DrawButton({ 500,700,200,50 }, "Exit"))
            CloseWindow();
        // 下一关按钮（最后一关不显示）
        if (curLv >= gameMap.GetLevelCount() - 1) return;
        if (ui.DrawButton({ 500,400,200,50 }, "Next Level"))
        {
            curLv++;
            gameMap.SwitchLevel(curLv);
			snake.Init(gameMap.GetSnakeInitPos(curLv));
            gameMap.SwitchLevel(curLv);
            snake.Init(gameMap.GetSnakeInitPos(curLv));
            gameStartTime = GetTime(); // 进入下一关时重置开始时间
            isRecordSaved = false; // 重置保存标记
            gameState = state_gameplay;
        }
        
    }

    // 游戏结束界面绘制
    void DrawGameOver()
    {
        DrawFPS(10, 10);
        int cur = gameMap.GetCurLevel();
        if (cur >= gameMap.GetLevelCount() - 1 && !snake.GetIsDead())
        {
            // 最后一关通关提示
            ui.DrawTextTip("This is the last level.", 300, 200, 32, GREEN);
            // 显示通关时间和最好成绩
            char timeStr[64];
            sprintf(timeStr, "Time: %.2fs | Best: %.2fs", gameCostTime, bestTime);
            ui.DrawTextTip(timeStr, 480, 260, 28, DARKGREEN);

            if (ui.DrawButton({ 500,500,200,50 }, "Back to Main Menu"))
                gameState = state_main_menu;
        }
        else if (snake.GetIsDead())
        {
            // 游戏失败提示
            ui.DrawTextTip("Game Over", 450, 200, 48, RED);
        }
        else {
            // 普通关卡通关提示
            ui.DrawTextTip("Congratulations clearance", 300, 200, 48, GREEN);
            // 显示通关时间和最好成绩
            char timeStr[64];
            sprintf(timeStr, "Time: %.2fs | Best: %.2fs", gameCostTime, bestTime);
            ui.DrawTextTip(timeStr, 480, 260, 28, DARKGREEN);
        }
    }

    //========= 皮肤选择界面逻辑 ==========
    void UpdateChangeSkin()
    {
        // 动态生成皮肤选择按钮
        for (int i = 0; i < texRes.MAX_SKINS; i++)
        {
            if (ui.DrawButton({ 500,300.0f + i * 60,200,50 }, (string("Skin ") + to_string(i + 1)).c_str()))
            {
                texRes.currentSkin = i; // 切换到选中的皮肤
                gameState = state_main_menu; // 返回主菜单
            }
        }
        // 返回主菜单按钮
        if (ui.DrawButton({ 480,300.0f + texRes.MAX_SKINS * 60,240,50 }, "Back to Main Menu")) {
            gameState = state_main_menu;
        }
    }

    // 皮肤选择界面绘制
    void DrawChangeSkin()
    {
        DrawFPS(10, 10);
        ui.DrawTextTip("Select Snake Skin", 400, 200, 48, DARKGREEN);
        // 绘制所有皮肤选择按钮和预览图
        for (int i = 0; i < texRes.MAX_SKINS; i++)
        {
            ui.DrawButton({ 500.0f, 300.0f + i * 60.0f, 200.0f, 50.0f }, (string("Skin ") + to_string(i + 1)).c_str());
            // 绘制皮肤预览（蛇头+身体+尾巴）
            DrawTexture(texRes.Headright[i], 830, 300.0f + i * 60.0f, WHITE);
            DrawTexture(texRes.bodyhend[i], 790, 300.0f + i * 60.0f, WHITE);
            DrawTexture(texRes.tailleft[i], 750, 300.0f + i * 60.0f, WHITE);
        }
        // 绘制返回主菜单按钮
        ui.DrawButton({ 480.0f, 300.0f + texRes.MAX_SKINS * 60.0f, 240.0f, 50.0f }, "Back to Main Menu");
    }
};

//=================================== 主函数入口 ===================================
/**
 * @brief 程序入口函数
 * 创建游戏主控对象并启动游戏主循环
 * @return int 程序退出码
 */
int main()
{
    SnakeGameMain game; // 创建游戏主控对象
    game.Run(); // 启动游戏主循环
    return 0;
}