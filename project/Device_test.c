#include <math.h>
#include <stdlib.h> // for exit
#include <pthread.h>
#include "./lib/accelMagGyro.h"
#include "./lib/bitmap.h"
#include "./lib/button.h"
#include "./lib/buzzer.h"
#include "./lib/colorLED.h"
#include "./lib/fnd.h"
#include "./lib/led.h"
#include "./lib/temp.h"
#include "./lib/textLCD.h"
#include "./lib/display.h"

#define MENU 7
#define WIN 8
#define PWROFF 9
#define FAIL 10
#define EXIT 11
#define LEVEL1 12
#define LEVEL2 13
#define LEVEL3 14

static int msgID = 0;
int status = MENU;
int score;
static int obstacleX=700;
static int obstacleY=300;
static int obstacleX2_1=750;
static int obstacleY2_1=100;
static int obstacleX2_2=250;
static int obstacleY2_2=300;
void *score_timer(void *data)
{
    while (1)
    {
        if (score < 0)
            score = 0;
        fndDisp(score, 0);
        usleep(50);
        score--;
    }
}

void HW_init()
{
    //init_accel();
    //init_gyro();
    textLCD_Init();
    ledInit();
    printf("HW INIT\n");
}

void HW_close()
{
    close_accel();
    textLCD_off();
    ledExit();
}

int set_Mobility()
{
    int temp = readTEMP() / 5;
    if (temp > 12)
        temp = 12;
    return temp;
}

void level_1_obstacle(int OnOff)
{

    static int obstacleDirection = 1; // 1: 오른쪽, -1: 왼쪽

    if (OnOff == 1)
        draw_bmp_custom("joat.bmp", obstacleX, obstacleY, 80, 80, 0);

    else if (OnOff == 0)
        {
        draw_square(obstacleX-10, obstacleY, 80, 80, 0xFFFFFF, 0);
        draw_square(obstacleX+10, obstacleY, 80, 80, 0xFFFFFF, 0);
        }
    else
        ;

    // 장애물 이동 로직
    obstacleX += obstacleDirection * 10;

    // 화면을 벗어나면 방향 전환
    if (obstacleX <= 70 || obstacleX >= 1024 - 150)
        obstacleDirection *= -1;
}
void level_2_obstacle(int OnOff)
{

    static int obstacleDirection = 1; // 1: 아래쪽, -1: 위쪽

    if (OnOff == 1)
{        
        draw_bmp_custom("joat.bmp", obstacleX2_1, obstacleY2_1, 80, 80, 0);
        draw_bmp_custom("joat.bmp", obstacleX2_2, obstacleY2_2, 80, 80, 0);
}
    else if (OnOff == 0)
        {
        draw_square(obstacleX2_1, obstacleY2_1-10, 80, 80, 0xFFFFFF, 0);
        draw_square(obstacleX2_1, obstacleY2_1+10, 80, 80, 0xFFFFFF, 0);
        draw_square(obstacleX2_2, obstacleY2_2-10, 80, 80, 0xFFFFFF, 0);
        draw_square(obstacleX2_2, obstacleY2_2+10, 80, 80, 0xFFFFFF, 0);
        }
    else
        ;

    // 장애물 이동 로직
    obstacleY2_1 += obstacleDirection * 10;
    obstacleY2_2 += obstacleDirection * 10;

    // 화면을 벗어나면 방향 전환
    if (obstacleY2_1 <= 100 || obstacleY2_1 >= 600 - 100 ||obstacleY2_2 <= 100 || obstacleY2_2 >= 600 - 100)
        obstacleDirection *= -1;
}

void level_1_background(void)
{
    
    draw_background(0x8B4513, 0);
    draw_square(50, 50, 1024 - 100, 600 - 100, 0xFFFFFF, 0);
    
    level_1_obstacle(1);
   
}
void level_1_update(void)
{
    level_1_obstacle(0); // 장애물 지우기
    usleep(5000);    
    level_1_obstacle(1); // 장애물 그리기
          
}
void level_2_background(void)
{
    draw_background(0x8B4513, 0);
    draw_square(50, 50, 1024 - 100, 600 - 100, 0xFFFFFF, 0);
    draw_square(358, 250, 308, 100, 0x8B4513, 0);
    level_2_obstacle(1);
}
void level_2_update(void)
{
    level_2_obstacle(0); // 장애물 지우기
    usleep(5000);    
    level_2_obstacle(1); // 장애물 그리기
          
}
void level_3_background(void)
{
    draw_background(0x8B4513, 0);
    draw_square(50, 50, 1024 - 100, 600 - 100, 0xFFFFFF, 0);
    draw_square(258, 200, 150, 200, 0x8B4513, 0);
    draw_square(566, 200, 150, 200, 0x8B4513, 0);
}

