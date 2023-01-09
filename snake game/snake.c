#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>

#define BLACK   0
#define BLUE1   1
#define GREEN1   2
#define CYAN1   3
#define RED1   4
#define MAGENTA1 5
#define YELLOW1   6
#define GRAY1   7
#define GRAY2   8
#define BLUE2   9
#define GREEN2   10
#define CYAN2   11
#define RED2   12
#define MAGENTA2 13
#define YELLOW2   14
#define WHITE   15

#define BLANK ' '

#define ESC   0x1b

#define SPECIAL1 0xe0 
#define SPECIAL2 0x00

#define UP  0x48 
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define UP2     'w'
#define DOWN2   's'
#define LEFT2   'a'
#define RIGHT2  'd'

#define SNAKE_LENGTH 6
#define SNAKE_LENGTH_MAX 20

#define WIDTH 120
#define HEIGHT 40

int key, g, G;
int snake_length = SNAKE_LENGTH;
int snake1_length = SNAKE_LENGTH;
int Delay = 100;
int time_out = 60;
int time_out1 = 60;
int score = 0, score1 = 0;
int hearts[WIDTH][HEIGHT] = { 0 };
int heartinterval = 5;
int xs[WIDTH][HEIGHT] = { 0 };
int xinterval = 7;
int called[2];
int keep_moving = 1;
int frame_count = 0;
int p1_frame_sync = 10;
int p1_frame_sync_start = 0;
int p2_frame_sync = 10;
int p2_frame_sync_start = 0;
int p3_frame_sync = 10;
int p3_frame_sync_start = 0;
int p4_frame_sync = 10;
int p4_frame_sync_start = 0;
int heart_frame_sync = 50;
int mi_frame_sync = 50;
int hearts_count = 0;
int mi_count = 0;


COORD snake[SNAKE_LENGTH_MAX];
COORD snake1[SNAKE_LENGTH_MAX];

