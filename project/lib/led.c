#include "led.h"

static unsigned int ledValue = 0;
static int fd = 0;

int ledInit(void)
{
    fd = open(LED_DRIVER_NAME, O_WRONLY);
    if (fd < 0)
    {
        perror("driver (//dev//cnled) open error.\n");
        return 1;
    }
    ledValue = 0;
}

int ledOnOff(int ledNum, int onOff) // ON or OFF
{
    int i = 1;
    i = i << ledNum;

    switch (onOff)
    {
    case ON:
        ledValue |= i;
        break;

    case OFF:
        ledValue = ledValue & (~i);
        break;

    default:
        printf("Error\r\n");
    }
    write(fd, &ledValue, 4);
}

void ledOffAll()
{
    ledValue = 0;  // 모든 비트를 0으로 설정하여 모든 LED를 끕니다.
    write(fd, &ledValue, 4);
}



int ledStatus(void)
{
    for (int i = 0; i < 8; i++)
        printf("%d led on :%d\n", i, (ledValue >> i) & 1);
    printf("\r\n");
}

int ledExit(void)
{
    ledValue = 0;
    ledOnOff(0, 0);
    close(fd);
}
