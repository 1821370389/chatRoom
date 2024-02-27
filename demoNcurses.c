#include <ncurses.h>
#include <unistd.h>

/*  
    ncurses库安装：
    apt-get install libncurses-dev 
    编译：
    gcc demoNcurses.c -o demoNcurses -lncurses
*/

int main() {
    // 初始化 ncurses 模式
    initscr();
#if 0
    // 在屏幕上打印 "Hello, ncurses!"
    printw("Hello, ncurses!\n");

    // 刷新屏幕以显示文本
    refresh();
    for(int idx = 0; idx < 10; idx++) {
        sleep(1);
        printw("Hello, ncurses! %d\n", idx);
    }
        refresh();
    // 等待用户按下任意键
    getch();
#endif
    cbreak();  // 禁用行缓冲（用户按下一个键立即生效）
    noecho();  // 关闭回显

    // 打印提示信息
    printw("Press any key to continue...\n");
    refresh();  // 刷新屏幕
    keypad(stdscr, TRUE);  // 启用功能键支持
    int ch;
    printw("q key Exit\n");
    while ((ch = getch()) != 'q') {  // 输入 'q' 退出循环
        switch (ch) {
            case KEY_UP:
                printw("Up arrow key pressed.\n");
                break;
            case KEY_DOWN:
                printw("Down arrow key pressed.\n");
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
