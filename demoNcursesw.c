#include <ncursesw/ncurses.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>

/*  
    ncurses库安装：
    apt-get install libncursesw5-dev
    编译：
    gcc demoNcurses.c -o demoNcurses -lncursesw
*/

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
