# include <iostream>
# include <string>
# include <limits>

using namespace std;

char map[3][64][8][8]; //下棋的棋子图
char level[5][8][8]; //准备的关卡数目
bool map2[8][8]; //判断此处是否可以落子
int black, white; //统计黑白棋子的数量
int X1,Y1; //记录上一次落子的位置
int now_game = 0, moveCount = 0;         //记录当前局对应的数组第一维与落子次数

struct moveEcho
{
    int x, y;
    char color;
};
moveEcho moveHistory[70]; //记录每一步的落子位置和颜色

void clear(int present);                    //还原棋盘
void draw(int present, int k, int x, int y, char color); //画出本轮落子后的棋盘
int judge(int presesnt, int k, int x, int y, char color); //判断是否可以使用
bool gameIsValid(int present, int k, char color);             //判断此处是否可以落子
void flip(int present, int k, int x, int y, char color);  //翻转棋子
void countChess(int present, int k);                  //统计黑白棋子的数量
char undoMove(int present, int k, int x, int y, bool flag);                      //悔棋

int main()
{
    char op = '0', color = 'B', special_op = '0';
    clear(now_game);
    cout << "欢迎游玩黑白棋游戏！" << endl;
    draw(now_game, moveCount, -1, -1, '0');
    cout << "请输入你想进行的操作：P（开始游戏）/Q（退出游戏）" << endl;

    while (1)
    {
        if (!(cin >> op)) break;

        if (op == 'Q')
        {
            cout << "感谢游玩黑白棋游戏！期待您下次光临！" << endl;
            break;
        }
        if (op != 'P')
        {
            cout << "请输入 P（开始游戏）或 Q（退出游戏）！" << endl;
            continue;
        }

        color = 'B'; // 每局开始由黑棋先手

        while (moveCount < 60) // 黑白棋初始占四格，最多再落 60 子
        {
            // 修复1：每轮按当前颜色和当前棋盘重新计算 map2
            bool flag = gameIsValid(now_game, moveCount, color);
            if (!flag)
            {
                // 一方没有合法落点时，按照黑白棋规则跳过该方
                if (gameIsValid(now_game, moveCount, (color == 'B') ? 'W' : 'B'))
                {
                    cout << color << "方无合法落子位置，跳过此回合。" << endl;
                    color = (color == 'B') ? 'W' : 'B';
                    continue;
                }

                // 双方都没有合法落点，游戏自然结束
                cout << "双方均无合法落子位置！" << endl;
                break;
            }

            cout << "您是否要进行特殊操作？ R. 悔棋 T. 打开作弊模式 Y. 立刻结算 U. 查看回放" << endl;
            if (!(cin >> special_op)) return 0;

            if (special_op == 'Y') break;

            switch (special_op)
            {
                case 'R':
                {
                    // 修复2：只执行一次悔棋，不读取 moveHistory[-1]
                    char colorUndo = undoMove(now_game, moveCount, X1, Y1, flag);
                    if (colorUndo != ' ')
                    {
                        color = colorUndo;
                        draw(now_game, moveCount, -1, -1, '0');
                    }
                    continue;
                }
                case 'T':
                    cout << "作弊模式暂未开放，请重新输入落子位置！" << endl;
                    continue;
                case 'U':
                    cout << "查看回放功能暂未开放，请重新输入落子位置！" << endl;
                    continue;
                default:
                    cout << "未检测到特殊操作指令，将进行正常操作。" << endl;
            }

            cout << "当前轮到" << color << "方落子，请输入落子位置（行 列）：" << endl;
            if (!(cin >> X1 >> Y1))
            {
                if (cin.eof()) return 0;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "请输入两个整数坐标！" << endl;
                continue;
            }

            if (X1 < 0 || X1 >= 8 || Y1 < 0 || Y1 >= 8)
            {
                cout << "坐标超出棋盘范围，请重新输入！" << endl;
                continue;
            }

            if (map2[X1][Y1])
            {
                // 修复3：只记录真正成功的落子；记录的是落子前的棋子颜色
                moveHistory[moveCount].x = X1;
                moveHistory[moveCount].y = Y1;
                moveHistory[moveCount].color = color;

                // flip 在 map[present][k+1] 上生成新局面，不破坏历史快照 k
                flip(now_game, moveCount, X1, Y1, color);
                moveCount++;
                draw(now_game, moveCount, -1, -1, '0');

                color = (color == 'B') ? 'W' : 'B';
            }
            else
            {
                cout << "此处不能落子，请重新输入！" << endl;
                // 非法落子不能增加 moveCount
            }
        }

        countChess(now_game, moveCount);
        cout << "游戏结束！" << endl;

        // 修复4：原 map 第一维只有 3，循环使用其空间防止越界
        moveCount = 0;
        now_game = (now_game + 1) % 3;
        color = 'B';
        clear(now_game);
        draw(now_game, moveCount, -1, -1, '0');
        cout << "请输入你想进行的操作：P（开始游戏）/Q（退出游戏）" << endl;
    }

    return 0;
}

