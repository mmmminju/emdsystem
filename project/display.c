#include "display.h"

int fb_fd0;
int fb_fd1;
struct fb_var_screeninfo fbvar;
struct fb_fix_screeninfo fbfix;
unsigned char *fb_mapped0;
unsigned char *fb_mapped1;
unsigned long *ptr0;
unsigned long *ptr1;

int screen_width;
int screen_height;
int bits_per_pixel;
int line_length;

int mem_size;

    char *pData, *data;
    char r, g, b;
    unsigned long pixel;
    int cols = 0, rows = 0;
    unsigned long bmpdata[1280 * 800];
    int k, t;
    unsigned char *pfbmap;

int display_init(void)
{
    // 커서 제거
    int conFD = open("/dev/tty0", O_RDWR);
    ioctl(conFD, KDSETMODE, KD_GRAPHICS);
    close(conFD);

    if (access(FBDEV_FILE0, F_OK))
    {
        printf("%s: access error\n", FBDEV_FILE0);
        exit(1);
    }

    if ((fb_fd0 = open(FBDEV_FILE0, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE0);
        exit(1);
    }

    if (ioctl(fb_fd0, FBIOGET_VSCREENINFO, &fbvar))
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE0);
        exit(1);
    }

    if (ioctl(fb_fd0, FBIOGET_FSCREENINFO, &fbfix))
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE0);
        exit(1);
    }

    if (access(FBDEV_FILE1, F_OK))
    {
        printf("%s: access error\n", FBDEV_FILE1);
        exit(1);
    }

    if ((fb_fd1 = open(FBDEV_FILE1, O_RDWR)) < 0)
    {
        printf("%s: open error\n", FBDEV_FILE1);
        exit(1);
    }

    if (ioctl(fb_fd1, FBIOGET_VSCREENINFO, &fbvar))
    {
        printf("%s: ioctl error - FBIOGET_VSCREENINFO \n", FBDEV_FILE1);
        exit(1);
    }

    if (ioctl(fb_fd1, FBIOGET_FSCREENINFO, &fbfix))
    {
        printf("%s: ioctl error - FBIOGET_FSCREENINFO \n", FBDEV_FILE1);
        exit(1);
    }

    screen_width = fbvar.xres;
    screen_height = fbvar.yres;
    bits_per_pixel = fbvar.bits_per_pixel;
    line_length = fbfix.line_length;

    printf("screen_width : %d\n", screen_width);
    printf("screen_height : %d\n", screen_height);
    printf("bits_per_pixel : %d\n", bits_per_pixel);
    printf("line_length : %d\n", line_length);

    mem_size = screen_width * screen_height * 4;
    fb_mapped0 = (unsigned char *)mmap(0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd0, 0);
    fb_mapped1 = (unsigned char *)mmap(0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd1, 0);

    if (fb_mapped0 < 0 || fb_mapped1 < 0)
    {
        printf("mmap error!\n");
        exit(1);
    }

    return 0;
}

int draw_background(int color, int idx_buffer)
{
    if (idx_buffer == 0)
    {
        for (int i = 0; i < screen_height; i++)
        {
            for (int j = 0; j < screen_width; j++)
            {
                ptr0 = (unsigned long *)fb_mapped0 + screen_width * i + j;
                *ptr0 = color;
            }
        }
        return 0;
    }
    else if (idx_buffer == 1)
    {
        for (int i = 0; i < screen_height; i++)
        {
            for (int j = 0; j < screen_width; j++)
            {
                ptr1 = (unsigned long *)fb_mapped1 + screen_width * i + j;
                *ptr1 = color;
            }
        }
        return 0;
    }
    else
        return -1;
}


int draw_square(int der_x, int der_y, int dX, int dY, int color, int idx_buffer)
{
    if (idx_buffer == 0)
    {
        for (int x = der_x; x < der_x + dX; x++)
        {
            for (int y = der_y; y < der_y + dY; y++)
            {
                ptr0 = (unsigned long *)fb_mapped0 + screen_width * y + x;
                *ptr0 = color;
            }
        }
        return 0;
    }
    else if (idx_buffer == 1)
    {
        for (int x = der_x; x < der_x + dX; x++)
        {
            for (int y = der_y; y < der_y + dY; y++)
            {
                ptr1 = (unsigned long *)fb_mapped1 + screen_width * y + x;
                *ptr1 = color;
            }
        }
        return 0;
    }
    else
        return -1;
}