void gotoxy(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void gotoxy2(COORD pos) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void erasestar(int x, int y) {
    gotoxy(x, y);
    putchar("  ");
}

void textcolor(int fg_color, int bg_color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void removeCursor(void) {
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void showCursor(void) {
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void cls(int text_color, int bg_color) {
    char cmd[100];
    system("cls");
    sprintf(cmd, "COLOR %x%x", bg_color, text_color);
    system(cmd);
}

void showscore(int player) {
    switch (player) {
    case 0:
        gotoxy(14, 0);
        textcolor(YELLOW1, BLACK);
        printf("Player1 : %d", score);
        break;
    case 1:
        gotoxy(95, 0);
        textcolor(CYAN1, BLACK);
        printf("Player2 : %d", score1);
        break;
    }
}

void flush_key() {
    while (kbhit())
        getch;
}

#define HEART "♥"
void show_heart() {
    int x, y;
    x = rand() % (WIDTH - 30) + 20;
    y = rand() % (HEIGHT - 20) + 10;
    textcolor(RED1, BLACK);
    gotoxy(x, y);
    printf(HEART);
    hearts[x][y] = 1;
    hearts_count++;
    textcolor(BLACK, WHITE);
}

#define mi "★"
void show_x() {
    int x, y;
    x = rand() % (WIDTH - 30) + 20;
    y = rand() % (HEIGHT - 20) + 10;
    textcolor(MAGENTA1, BLACK);
    gotoxy(x, y);
    printf(mi);
    xs[x][y] = 1;
    mi_count++;
    textcolor(BLACK, WHITE);
}

void draw_box2(int x1, int y1, int x2, int y2, char* ch) {
    int x, y;
    for (x = x1; x <= x2; x += 2) {
        gotoxy(x, y1);
        printf("%s", ch);
        gotoxy(x, y2);
        printf("%s", ch);
    }
    for (y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        printf("%s", ch);
        gotoxy(x2, y);
        printf("%s", ch);
    }
}

void draw_hline(int y, int x1, int x2, char ch) {
    gotoxy(x1, y);
    for (; x1 <= x2; x1++)
        putchar(ch);
}

void init_game() {
    int x, y;
    char cmd[100];

    srand(time(NULL));
    called[0] = called[1] = 0;
    for (x = 0; x < WIDTH; x++)
        for (y = 0; y < HEIGHT; y++) {
            hearts[x][y] = 0;
            xs[x][y] = 0;
        }
    snake_length = SNAKE_LENGTH;
    snake1_length = SNAKE_LENGTH;
    heartinterval = 5;
    xinterval = 7;
    time_out = 60;
    time_out1 = 60;
    keep_moving = 1;
    Delay = 100;
    p1_frame_sync = 10;
    p1_frame_sync_start = 0;
    p2_frame_sync = 10;
    p2_frame_sync_start = 0;
    p3_frame_sync = 10;
    p3_frame_sync_start = 0;
    p4_frame_sync = 10;
    p4_frame_sync_start = 0;
    heart_frame_sync = 50;
    mi_frame_sync = 50;
    score = 0;
    score1 = 0;

    removeCursor();
    textcolor(WHITE, GREEN2);
    draw_hline(0, 0, 119, ' ');
    textcolor(BLACK, WHITE);
    sprintf(cmd, "mode con cols=%d lines=%d", WIDTH, HEIGHT);
    system(cmd);
}

void show_time(int remain_time) {
    gotoxy(53, 0);
    textcolor(GREEN2, BLACK);
    printf("남은 시간: %02d", remain_time);
    textcolor(GREEN2, BLACK);
}

void move_heart() {
    int x, y, dx, dy, newx, newy;
    int newhearts[WIDTH][HEIGHT] = { 0 };
    static call_count = 0;

    if (hearts_count == 0)
        return;
    for (x = 0; x < WIDTH; x++) {
        for (y = 0; y < HEIGHT; y++) {
            if (hearts[x][y]) {
                dx = rand() % 3 - 1;
                dy = rand() % 3 - 1;
                newx = x + dx;
                newy = y + dy;

                if (newx == WIDTH) newx = WIDTH - 1;
                if (newx < 0) newx = 0;
                if (newy > HEIGHT - 1) newy = HEIGHT - 1;
                if (newy < 1) newy = 1;

                gotoxy(x, y);
                textcolor(WHITE, BLACK);
                printf(" ");
                textcolor(RED1, BLACK);
                gotoxy(newx, newy);
                printf(HEART);
                newhearts[newx][newy] = 1;
                textcolor(WHITE, BLACK);
            }
        }
    }
    memcpy(hearts, newhearts, sizeof(newhearts));
}

void move_mi() {
    int x, y, dx, dy, newx, newy;
    int newmis[WIDTH][HEIGHT] = { 0 };
    static call_count = 0;

    if (mi_count == 0)
        return;
    for (x = 0; x < WIDTH; x++) {
        for (y = 0; y < HEIGHT; y++) {
            if (xs[x][y]) {
                dx = rand() % 3 - 1;
                dy = rand() % 3 - 1;
                newx = x + dx;
                newy = y + dy;

                if (newx == WIDTH) newx = WIDTH - 1;
                if (newx < 0) newx = 0;
                if (newy > HEIGHT - 1) newy = HEIGHT - 1;
                if (newy < 1) newy = 1;

                gotoxy(x, y);
                textcolor(WHITE, BLACK);
                printf(" ");
                textcolor(MAGENTA1, BLACK);
                gotoxy(newx, newy);
                printf(mi);
                newmis[newx][newy] = 1;
                textcolor(WHITE, BLACK);
            }
        }
    }
    memcpy(xs, newmis, sizeof(newmis));
}

int showsnake(void) {
    int i;
    gotoxy2(snake[0]);
    if (frame_count % 2)
        textcolor(YELLOW1, BLACK);
    else
        textcolor(CYAN1, BLACK);
    printf("●");
    textcolor(YELLOW1, BLACK);
    for (i = 1; i < snake_length; i++) {
        gotoxy2(snake[i]);
        printf("□");
    }
    return 0;
}

int showsnake1(void) {
    int i;
    gotoxy2(snake1[0]);
    if (frame_count % 2)
        textcolor(CYAN1, BLACK);
    else
        textcolor(YELLOW1, BLACK);
    printf("●");
    textcolor(CYAN1, BLACK);
    for (i = 1; i < snake1_length; i++) {
        gotoxy2(snake1[i]);
        printf("□");
    }
    return 0;
}

void movesnake(int x, int y) {
    int i;
    gotoxy2(snake[snake_length - 1]);
    printf("  ");
    for (i = snake_length - 1; i >= 1; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0].X = x; snake[0].Y = y;
    showsnake();
}

void movesnake1(int x, int y) {
    int i;
    gotoxy2(snake1[snake1_length - 1]);
    printf("  ");
    for (i = snake1_length - 1; i >= 1; i--) {
        snake1[i] = snake1[i - 1];
    }
    snake1[0].X = x; snake1[0].Y = y;
    showsnake1();
}

void putstar(int x, int y, char ch) {
    gotoxy(x, y);
    putchar(ch);
}

int gameguide(void) {
    int i;
    for (i = 3; i <= 3; i++) {
        gotoxy(23, 8);
        printf("□□□□□□□□□□□□□□□□□□□□●");
        gotoxy(23, 9);
        printf("□");
        gotoxy(23, 10);
        printf("□   G       A       M       E       G       U       I       D       E");
        gotoxy(23, 11);
        printf("□");
        gotoxy(23, 12);
        printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□");
        gotoxy(23, 18);
        printf("▶ 2인 플레이 게임: AWSD, 방향키");
        gotoxy(23, 20);
        printf("▶ 60초 동안 점수를 더 많이 얻는 사람이 승리");
        gotoxy(23, 22);
        printf("▶ 하트를 먹으면 꼬리가 한개씩 길어지며, 별을 먹으면 꼬리가 한개씩 짧아짐");
        gotoxy(23, 24);
        printf("▷ ♥: +3점  ★: -1점");
        gotoxy(90, 32);
        printf("┌──────────────────┐");
        gotoxy(90, 33);
        printf("│   GAME START: G  │");
        gotoxy(90, 34);
        printf("└──────────────────┘");
        draw_box2(i * 2, i, 120 - i * 2, 38 - i, "■");
        _getch();
    }
    while (1) {
        if (_kbhit()) {
            key = _getch();
            if (key == ESC)
                exit(0);
            else if (key == 'G' || key == 'g') {
                cls(WHITE, BLACK);
                gametitle();
            }
        }
    }
}

int title(void) {
    int i;

    for (i = 3; i <= 3; i++) {
        gotoxy(35, 10);
        printf("□□□□□□□□□□□□□□●");
        gotoxy(35, 11);
        printf("□");
        gotoxy(35, 12);
        printf("□      S          N          A         K         E");
        gotoxy(35, 13);
        printf("□");
        gotoxy(35, 14);
        printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□");
        gotoxy(95, 33);
        printf("N반 2171339 양인서\n");
        gotoxy(44, 20);
        printf("START KEY: G");
        gotoxy(43, 22);
        printf("▶ PLAYER1 MOVE: ←, →, ↑, ↓");
        gotoxy(43, 24);
        printf("▶ PLAYER2 MOVE: A, D, W, S");
        gotoxy(43, 26);
        printf("▶ QUIT KEY: ESC");
        draw_box2(i * 2, i, 120 - i * 2, 38 - i, "■");
        _getch();
    }
    while (1) {
        if (_kbhit()) {
            key = _getch();
            if (key == ESC)
                exit(0);
            else if (key == 'G' || key == 'g') {
                cls(WHITE, BLACK);
                gameguide();
            }
        }
    }
    return 0;
}

int gametitle(void) {
    int i, count = 0;
    unsigned char last_ch, ch, last_ch1, ch1;
    int oldx, oldy, newx, newy;
    int oldx1, oldy1, newx1, newy1;
    int keep_moving = 0;
    int keep_moving1 = 0;
    int run_time, start_time, remain_time, last_remain_time, heart_time;
    int run_time1, start_time1, remain_time1, last_remain_time1, x_time;

START:
    init_game();
    heart_time = 0;
    start_time = time(NULL);
    last_remain_time = remain_time = time_out;

    x_time = 0;
    start_time1 = time(NULL);
    last_remain_time1 = remain_time1 = time_out1;

    textcolor(GREEN2, BLACK);
    for (i = 3; i <= 3; i++) {
        draw_box2(i * 2, i, 120 - i * 2, 38 - i, "■");
    }

    while (1) {
        if (called[0] == 0) {
            oldx = 40, oldy = 10, newx = 40, newy = 10;
            putstar(oldx, oldy, showsnake);
            called[0] = 1;
            last_ch = 0;
            ch = 0;
        }

        if (last_ch == ch && frame_count % p1_frame_sync != 0)
            return;
        if (keep_moving && ch == 0)
            ch = last_ch;
        last_ch = ch;

        newx = oldx = 40;
        newy = oldy = 10;

        for (i = 0; i < SNAKE_LENGTH; i++) {
            snake[i].X = oldx - i * 2;
            snake[i].Y = oldy;
        }
        removeCursor();
        showsnake();

        if (called[1] == 0) {
            oldx1 = 20, oldy1 = 10, newx1 = 20, newy1 = 10;
            putstar(oldx1, oldy1, showsnake1);
            called[1] = 1;
            last_ch1 = 0;
            ch1 = 0;
        }

        if (last_ch1 == ch1 && frame_count % p3_frame_sync != 0)
            return;
        if (keep_moving1 && ch1 == 0)
            ch1 = last_ch1;
        last_ch1 = ch1;

        newx1 = oldx1 = 20;
        newy1 = oldy1 = 10;

        for (i = 0; i < SNAKE_LENGTH; i++) {
            snake1[i].X = oldx1 - i * 2;
            snake1[i].Y = oldy1;
        }
        removeCursor();
        showsnake1();

        while (1) {
            if (_kbhit() == 1) {
                char c1;
                c1 = _getch();
                if (c1 == '\0')
                    continue;
                else
                    ch = c1;
                if (ch == ESC)
                    exit(0);
                if (ch == SPECIAL1 || ch == SPECIAL2) {
                    ch = _getch();
                    switch (ch) {
                    case UP:
                        last_ch = UP;
                        keep_moving = 1;
                        break;
                    case DOWN:
                        last_ch = DOWN;
                        keep_moving = 1;
                        break;
                    case LEFT:
                        last_ch = LEFT;
                        keep_moving = 1;
                        break;
                    case RIGHT:
                        last_ch = RIGHT;
                        keep_moving = 1;
                        break;
                    default:
                        keep_moving = 0;
                    }
                }
                else {
                    ch1 = ch;
                    switch (ch1) {
                    case UP2: last_ch1 = UP2;
                        keep_moving1 = 1;
                        break;
                    case DOWN2: last_ch1 = DOWN2;
                        keep_moving1 = 1;
                        break;
                    case RIGHT2: last_ch1 = RIGHT2;
                        keep_moving1 = 1;
                        break;
                    case LEFT2: last_ch1 = LEFT2;
                        keep_moving1 = 1;
                        break;
                    default:
                        keep_moving1 = 0;
                        break;
                    }
                }
            }
            if (keep_moving) {
                switch (last_ch) {
                case UP:
                    if (oldy > 4)
                        newy = oldy - 1;
                    else {
                        last_ch = DOWN; newy = 4;
                    }
                    break;
                case DOWN:
                    if (oldy < HEIGHT - 6)
                        newy = oldy + 1;
                    else {
                        last_ch = UP; newy = HEIGHT - 6;
                    }
                    break;
                case LEFT:
                    if (oldx > 8)
                        newx = oldx - 2;
                    else {
                        last_ch = RIGHT; newx = oldx;
                    }
                    break;
                case RIGHT:
                    if (oldx < WIDTH - 8)
                        newx = oldx + 2;
                    else {
                        last_ch = LEFT; newx = oldx;
                    }
                    break;
                }
                if (keep_moving) {
                    erasestar(oldx, oldy);
                    putstar(newx, newy, showsnake);
                    oldx = newx;
                    oldy = newy;
                    if (hearts[newx][newy]) {
                        score += 3;
                        if (snake_length < SNAKE_LENGTH_MAX) {
                            snake_length++;
                        }
                        hearts[newx][newy] = 0;
                        hearts_count--;
                        showscore(1);

                        if (p2_frame_sync > 5)
                            p2_frame_sync--;
                    }
                    if (xs[newx][newy]) {
                        if (score) score--;
                        if (snake_length > 5) {
                            for (i = 1; i < snake_length; i++) {
                                gotoxy2(snake[i]);
                                printf("   ");
                            }
                            snake_length--;
                        }
                        xs[newx][newy] = 0;
                        mi_count--;

                        if (p4_frame_sync > 5)
                            p4_frame_sync--;
                    }
                }
                movesnake(newx, newy);
                oldx = newx;
                oldy = newy;
                keep_moving = 1;
            }
            if (keep_moving1) {
                switch (last_ch1) {
                case UP2:
                    if (oldy1 > 4)
                        newy1 = oldy1 - 1;
                    else {
                        last_ch1 = DOWN2; newy1 = 4;
                    }
                    break;
                case DOWN2:
                    if (oldy1 < HEIGHT - 6)
                        newy1 = oldy1 + 1;
                    else {
                        last_ch1 = UP2; newy1 = HEIGHT - 6;
                    }
                    break;
                case LEFT2:
                    if (oldx1 > 8)
                        newx1 = oldx1 - 2;
                    else {
                        last_ch1 = RIGHT2; newx1 = oldx1;
                    }
                    break;
                case RIGHT2:
                    if (oldx1 < WIDTH - 8)
                        newx1 = oldx1 + 2;
                    else {
                        last_ch1 = LEFT2; newx1 = oldx1;
                    }
                    break;
                }
                if (keep_moving1) {
                    erasestar(oldx1, oldy1);
                    putstar(newx1, newy1, showsnake1);
                    oldx1 = newx1;
                    oldy1 = newy1;
                    keep_moving1 = 1;
                    if (hearts[newx1][newy1]) {
                        score1 += 3;
                        if (snake1_length < SNAKE_LENGTH_MAX) {
                            snake1_length++;
                        }
                        hearts[newx1][newy1] = 0;
                        hearts_count--;
                        if (p2_frame_sync > 5)
                            p2_frame_sync--;
                    }
                    if (xs[newx1][newy1]) {
                        if (score1) {
                            score1--;
                            if (snake1_length > 5) {
                                for (i = 1; i < snake1_length; i++) {
                                    gotoxy2(snake1[i]);
                                    printf("   ");
                                }
                                snake1_length--;
                            }
                            xs[newx1][newy1] = 0;
                            mi_count--;

                            if (p4_frame_sync > 5)
                                p4_frame_sync--;
                        }
                    }
                }
                movesnake1(newx1, newy1);
                oldx1 = newx1;
                oldy1 = newy1;
                keep_moving1 = 1;
            }
            if (count % (1000 / Delay) == 0)
                remain_time--;
            if (count % (1000 / Delay * 5) == 0)
                show_heart();
            if (count % (1000 / Delay * 7) == 0)
                show_x();
            showscore(0);
            showscore(1);
            count++;
            show_time(remain_time);
            if (remain_time == 0) break;
            if (frame_count % heart_frame_sync == 0)
                move_heart();
            if (frame_count % mi_frame_sync == 0)
                move_mi();
            Sleep(Delay);
            frame_count++;
        }
        if (remain_time == 0) break;
    }

    cls(WHITE, BLACK);
    textcolor(WHITE, BLACK);

    gotoxy(35, 10);
    printf("□□□□□□□□□□□□□□●");
    gotoxy(35, 11);
    printf("□");
    gotoxy(35, 12);
    printf("□   G      A      M      E      O      V      E     R");
    gotoxy(35, 13);
    printf("□");
    gotoxy(35, 14);
    printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□");

    gotoxy(43, 18);
    printf("PLAYER1 SCORE: %02d  PLAYER2 SCORE: %02d", score, score1);

    gotoxy(52, 20);
    if (score > score1)
        printf(" WINNER IS PLAYER1 ");
    else if (score < score1)
        printf(" WINNER IS PLAYER2 ");
    else
        printf("       DRAW        ");

    textcolor(WHITE, BLACK);
    while (1) {
        int c1, c2;
        do {
            c1 = rand() % 16;
            c2 = rand() % 16;
        } while (c1 == c2);
        textcolor(c1, c2);

        for (i = 3; i <= 3; i++) {
            draw_box2(i * 2, i, 120 - i * 2, 38 - i, "■");
        }

        gotoxy(48, 25);
        printf("┌────────────┐");
        gotoxy(48, 26);
        printf("│ ▶ Hit (R) to Restart  │");
        gotoxy(48, 27);
        printf("└────────────┘");
        textcolor(WHITE, BLACK);
        Sleep(300);
        if (_kbhit()) {
            ch = _getch();
            if (ch == 'r' || ch == 'R')
                break;
        }
    }
    if (ch == 'r' || ch == 'R') {
        cls(WHITE, BLACK);
        goto START;
    }
    gotoxy(0, HEIGHT - 1);
}

int main(void) {
    title();
}