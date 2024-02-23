#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>

/*  
    ncurses库安装：
    apt-get install libncursesw5-dev
    编译：
    gcc demoNcursesw.c -lncursesw
*/

#if 0
int main() {
    // 设置编码格式为utf-8
    setlocale(LC_ALL, "");

    // 初始化 ncurses 模式
    initscr();

    // 设置颜色
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    // cbreak();  // 禁用行缓冲（用户按下一个键立即生效）
    noecho();  // 关闭回显


    // 打印提示信息
    printw("Press any key to continue...\n");
    refresh();  // 刷新屏幕
    keypad(stdscr, TRUE);  // 启用功能键支持
    int ch;
    attron(COLOR_PAIR(1));
    printw("q key Exit\n");
    attroff(COLOR_PAIR(1));
    printw("中文测试\n");
    while ((ch = getch()) != 'q') {  // 输入 'q' 退出循环
        switch (ch) {
            case KEY_UP:
                printw("Up arrow key pressed.\n");
                break;
            case KEY_DOWN:
                printw("Down arrow key pressed.\n");
                refresh();  // 刷新屏幕
                char str[100];
                scanf("%s", str);
                printw("%s\n", str);
                break;
            case KEY_LEFT:
                printw("Left arrow key pressed.\n");
                break;
            case KEY_RIGHT:
                printw("Right arrow key pressed.\n");
                break;
            default:
                printw("You pressed '%c' (ASCII code: %d)\n", ch, ch);
                break;
        }
        refresh();  // 刷新屏幕
    }

    // 关闭 ncurses 模式
    endwin();

    return 0;
}
#elif 1


#define INPUT_HEIGHT 3
#define CHAT_HEIGHT (LINES - INPUT_HEIGHT)

int main() {
    setlocale(LC_ALL, "");

    // 初始化 ncurses 模式
    initscr();

    start_color();  // 启用颜色功能
    init_pair(1, COLOR_BLUE, COLOR_RED);  // 定义颜色对，红色背景，黑色前景

    // 创建窗口
    WINDOW *input_win = newwin(INPUT_HEIGHT, COLS, LINES - INPUT_HEIGHT, 0);
    WINDOW *chat_win = newwin(CHAT_HEIGHT, COLS, 0, 0);

    // 设置窗口属性
    box(chat_win, 0, 0);
    scrollok(chat_win, TRUE);   // 设置卷动
    wbkgd(chat_win, COLOR_PAIR(1));   // 设置窗口背景色为红色
    
    keypad(input_win, TRUE);  // 启用功能键支持
    cbreak();  // 禁用行缓冲（用户按下一个键立即生效）

    // 打印提示信息
    mvwprintw(chat_win, 1, 0, "按 'q'键退出\n");
    
    noecho();  // 关闭回显

    // 刷新窗口
    wrefresh(chat_win);
    wrefresh(input_win);

    // 输入循环
    int ch;
    char input_buffer[COLS];    // COLS是 ncurses 库提供的一个预定义宏，表示当前终端窗口的列数。
    memset(input_buffer, 0, sizeof(input_buffer));
    int buffer_index = 0;
    while (1) 
    {
        // wprintw(chat_win, "buffer:%s\n", input_buffer);
        ch = wgetch(input_win);
        if (strcmp(input_buffer, "q") == 0 && ch == '\n')
        {
            break;
        }
        // wprintw(chat_win, "按下了%d\n", ch);
        switch(ch)
        {
            case '\n':
            {
                // 回车键

                wprintw(chat_win, "|You: %s\n", input_buffer);
                wrefresh(chat_win);
                wscrl(chat_win, 1);  // 向上滚动一个行
                memset(input_buffer, 0, sizeof(input_buffer));
                buffer_index = 0;
                wclear(input_win);
                wrefresh(input_win);
                break;
            }
            case 263:
            {
                // 退格键
                if (buffer_index > 0) {
                    buffer_index--;
                    input_buffer[buffer_index] = '\0';
                    wclear(input_win);
                    wprintw(input_win, "%s", input_buffer);
                    wrefresh(input_win);
                }
                break;
            }
            case 258:
            {
                // 下方向键
                wscrl(chat_win, 1);
                wrefresh(chat_win);
                break;
            }
            case 259:
            {
                // 上方向键
                wscrl(chat_win, -1);
                wrefresh(chat_win);
                break;
            }
            case 260:
            {
                // 左方向键
                wscrl(chat_win, -COLS);
                wrefresh(chat_win);
                break;
            }
            case 261:
            {
                // 右方向键
                wscrl(chat_win, COLS);
                wrefresh(chat_win);
                break;
            }
            default:
            {
                wprintw(input_win, "%c", ch);
                wrefresh(input_win);
                input_buffer[buffer_index++] = ch;
                break;
            }
        }
    }

    // 清理窗口
    delwin(input_win);
    delwin(chat_win);

    // 关闭 ncurses 模式
    endwin();

    return 0;
}
#endif 