void level_1_obj_1(int OnOff)
{
    if (OnOff == 1)
      draw_bmp_custom("trophy.bmp", 340, 220, 50, 50, 0);
    //draw_square(340, 220, 50, 50, 0xFF0000, 0);
       
    else if (OnOff == 0)
{
        draw_square(340, 220, 50, 50, 0xFFFFFF, 0);
} 
   else
        ;
}

void level_1_obj_2(int OnOff)
{
    if (OnOff == 1)      
        draw_bmp_custom("hou.bmp", 730, 400, 50, 50, 0);        
        //draw_square(730, 400, 50, 50, 0x0000FF, 0);
    else if (OnOff == 0)
        draw_square(730, 400, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_2_obj_1(int OnOff)
{
    if (OnOff == 1)
        draw_square(850, 400, 50, 50, 0xFF0000, 0);
    else if (OnOff == 0)
        draw_square(850, 400, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_2_obj_2(int OnOff)
{
    if (OnOff == 1)
        draw_square(900, 100, 50, 50, 0x00FF00, 0);
    else if (OnOff == 0)
        draw_square(900, 100, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_2_obj_3(int OnOff)
{
    if (OnOff == 1)
        draw_square(440, 450, 50, 50, 0x0000FF, 0);
    else if (OnOff == 0)
        draw_square(440, 450, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_3_obj_1(int OnOff)
{
    if (OnOff == 1)
        draw_square(870, 500, 50, 50, 0xFF0000, 0);
    else if (OnOff == 0)
        draw_square(870, 500, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_3_obj_2(int OnOff)
{
    if (OnOff == 1)
        draw_square(120, 120, 50, 50, 0x00FF00, 0);
    else if (OnOff == 0)
        draw_square(120, 120, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

void level_3_obj_3(int OnOff)
{
    if (OnOff == 1)
        draw_square(512, 300, 50, 50, 0x0000FF, 0);
    else if (OnOff == 0)
        draw_square(512, 300, 50, 50, 0xFFFFFF, 0);
    else
        ;
}

int main(void)
{
    HW_init();
    display_init();
    int mobility = set_Mobility();
    msgID = buttonInit();
    int returnValue = 0;
    BUTTON_MSG_T messageRxData;
    int flag_safe = 1;
    int flag_display_score = 0;
    int orb_count = 0;
    int orb_case = 0;
    int Ax;
    int Ay;
    char buffer[17];
    sprintf(buffer, "Difficulty: %d", mobility);
    pthread_t thread;
    int thread_id;

    while (1)
    {
        switch (status)
        {
        case MENU: // 메인 메뉴
            // 프레임버퍼에 메뉴 사진 밀어넣고 띄우기
            draw_bmp_menu();
            printf("Press Key\r\n");
            writeLCD(1, "Game Menu       ");
            writeLCD(2, "                ");
            for (int i = 0; i < 8; i++)
            {
                ledOnOff(i, OFF);
            }
            returnValue = msgrcv(msgID, &messageRxData, sizeof(int), 0, 0); // 버튼 입력 받기
            if (returnValue < 0)
            {
                printf("Receive Key Failed\r\n");
                status = MENU;
                break;
            }
            switch (messageRxData.keyInput)
            {
            case KEY_HOME: // 정상 게임 모드
                printf("Home key\r\n");
                printf("PLAY key\r\n");
                status = LEVEL1;
                break;
            case KEY_BACK: // 디버그 게임 모드
                printf("Back key\r\n");
                printf("PLAY key\r\n");
                mobility = 1;
                status = LEVEL1;
                break;
            case KEY_VOLUMEDOWN: // 게임 종료
                printf("Volume down key\r\n");
                printf("PWR OFF\r\n");
                status = EXIT;
                break;
            default: // 잘못된 키 입력
                printf("KEY INPUT ERROR \r\n");
                break;
            }
            break;

        case WIN: // 게임 승리
            // 프레임버퍼에 승리 사진 밀어넣고 띄우기
            writeLCD(1, "You Won!        ");
            writeLCD(2, "You Are Winner! ");
            draw_bmp_win();
            pthread_cancel(thread);
            returnValue = msgrcv(msgID, &messageRxData, sizeof(int), 0, 0); // 버튼 입력 받기
            if (returnValue < 0)
            {
                printf("Receive Key Failed\r\n");
                status = WIN;
                break;
            }
            if (messageRxData.keyInput == KEY_HOME)
            {
                status = MENU;
            }
            else if (messageRxData.keyInput == KEY_VOLUMEDOWN)
            {
                status = EXIT;
            }
            break;

        case EXIT: // 게임 종료
            writeLCD(1, "Good Bye        ");
            writeLCD(2, "                ");
            draw_background(0x000000, 0);
            buttonExit();
            return 0;

        case LEVEL1: // 레벨 1 : 오브 2개, R>B 순서로 터치

            score = 999999;
            thread_id = pthread_create(&thread, NULL, score_timer, NULL); // 점수 계산 타이머 시작

            writeLCD(1, "LEVEL1          ");
            writeLCD(2, buffer);
            printf("LEVEL1\r\n");
            Ax = 200;
            Ay = 200;
            flag_safe = 1;
            orb_count = 0;
            orb_case = 0;

            level_1_background();
            while (flag_safe)
            {
                init_accel();
                double ax = read_accel(Y) / 163;
                double ay = read_accel(X) / 163;
                Ax = Ax + (ax * mobility);
                Ay = Ay + (ay * mobility);

                int Cx = (Ax + 20);
                int Cy = (Ay + 20);
                int obstacleLeft = obstacleX - 20;
                int obstacleRight = obstacleX + 70;  // Assuming obstacle width is 50 and adding 10 for some margin
                int obstacleTop = obstacleY-20;
                int obstacleBottom = obstacleY + 70;  // Assuming obstacle height is 50
                if (Cx > obstacleLeft && Cx < obstacleRight && Cy > obstacleTop && Cy < obstacleBottom)
                {
                    status = FAIL;
                    flag_safe = 0;
                }
                if (orb_count == 2)
                {
                    level_1_obj_1(OFF);
                    level_1_obj_2(OFF);
                    status = LEVEL2;
                    flag_safe = 0;
                }
                else if (orb_count == 1 && orb_case == 1)
                {
                    level_1_obj_1(OFF);
                    level_1_obj_2(ON);
                }
                else if (orb_count == 1 && orb_case == 2)
                {
                    level_1_obj_1(ON);
                    level_1_obj_2(OFF);
                }
                else if (orb_count ==0)
                {
                    level_1_obj_1(ON);
                    level_1_obj_2(ON);
                }

                if (Cx > 50 && Cx < 1024 - 50 && Cy > 50 && Cy < 600 - 50)
                {
                }
                else
                {
                    status = FAIL;
                    flag_safe = 0;
                }

                if (orb_count == 0 && Cx > 340 && Cx < 340 + 50 && Cy > 220 && Cy < 220 + 50)
                {
                    orb_count = 1;
                    orb_case = 1;
                    ledOnOff(0, ON);
                }
                if (orb_count == 0 && Cx > 730 && Cx < 730 + 50 && Cy > 400 && Cy < 400 + 50)
                {
                    orb_count = 1;
                    orb_case = 2;
                    ledOnOff(0, ON);
                }
                if (orb_count == 1 && Cx > 340 && Cx < 340 + 50 && Cy > 220 && Cy < 220 + 50 && orb_case ==2)
                {
                    orb_count = 2;
                    ledOnOff(1, ON);
                }
                if (orb_count == 1 && Cx > 730 && Cx < 730 + 50 && Cy > 400 && Cy < 400 + 50 && orb_case ==1)
                {
                    orb_count = 2;
                    ledOnOff(1, ON);
                }

                draw_bmp_custom("goat.bmp", Ax, Ay, 80 , 80, 0);
                //draw_square(Ax, Ay, 40, 40, 0x000000, 0);
                level_1_update(); // 장애물 이동 및 그리기
                usleep(100000);
                draw_square(Ax, Ay, 80, 80, 0xFFFFFF, 0);

                printf("Coordinate: %d, %d\r\n", Cx, Cy);
            }
            break;

case LEVEL2: // 레벨 2 : R > G > B 순서로 터치
    writeLCD(1, "LEVEL2          ");
    writeLCD(2, buffer);
    printf("LEVEL2\r\n");
    Ax = 100;
    Ay = 100;
    flag_safe = 1;
    int level2_orb1_trigger = 0, level2_orb2_trigger = 0, level2_orb3_trigger = 0;
    int orb_count = 0;
    level_2_background();

    while (flag_safe)
    {
        init_accel();
        double ax = read_accel(Y) / 163;
        double ay = read_accel(X) / 163;
        Ax = Ax + (ax * mobility);
        Ay = Ay + (ay * mobility);

        int Cx = (Ax + 20);
        int Cy = (Ay + 20);

                int obstacleLeft2_1 = obstacleX2_1 - 10;
                int obstacleRight2_1 = obstacleX2_2 + 60;  
                int obstacleTop2_1 = obstacleY2_1-10;
                int obstacleBottom2_1 = obstacleY2_1 + 60;  // Assuming obstacle height is 50
                
                int obstacleLeft2_2 = obstacleX2_2 - 10;
                int obstacleRight2_2 = obstacleX2_2 + 60;  
                int obstacleTop2_2 = obstacleY2_2-10;
                int obstacleBottom2_2 = obstacleY2_2 + 60;  // Assuming obstacle height is 50
                if (Cx > obstacleLeft2_1 && Cx < obstacleRight2_1 && Cy > obstacleTop2_1 && Cy < obstacleBottom2_1)
                {
                    status = FAIL;
                    flag_safe = 0;
                }
                if (Cx > obstacleLeft2_2 && Cx < obstacleRight2_2 && Cy > obstacleTop2_2 && Cy < obstacleBottom2_2)
                {
                    status = FAIL;
                    flag_safe = 0;
                }
        if (orb_count == 3)
        {
            level_2_obj_1(OFF);
            level_2_obj_2(OFF);
            level_2_obj_3(OFF);
            status = LEVEL3;
            flag_safe = 0;
            ledOnOff(4, ON);
        }
        else if (orb_count == 2)
        {
            if (level2_orb1_trigger == 1)
                level_2_obj_1(OFF);
            else if (level2_orb2_trigger == 1)
                level_2_obj_2(OFF);
            else if (level2_orb3_trigger == 1)
                level_2_obj_3(OFF);
            ledOnOff(3, ON);        
}
        else if (orb_count == 1)
        {
            if (level2_orb1_trigger == 1)
                level_2_obj_1(OFF);
            else if (level2_orb2_trigger == 1)
                level_2_obj_2(OFF);
            else if (level2_orb3_trigger == 1)
                level_2_obj_3(OFF);
            ledOnOff(2, ON);
        }
        else if (orb_count == 0)
        {
            level_2_obj_1(ON);
            level_2_obj_2(ON);
            level_2_obj_3(ON);
        }

        if (Cx > 50 && Cx < 1024 - 50 && Cy > 50 && Cy < 600 - 50)
        {
            if (Cx > 358 && Cx < 358 + 308 && Cy > 250 && Cy < 250 + 100)
            {
                status = FAIL;
                flag_safe = 0;
            }
        }
        else
        {
            status = FAIL;
            flag_safe = 0;
        }

        if (level2_orb1_trigger == 0 && Cx > 850 && Cx < 850 + 50 && Cy > 400 && Cy < 400 + 50)
        {
            level2_orb1_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
        }
        if (level2_orb2_trigger == 0 && Cx > 900 && Cx < 900 + 50 && Cy > 100 && Cy < 100 + 50)
        {
            level2_orb2_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
        }
        if (level2_orb3_trigger == 0 && Cx > 440 && Cx < 440 + 50 && Cy > 450 && Cy < 450 + 50)
        {
            level2_orb3_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
        }

               draw_bmp_custom("goat.bmp", Ax, Ay, 80 , 80, 0);
                //draw_square(Ax, Ay, 40, 40, 0x000000, 0);
                level_2_update(); // 장애물 이동 및 그리기
                usleep(100000);
                draw_square(Ax, Ay, 80, 80, 0xFFFFFF, 0);

                printf("Coordinate: %d, %d\r\n", Cx, Cy);
            }
    break;

       case LEVEL3: // 레벨 3 : R > G > B 순서로 터치
            writeLCD(1, "LEVEL3          ");
            writeLCD(2, buffer);
            printf("LEVEL3\r\n");
            Ax = 100;
            Ay = 100;
            flag_safe = 1;
    	     int level3_orb1_trigger = 0, level3_orb2_trigger = 0, level3_orb3_trigger = 0;
   	        orb_count = 0;
            level_3_background();
            while (flag_safe)
            {
                init_accel();
                double ax = read_accel(Y) / 163;
                double ay = read_accel(X) / 163;
                Ax = Ax + (ax * mobility);
                Ay = Ay + (ay * mobility);

                int Cx = (Ax + 20);
                int Cy = (Ay + 20);

                if (orb_count == 3)
                {
                    level_3_obj_1(OFF);
                    level_3_obj_2(OFF);
                    level_3_obj_3(OFF);
                    status = WIN;
                    flag_safe = 0;
           ledOnOff(7, ON);
                }
                else if (orb_count == 2)
                {
            if (level3_orb1_trigger == 1)
                level_2_obj_1(OFF);
            else if (level3_orb2_trigger == 1)
                level_2_obj_2(OFF);
            else if (level3_orb3_trigger == 1)
                level_2_obj_3(OFF);
           ledOnOff(6, ON);
                }
                else if (orb_count == 1)
                {
            if (level3_orb1_trigger == 1)
                level_2_obj_1(OFF);
            else if (level3_orb2_trigger == 1)
                level_2_obj_2(OFF);
            else if (level3_orb3_trigger == 1)
                level_2_obj_3(OFF);
           ledOnOff(5, ON);
                }
                else if (orb_count == 0)
                {
                    level_3_obj_1(ON);
                    level_3_obj_2(ON);
                    level_3_obj_3(ON);
                }

                if (Cx > 50 && Cx < 1024 - 50 && Cy > 50 && Cy < 600 - 50)
                {
                    if (Cx > 258 && Cx < 258 + 150 && Cy > 200 && Cy < 200 + 200)
                    {
                        status = FAIL;
                        flag_safe = 0;
                    }
                    else if (Cx > 566 && Cx < 566 + 150 && Cy > 200 && Cy < 200 + 200)
                    {
                        status = FAIL;
                        flag_safe = 0;
                    }
                }
                else
                {
                    status = FAIL;
                    flag_safe = 0;
                }

                if (level3_orb1_trigger == 0 && Cx > 870 && Cx < 870 + 50 && Cy > 500 && Cy < 500 + 50)
                {
            level3_orb1_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
                }
                if (level3_orb2_trigger == 0 && Cx > 120 && Cx < 120 + 50 && Cy > 120 && Cy < 120 + 50)
                {
            level3_orb2_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
                }
                if (level3_orb3_trigger == 0 && Cx > 512 && Cx < 512 + 50 && Cy > 300 && Cy < 300 + 50)
                {
            level3_orb3_trigger = 1;
            if (orb_count == 0)
                orb_count = 1;
            else if (orb_count == 1)
                orb_count = 2;
            else if (orb_count == 2)
                orb_count = 3;
                }

                draw_square(Ax, Ay, 40, 40, 0x000000, 0);
                usleep(100000);
                draw_square(Ax, Ay, 40, 40, 0xFFFFFF, 0);

                printf("Coordinate: %d, %d\r\n", Cx, Cy);
            }
            break;


        case FAIL: // 실패할 경우 처음부터 시작하거나 게임 종료
            writeLCD(2, "Failed          ");
            draw_bmp_fail();
            pthread_cancel(thread);
            returnValue = msgrcv(msgID, &messageRxData, sizeof(int), 0, 0); // 버튼 입력 받기
            if (returnValue < 0)
            {
                printf("Receive Key Failed\r\n");
                status = WIN;
                break;
            }
            if (messageRxData.keyInput == KEY_HOME)
            {
                status = MENU;
            }
            else if (messageRxData.keyInput == KEY_VOLUMEDOWN)
            {
                status = EXIT;
            }
            break;
        }
    }
}
