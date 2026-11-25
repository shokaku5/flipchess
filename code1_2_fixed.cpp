#include <iostream>
#include <limits>
#include <cctype>

using namespace std;

// 保留原有全局变量及数据结构
char origin_map[8][8];
char now_map[5][8][8];
bool map2[5][8][8];
int black, white;

struct moveEcho {
    int x, y;
    int rev[8];                 // 八个方向各翻转了多少枚棋子
    char color;
};

moveEcho moveHistory[5][70];
int X1, Y1;
int now_game, move_count;

bool cheatMode = false;         // T：是否显示合法落子提示

// 保留原有函数名和参数
void clear(int present, int steps);
void draw(int present);
int judge(int present, int steps, int x, int y, char color);
bool gameIsValid(int present, char color);
void flip(int present, int steps, int x, int y, char color);
void countChess(int present);
char undoMove(int present, int steps, int x, int y);

// 仅新增两个内部辅助函数：边界检查、扫描八个方向
bool inBoard(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

int scanDirections(int present, int x, int y, char color, int rev[8]) {
    for (int k = 0; k < 8; ++k) rev[k] = 0;

    if (present < 0 || present >= 5 || !inBoard(x, y) ||
        (color != 'B' && color != 'W') ||
        now_map[present][x][y] != '.') {
        return 0;
    }

    const char opposite = (color == 'B') ? 'W' : 'B';
    int ans = 0, pos = 0;

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;

            int nx = x + i, ny = y + j;
            int count = 0;      // 每个方向都必须重新计数

            while (inBoard(nx, ny) && now_map[present][nx][ny] == opposite) {
                ++count;
                nx += i;
                ny += j;
            }

            // 必须先经过异色棋子，再以本方棋子封口
            if (count > 0 && inBoard(nx, ny) &&
                now_map[present][nx][ny] == color) {
                rev[pos] = count;
                ans += count;
            }
            ++pos;
        }
    }
    return ans;
}

int main() {
    char op = '0', color = 'B', special_op = '0';

    now_game = 0;
    move_count = 0;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            origin_map[i][j] = '.';
        }
    }
    origin_map[3][3] = 'W';
    origin_map[3][4] = 'B';
    origin_map[4][3] = 'B';
    origin_map[4][4] = 'W';

    clear(now_game, move_count);

    cout << "欢迎游玩黑白棋游戏！\n";
    cout << "B=黑棋，W=白棋，.=空位，*=可落子位置（开启 T 后显示）\n";
    cout << "落子坐标使用 0~7，先输入行号，再输入列号。\n";
    draw(now_game);

    while (true) {
        cout << "\n主菜单：P 开始新游戏 / Q 退出游戏：";
        if (!(cin >> op)) break;
        op = static_cast<char>(toupper(static_cast<unsigned char>(op)));

        if (op == 'Q') {
            cout << "感谢游玩黑白棋游戏！期待您下次光临！\n";
            break;
        }
        if (op != 'P') {
            cout << "无效指令，请输入 P 或 Q。\n";
            continue;
        }

        color = 'B';
        move_count = 0;
        cheatMode = false;
        clear(now_game, move_count);
        cout << "\n新的一局开始！黑棋先手。\n";

        bool gameOver = false;
        while (!gameOver) {
            // 双方都没有合法落子时才结束；只有当前方没有时自动跳过
            if (!gameIsValid(now_game, color)) {
                char opposite = (color == 'B') ? 'W' : 'B';
                if (!gameIsValid(now_game, opposite)) {
                    cout << "\n双方都没有合法落子位置，本局结束！\n";
                    countChess(now_game);
                    break;
                }
                cout << "\n" << (color == 'B' ? "黑棋" : "白棋")
                     << "无合法落子位置，自动跳过本回合。\n";
                color = opposite;
            }

            gameIsValid(now_game, color); // 刷新当前玩家的 map2
            cout << "\n当前轮到" << (color == 'B' ? "黑棋 B" : "白棋 W")
                 << "，已经落子 " << move_count << " 步。\n";
            draw(now_game);

            cout << "请选择：M 正常落子 / R 悔棋 / T 合法位置提示 / "
                 << "U 回放 / Y 立即结算 / Q 退出：";
            if (!(cin >> special_op)) {
                cout << "\n输入结束，程序正常退出。\n";
                return 0;
            }
            special_op = static_cast<char>(
                toupper(static_cast<unsigned char>(special_op)));

            switch (special_op) {
                case 'R': {
                    if (move_count <= 0) {
                        cout << "当前没有可以撤销的棋步！\n";
                        break;
                    }

                    int last = move_count - 1;
                    char temp = undoMove(
                        now_game, last,
                        moveHistory[now_game][last].x,
                        moveHistory[now_game][last].y
                    );

                    if (temp != ' ') {
                        color = temp;      // 轮到刚刚被撤销的落子方
                        --move_count;
                    } else {
                        cout << "悔棋失败，棋盘未被修改。\n";
                    }
                    break;
                }

                case 'T':
                    cheatMode = !cheatMode;
                    cout << (cheatMode ? "合法落子位置提示已开启。\n"
                                     : "合法落子位置提示已关闭。\n");
                    break;

                case 'U':
                    cout << "回放功能暂未开放，不影响正常对局。\n";
                    break;

                case 'Y':
                    cout << "\n提前结束本局，当前成绩如下：\n";
                    countChess(now_game);
                    gameOver = true;
                    break;

                case 'Q':
                    cout << "感谢游玩黑白棋游戏！期待您下次光临！\n";
                    return 0;

                case 'M': {
                    if (move_count >= 60 || move_count >= 70) {
                        cout << "达到最大落子数，本局结束。\n";
                        countChess(now_game);
                        gameOver = true;
                        break;
                    }

                    cout << "请输入行号和列号（0~7，例如 2 3）：";
                    if (!(cin >> X1 >> Y1)) {
                        if (cin.eof()) {
                            cout << "\n输入结束，程序正常退出。\n";
                            return 0;
                        }
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "坐标必须是整数，请重新选择 M。\n";
                        break;
                    }

                    if (!inBoard(X1, Y1)) {
                        cout << "坐标超出 0~7 的范围！\n";
                        break;
                    }
                    if (judge(now_game, move_count, X1, Y1, color) <= 0) {
                        cout << "该位置不能落子，请重新选择。\n";
                        break;
                    }

                    // flip 负责一次性记录真实棋谱并完成翻转
                    flip(now_game, move_count, X1, Y1, color);
                    if (moveHistory[now_game][move_count].color != color) {
                        cout << "落子失败，棋盘未被修改。\n";
                        break;
                    }

                    ++move_count;
                    color = (color == 'B') ? 'W' : 'B';
                    break;
                }

                default:
                    cout << "无效指令，请使用 M/R/T/U/Y/Q。\n";
                    break;
            }
        }

        // 五个棋盘槽循环使用；开始新局前会重新清空棋盘和历史
        now_game = (now_game + 1) % 5;
    }
    return 0;
}