int draw_moving_square(int x, int y, int width, int height, int color, int idx_buffer) {
    int delta = 5;  // 각 반복에서의 위치 변경
    int steps = 100; // 애니메이션의 단계 수
    int i;

    for (i = 0; i < steps; i++) {
        // 이전 위치의 네모를 지우기
        draw_square(x, y, width, height, 0x000000, idx_buffer);

        // 네모의 위치 업데이트
        x += delta;
        y += delta;

        // 새 위치에 네모 그리기
        draw_square(x, y, width, height, color, idx_buffer);

        // 움직임을 보기 위해 짧은 시간 동안 일시 중지
        usleep(50000); // 50 밀리초
    }

    // 네모의 최종 위치를 지우기
    draw_square(x, y, width, height, 0xffffff, idx_buffer);

    return 0;
}
int draw_bmp_menu(void)
{
    read_bmp("MENU.bmp", &pData, &data, &cols, &rows);
    for (int j = 0; j < rows; j++)
    {
        k = j * cols * 3;
        t = (rows - 1 - j) * cols; // 가로 size가 작을 수도 있다.

        for (int i = 0; i < cols; i++)
        {
            b = *(data + (k + i * 3));
            g = *(data + (k + i * 3 + 1));
            r = *(data + (k + i * 3 + 2));

            pixel = ((r << 16) | (g << 8) | b);
            bmpdata[t + i] = pixel; // save bitmap data bottom up
        }
    }
    printf("1");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            ptr0 = (unsigned long *)fb_mapped0 + cols * i + j;
            *ptr0 = bmpdata[j + i * cols];
        }
    }
    close_bmp(&pData);
}

int draw_bmp_win(void)
{
    read_bmp("WIN.bmp", &pData, &data, &cols, &rows);
    for (int j = 0; j < rows; j++)
    {
        k = j * cols * 3;
        t = (rows - 1 - j) * cols; // 가로 size가 작을 수도 있다.

        for (int i = 0; i < cols; i++)
        {
            b = *(data + (k + i * 3));
            g = *(data + (k + i * 3 + 1));
            r = *(data + (k + i * 3 + 2));

            pixel = ((r << 16) | (g << 8) | b);
            bmpdata[t + i] = pixel; // save bitmap data bottom up
        }
    }
    printf("1");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            ptr0 = (unsigned long *)fb_mapped0 + cols * i + j;
            *ptr0 = bmpdata[j + i * cols];
        }
    }
    close_bmp(&pData);
}

int draw_bmp_fail(void)
{
    read_bmp("FAIL.bmp", &pData, &data, &cols, &rows);
    for (int j = 0; j < rows; j++)
    {
        k = j * cols * 3;
        t = (rows - 1 - j) * cols; // 가로 size가 작을 수도 있다.

        for (int i = 0; i < cols; i++)
        {
            b = *(data + (k + i * 3));
            g = *(data + (k + i * 3 + 1));
            r = *(data + (k + i * 3 + 2));

            pixel = ((r << 16) | (g << 8) | b);
            bmpdata[t + i] = pixel; // save bitmap data bottom up
        }
    }
    printf("1");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            ptr0 = (unsigned long *)fb_mapped0 + cols * i + j;
            *ptr0 = bmpdata[j + i * cols];
        }
    }
    close_bmp(&pData);
}
int draw_bmp_custom(char *filename, int x, int y, int width, int height, int idx_buffer) {
    char *pData, *data;
    int cols, rows;
    unsigned char r, g, b;
    unsigned long pixel;
    unsigned long bmpdata[1280 * 800];
    int k, t;
    unsigned long *ptr;

    // Read BMP file
    if (read_bmp(filename, &pData, &data, &cols, &rows) < 0) {
        printf("File open failed\r\n");
        return -1;
    }

    // Draw BMP at the specified position and size
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Calculate source pixel position
            k = (i * rows / height) * cols * 3;
            t = (rows - 1 - (i * rows / height)) * cols;

            // Read RGB values
            b = *(data + (k + (j * cols / width) * 3));
            g = *(data + (k + (j * cols / width) * 3 + 1));
            r = *(data + (k + (j * cols / width) * 3 + 2));

            // Create pixel value
            pixel = ((r << 16) | (g << 8) | b);
            
            // Determine framebuffer pointer based on the specified buffer index
            if (idx_buffer == 0) {
                ptr = (unsigned long *)fb_mapped0 + (y + i) * screen_width + (x + j);
            } else if (idx_buffer == 1) {
                ptr = (unsigned long *)fb_mapped1 + (y + i) * screen_width + (x + j);
            } else {
                printf("Invalid buffer index\r\n");
                return -1;
            }

            // Write pixel to the framebuffer
            *ptr = pixel;
        }
    }

    // Close BMP file
    close_bmp(&pData);

    return 0;
}