void clear(int present)//还原棋盘
{
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            map[present][0][i][j] = '.';
            map2[i][j] = false;
        }
    }
    map[present][0][3][3] = 'W';
    map[present][0][3][4] = 'B';
    map[present][0][4][3] = 'B';
    map[present][0][4][4] = 'W';
    
}

void draw(int present, int k, int x, int y, char color)
{
    if(x >= 0 && x < 8 && y >= 0 && y < 8)
    {
        map[present][k][x][y] = color;
    }
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            cout << map[present][k][i][j] << " ";
            map[present][k+1][i][j] = map[present][k][i][j];
        }
        cout << endl;
    }
}

int judge(int present, int k, int x, int y, char color)
{
    int count = 0, ans=0;                                              //统计可以翻转的棋子数量
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            if(i == 0 && j == 0) continue;
            int nx = x + i;
            int ny = y + j;
            while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                if(map[present][k][nx][ny] == color)                        //如果遇到同色棋子，停止计数。
                {
                    if(count > 0)                               //遇上了可以翻转的棋子，flag置为true，ans加上count，count清零
                    {
                        ans += count;
                        count = 0;
                    }
                    break;
                }
                else if(map[present][k][nx][ny] == '.')                     //遇上了空格，说明此方向不能翻转棋子，count清零，停止计数。
                {
                    count = 0;
                    break;
                }
                count++;                                        //遇上了异色棋子，count加1，继续计数。
                nx += i;
                ny += j;
            }
            if(count > 0) count = 0;                            //如果遇上了边界，说明此方向不能翻转棋子，count清零
        }
    }
    return ans;
}

bool gameIsValid(int present, int k, char color)
{
    bool flag = false; //标记是否有可以落子的地方
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(map[present][k][i][j] == '.')                             //如果此处为空格，判断是否可以落子
            {
                if(judge(present, k, i, j, color) > 0)                   //如果可以翻转棋子，说明此处可以落子
                {
                    map2[i][j] = true;
                    flag = true;
                }
                else                                         //如果不能翻转棋子，说明此处不能落子
                {
                    map2[i][j] = false;
                }
            }
            else                                             //如果此处不为空格，说明此处不能落子
            {
                map2[i][j] = false;
            }
        }
    }
    return flag; //返回是否有可以落子的地方
}

void flip(int present, int k, int x, int y, char color)
{
    // 保留 k 对应的旧棋盘，先复制到 k+1，便于悔棋时恢复
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            map[present][k+1][i][j] = map[present][k][i][j];
        }
    }

    map[present][k+1][x][y] = color;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue;
            int nx = x + i;
            int ny = y + j;
            int count = 0;

            while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                if (map[present][k+1][nx][ny] == color)
                {
                    if (count > 0)
                    {
                        for (; count >= 1; count--)
                        {
                            map[present][k+1][x + count * i][y + count * j] = color;
                        }
                    }
                    break;
                }
                else if (map[present][k+1][nx][ny] == '.')
                {
                    count = 0;
                    break;
                }

                count++;
                nx += i;
                ny += j;
            }
        }
    }
}

void countChess(int present, int k)
{
    black = 0;
    white = 0;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        { 
            if(map[present][k][i][j] == 'B') black++;
            else if(map[present][k][i][j] == 'W') white++;
        }
    }
    cout << "黑棋棋子数：" << black << endl;
    cout << "白棋棋子数：" << white << endl;
    if (black > white) cout << "可恶，就差一点点……" << endl;
    else if (white > black) cout << "菜就多练" << endl;
    else cout << "棋逢对手，将遇良才。痛快！" << endl;
}

char undoMove(int present, int k, int x, int y, bool flag)
{
    // 保留原参数表；棋盘快照足以完成悔棋，无需手工逐颗翻回
    (void)present;
    (void)x;
    (void)y;
    (void)flag;

    if (k <= 0)
    {
        cout << "尚未落子，无法悔棋！" << endl;
        return ' ';
    }

    char colorUndo = moveHistory[k-1].color;
    moveCount--; // 回到上一张棋盘快照
    cout << "悔棋成功！" << endl;
    return colorUndo;
}