void clear(int present, int steps) {
    if (present < 0 || present >= 5) return;
    (void)steps;               // 保留原函数形参，每次均重置整局棋谱

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            now_map[present][i][j] = origin_map[i][j];
            map2[present][i][j] = false;
        }
    }
    for (int k = 0; k < 70; ++k) {
        moveHistory[present][k] = moveEcho{};
        moveHistory[present][k].color = '.';
    }
}

void draw(int present) {
    if (present < 0 || present >= 5) return;

    cout << "    0 1 2 3 4 5 6 7\n";
    for (int i = 0; i < 8; ++i) {
        cout << " " << i << "  ";
        for (int j = 0; j < 8; ++j) {
            char ch = now_map[present][i][j];
            if (cheatMode && ch == '.' && map2[present][i][j])
                ch = '*';
            cout << ch << ' ';
        }
        cout << '\n';
    }
}

int judge(int present, int steps, int x, int y, char color) {
    (void)steps;
    int rev[8] = {};
    // 仅判断是否合法，绝不改写 moveHistory
    return scanDirections(present, x, y, color, rev);
}

bool gameIsValid(int present, char color) {
    if (present < 0 || present >= 5 ||
        (color != 'B' && color != 'W')) {
        return false;
    }

    bool flag = false;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            map2[present][i][j] =
                judge(present, move_count, i, j, color) > 0;
            if (map2[present][i][j]) flag = true;
        }
    }
    return flag;
}

void flip(int present, int steps, int x, int y, char color) {
    if (present < 0 || present >= 5 ||
        steps < 0 || steps >= 70) return;

    int rev[8] = {};
    if (scanDirections(present, x, y, color, rev) <= 0) return;

    // 先保存全部八个方向的翻转长度，再改棋盘
    moveHistory[present][steps].x = x;
    moveHistory[present][steps].y = y;
    moveHistory[present][steps].color = color;
    for (int k = 0; k < 8; ++k)
        moveHistory[present][steps].rev[k] = rev[k];

    now_map[present][x][y] = color;
    int pos = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            for (int k = 1; k <= rev[pos]; ++k) {
                int nx = x + k * i, ny = y + k * j;
                if (inBoard(nx, ny))
                    now_map[present][nx][ny] = color;
            }
            ++pos;
        }
    }
}

void countChess(int present) {
    if (present < 0 || present >= 5) return;

    black = 0;
    white = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (now_map[present][i][j] == 'B') ++black;
            else if (now_map[present][i][j] == 'W') ++white;
        }
    }
    cout << "黑棋棋子数：" << black << '\n';
    cout << "白棋棋子数：" << white << '\n';

    if (black > white) cout << "黑棋获胜！\n";
    else if (white > black) cout << "白棋获胜！\n";
    else cout << "平局！棋逢对手，将遇良才。\n";
}

char undoMove(int present, int steps, int x, int y) {
    if (present < 0 || present >= 5 ||
        steps < 0 || steps >= 70 ||
        steps != move_count - 1 || !inBoard(x, y)) {
        return ' ';
    }

    const moveEcho &history = moveHistory[present][steps];
    char colorUndo = history.color;
    if ((colorUndo != 'B' && colorUndo != 'W') ||
        history.x != x || history.y != y ||
        now_map[present][x][y] != colorUndo) {
        return ' ';
    }

    bool hasFlip = false;
    int pos = 0;

    // 完整检查棋谱；检查全部通过后，才允许更改棋盘
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;

            int n = history.rev[pos++];
            if (n < 0 || n > 6) return ' ';
            if (n == 0) continue;
            hasFlip = true;

            for (int k = 1; k <= n + 1; ++k) {
                int nx = x + k * i, ny = y + k * j;
                if (!inBoard(nx, ny) ||
                    now_map[present][nx][ny] != colorUndo) {
                    return ' ';
                }
            }
        }
    }
    if (!hasFlip) return ' ';

    char opposite = (colorUndo == 'B') ? 'W' : 'B';
    now_map[present][x][y] = '.';

    pos = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            for (int k = 1; k <= history.rev[pos]; ++k) {
                now_map[present][x + k * i][y + k * j] = opposite;
            }
            ++pos;
        }
    }

    // 这一步已撤销，清掉记录以便在同一下标重新落子
    moveHistory[present][steps] = moveEcho{};
    moveHistory[present][steps].color = '.';

    cout << "悔棋成功！\n";
    return colorUndo;
}