struct Obstacle {
    int x;        // 현재 x 좌표
    int y;        // 현재 y 좌표
    int width;    // 장애물 너비
    int height;   // 장애물 높이
    int motion_range;  // y-좌표의 움직임 범위
    int steps;    // 움직임의 단계 수
    char *filename;  // 장애물 BMP 파일 이름
};

// 장애물 초기화 함수
void init_obstacle(struct Obstacle *obstacle, int x, int y, int width, int height, int motion_range, int steps, char *filename) {
    obstacle->x = x;
    obstacle->y = y;
    obstacle->width = width;
    obstacle->height = height;
    obstacle->motion_range = motion_range;
    obstacle->steps = steps;
    obstacle->filename = filename;
}

// 장애물 그리기 함수
void draw_obstacle(struct Obstacle *obstacle, int idx_buffer) {
    int i, j;
    unsigned char r, g, b;
    unsigned long pixel;
    unsigned long bmpdata[1024 * 600];
    int k, t;
    unsigned long *ptr;

    // BMP 파일 읽기
    char *pData, *data;
    int cols, rows;

    if (read_bmp(obstacle->filename, &pData, &data, &cols, &rows) < 0) {
        printf("파일 오픈 실패\r\n");
        return;
    }

    // 장애물 이동 및 그리기
    for (i = 0; i < obstacle->steps; i++) {
        int y = obstacle->y + i * obstacle->motion_range / obstacle->steps;

        // 지정된 위치와 크기에서 BMP 그리기
        for (j = 0; j < obstacle->height; j++) {
            for (k = 0; k < obstacle->width; k++) {
                // 원본 픽셀 위치 계산
                int src_i = (j * rows / obstacle->height) * cols * 3;
                int src_j = (k * cols / obstacle->width) * 3;

                // RGB 값 읽기
                b = *(data + src_i + src_j);
                g = *(data + src_i + src_j + 1);
                r = *(data + src_i + src_j + 2);

                // 픽셀 값 생성
                pixel = ((r << 16) | (g << 8) | b);

                // 지정된 버퍼 인덱스에 따라 프레임버퍼 포인터 결정
                if (idx_buffer == 0) {
                    ptr = (unsigned long *)fb_mapped0 + (y + j) * screen_width + (obstacle->x + k);
                } else if (idx_buffer == 1) {
                    ptr = (unsigned long *)fb_mapped1 + (y + j) * screen_width + (obstacle->x + k);
                } else {
                    printf("잘못된 버퍼 인덱스\r\n");
                    return;
                }

                // 프레임버퍼에 픽셀 쓰기
                *ptr = pixel;
            }
        }

        // 움직임을 시각화하기 위한 짧은 지연
        usleep(5000);  // 50 밀리초
    }

    // BMP 파일 닫기
    close_bmp(&pData);
}

// 장애물 이동 함수
void move_obstacle(struct Obstacle *obstacle, int new_x, int new_y) {
    obstacle->x = new_x;
    obstacle->y = new_y;
}
int display_close(void)
{
    munmap(fb_mapped0, mem_size);
    close(fb_fd0);
    munmap(fb_mapped1, mem_size);
    close(fb_fd1);
    return 0;
}
