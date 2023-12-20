
//이 코드는 Linux 기반 시스템에서 FND (Fluorescent Numeric Display) 디바이스를 제어하기 위한 프로그램이다.
// FND는 일반적으로 7-segment 디스플레이로 구성되어 숫자를 표시하는 데 사용된다. 
#include "fnd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

//매크로 정의 및 구조체 설명
#define FND_DRIVER_NAME		"/dev/perifnd"      //FND 디바이스 드라이버 파일의 경로를 지정한다. 
#define MAX_FND_NUM			6           //FND에 표시할 수 있는 숫자의 최대 자릿수를 정의한다. 
#define FND_DATA_BUFF_LEN	(MAX_FND_NUM + 2)   // FND에 전송할 데이터를 담을 버퍼의 크기를 정의한다. 


typedef struct FNDWriteDataForm_tag 
{
	char	DataNumeric[FND_DATA_BUFF_LEN];
	char	DataDot[FND_DATA_BUFF_LEN];
	char	DataValid[FND_DATA_BUFF_LEN];	
}stFndWriteForm,*pStFndWriteForm;     //stFndWriteForm: FND에 전송할 데이터를 담는 구조체로, 숫자, 소수점 여부 및 데이터 유효성을 표현


#define FND_DRIVER_NAME		"/dev/perifnd"

void doHelp(void)    //프로그램 사용법에 대한 도움말을 출력하는 함수
{
	printf("Usage:\n");
	printf("fndtest [option] [displaynum] >\n");
	printf("option   1  : display  , displaynum range 0 ~ 999999\n");
	printf("option   0  : fnd off\n");
	printf("ex) fndtest 1 '123456'  ; display  '123456' \n");
}

#define ONE_SEG_DISPLAY_TIME_USEC	1000
// return 1 => success  , 0 => error
int fndDisp(int num , int dotflag)    //num은 FND에 숫자를 표시한다.   //dotflag는 각 자리에 소수점을 표시할지 여부를 나타내는 플래그
{
	int fd;
	int temp,i;
	stFndWriteForm stWriteData;
	
	for (i = 0; i < MAX_FND_NUM ; i++ )
	{
		stWriteData.DataDot[i] = (dotflag & (0x1 << i)) ? 1 : 0;  
		stWriteData.DataValid[i] = 1;
	}
	// if 6 fnd
	temp = num % 1000000;	stWriteData.DataNumeric[0]= temp /100000;  
	temp = num % 100000;	stWriteData.DataNumeric[1]= temp /10000;
	temp = num % 10000;		stWriteData.DataNumeric[2] = temp /1000;
	temp = num %1000;		stWriteData.DataNumeric[3] = temp /100;
	temp = num %100;		stWriteData.DataNumeric[4] = temp /10;
							stWriteData.DataNumeric[5] = num %10;

	fd = open(FND_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
		return 0;
	}	
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	close(fd);
	return 1;
}

int fndOff()      //fndOff 함수는 FND를 끄는 함수로, 모든 자리에 0을 표시하여 FND를 끈다. 
{
	int fd,i;  //fd는  FND 디바이스 드라이버를 열기 위한 파일 디스크립터
	stFndWriteForm stWriteData;
	
	for (i = 0; i < MAX_FND_NUM ; i++ )
	{
		stWriteData.DataDot[i] =  0;  
		stWriteData.DataNumeric[i] = 0;
		stWriteData.DataValid[i] = 0;
	}
	fd = open(FND_DRIVER_NAME,O_RDWR);
	if ( fd < 0 )
	{
		perror("driver open error.\n");
		return 0;
	}	
	write(fd,&stWriteData,sizeof(stFndWriteForm));
	close(fd);
	return 1;
}
