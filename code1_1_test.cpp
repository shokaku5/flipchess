# include <iostream>
# include <string>

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
    while(1)
    {
        cin >> op;
        if (op == 'Q')
        {
            cout << "感谢游玩黑白棋游戏！期待您下次光临！" << endl;
            break;
        }
        bool flag = gameIsValid(now_game, moveCount, color);
        while(flag)
        {
            cout << "您是否要进行特殊操作？ R. 悔棋 T. 打开作弊模式 Y. 立刻结算 U. 查看回放" << endl; //提示用户是否要进行特殊操作，U暂时不写
            cin >> special_op;
            if(special_op == 'Y')
            {
                break;
            }
            switch(special_op)
            {
                case 'R':
                    if (undoMove(now_game, moveCount, moveHistory[moveCount-1].x, moveHistory[moveCount-1].y, flag) == ' ')
                    {
                        cout << "将立即进行结算。" << endl;
                        break;
                    }
                    else
                    {
                        color = undoMove(now_game, moveCount, X1, Y1, flag);
                        draw(now_game, moveCount, -1, -1, '0');
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
            cin >> X1 >> Y1;
            if(X1 < 0 || X1 >= 8 || Y1 < 0 || Y1 >= 8)                                 //判断数组坐标是否越界
            {
                cout << "坐标超出棋盘范围，请重新输入！" << endl;
                continue;
            }
            if(map2[X1][Y1])
            {
                flip(now_game, moveCount, X1, Y1, color);
                draw(now_game, moveCount, X1, Y1, color);
                color = (color == 'B') ? 'W' : 'B';
            }
            else
            {
                cout << "此处不能落子，请重新输入！" << endl;
            }
            moveHistory[moveCount].x = X1, moveHistory[moveCount].y = Y1, moveHistory[moveCount].color = color;
            moveCount++;
            flag = gameIsValid(now_game, moveCount, color);
        }
        countChess(now_game, moveCount);
        cout << "游戏结束！" << endl;
        cout << "请输入你想进行的操作：P（开始游戏）/Q（退出游戏）" << endl;
        moveCount = 0, now_game++;
        clear(now_game);
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
    map[present][k][x][y] = color;
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
                if (map[present][k][nx][ny] == color)
                {
                    if (count > 0)
                    {
                        // 翻转棋子
                        for (; count>=1; count--)
                        {
                            map[present][k][x + count * i][y + count * j] = color;
                        }
                    }
                    break;
                }
                else if (map[present][k][nx][ny] == '.')
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
    char colorUndo = map[present][k][x][y];
    map[present][k][x][y] = '.';
    if(flag) 
    {
        colorUndo = moveHistory[moveCount].color;
        moveCount--;
        cout << "悔棋成功！" << endl;
        return colorUndo;
    }
    else
    {
        cout << "当前局面非法！该局比赛结束。" << endl;
        return ' ';
    }
    return ' ';
}

