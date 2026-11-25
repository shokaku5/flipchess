# include <iostream>
# include <string>

using namespace std;

char origin_map[8][8];
char now_map[5][8][8];
bool map2[5][8][8];
int black, white;
struct moveEcho{
    int x, y;
    int rev[8];                 //记录八个方向的扫描情况。
    char color;
};
moveEcho moveHistory[5][70];
int X1, Y1;
int now_game, move_count;                    //当前是哪个

void clear(int present, int steps);
void draw(int present);
int judge(int present, int steps, int x, int y, char color);
bool gameIsValid(int present, char color);
void flip(int present, int steps, int x, int y, char color);
void countChess(int present);
char undoMove(int present, int steps, int x, int y);

int main()
{
    char op = '0', color = 'B', special_op = '0';
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            origin_map[i][j] = '.';
        }
    }
    origin_map[3][3] = 'W';
    origin_map[3][4] = 'B';
    origin_map[4][3] = 'B';
    origin_map[4][4] = 'W';
    clear(now_game, move_count);
    cout << "欢迎游玩黑白棋游戏！" << endl;
    draw(now_game);
    cout << "请输入你想进行的操作：P（开始游戏）/Q（退出游戏）" << endl;
    while(1)
    {
        cin >> op;
        if (op == 'Q')
        {
            cout << "感谢游玩黑白棋游戏！期待您下次光临！" << endl;
            break;
        }
        while(gameIsValid(now_game, color))
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
                {
                    char temp = undoMove(now_game, move_count, moveHistory[now_game][move_count].x, moveHistory[now_game][move_count].y);
                    if (temp != ' ')
                    {
                        color = temp;
                        move_count--;
                        draw(now_game);
                        continue;
                    }
                    else
                    {
                        cout << "悔棋失败，当前局面非法！将立即进行结算。" << endl;
                        break;
                    }
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
        }
    }
    return 0;
}

void clear (int present, int steps)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            now_map[present][i][j] = '.';
        }
    }
    for(int i = 0; i < steps; i++)
    {
        moveHistory[present][i].x = 0;
        moveHistory[present][i].y = 0;
        moveHistory[present][i].color = '.';
        for(int j = 0; j < 8; j++)
        {
            moveHistory[present][i].rev[j] = 0;
        }
    }
    now_map[present][3][3] = 'W';
    now_map[present][3][4] = 'B';
    now_map[present][4][3] = 'B';
    now_map[present][4][4] = 'W';
}

void draw (int present)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            cout << now_map[present][i][j] << ' ';
        }
        cout << endl;
    }
}

int judge(int present, int steps, int x, int y, char color)
{
    int count = 0, ans = 0, pos = 0;
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue;
            int nx = x + i;
            int ny = y + j;
            while(nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
            {
                if(now_map[present][nx][ny] == color)                        //如果遇到同色棋子，停止计数。
                {
                    if(count > 0)                               //遇上了可以翻转的棋子，flag置为true，ans加上count，count清零
                    {
                        ans += count;
                        moveHistory[present][steps].rev[pos] = count;
                        count = 0;
                    }
                    break;
                }
                else if(now_map[present][nx][ny] == '.')                     //遇上了空格，说明此方向不能翻转棋子，count清零，停止计数。
                {
                    count = 0;
                    break;
                }
                count++;                                        //遇上了异色棋子，count加1，继续计数。
                nx += i;
                ny += j;
            }
            if(count > 0) count = 0; 
            pos++;
        }
    }
    return ans;
}

bool gameIsValid(int present, char color)
{
    bool flag = false;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(now_map[present][i][j] == '.')
            {
                if(judge(present, move_count, i, j, color) > 0)
                {
                    map2[present][i][j] = true;
                    flag = true;
                }
                else
                {
                    map2[present][i][j] = false;
                }
            }
            else
            {
                map2[present][i][j] = false;
            }
        }
    }
    return flag;
}

void flip(int present, int steps, int x, int y, char color)
{
    now_map[present][x][y] = color;
    int pos = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0) continue;
            for(int k = 1; k <= moveHistory[present][steps].rev[pos]; k++)
            {
                now_map[present][x + k * i][y + k * j] = color;
            }
            pos++;
        }
    }
}

void countChess(int present)
{
    black = 0, white = 0;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(now_map[present][i][j] == 'B') black++;
            else if(now_map[present][i][j] == 'W') white++;
        }
    }
    cout << "黑棋棋子数：" << black << endl;
    cout << "白棋棋子数：" << white << endl;
    if (black > white) cout << "可恶，就差一点点……" << endl;
    else if (white > black) cout << "菜就多练" << endl;
    else cout << "棋逢对手，将遇良才。痛快！" << endl;
}

char undoMove(int present, int steps, int x, int y)
{
    char colorUndo = now_map[present][x][y];
    now_map[present][x][y] = '.';
    if(gameIsValid(present, colorUndo))
    {
        int pos = 0;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0) continue;
                int nx = x + i, ny = y + j;
                while(moveHistory[present][steps].rev[pos] > 0)
                {
                    if(now_map[present][nx][ny] == 'B') now_map[present][nx][ny] = 'W';
                    else now_map[present][nx][ny] = 'B';
                    moveHistory[present][steps].rev[pos]--;
                    nx += i;
                    ny += j;
                }
                pos++;
            }
        }
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