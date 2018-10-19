//피보나치 수열, 하노이의 탑

#include<stdio.h>
#include<windows.h>
#include<mmsystem.h>

#pragma comment(lib, "winmm.lib")
#define whoareyou "C:\\workspace_vs\\whoareyou.wav"
#define ddack "C:\\workspace_vs\\ddack.wav"
#define frustration "C:\\workspace_vs\\frustration.wav"
#define tetris "C:\\workspace_vs\\tetris.wav"
#define stupid "C:\\workspace_vs\\stupid.wav"


#define height_MAX	12			// 3층부터 12층까지 가능 -> 12 초과는 cmd창에서 표현불가, 3 미만은 의미없음
char coordinate_of_tower[(height_MAX+3)][(6*height_MAX+3)];  // 2차원 배열을 통해 하노이 탑 전체 모형을 좌표로 설정 
int flr[3] = { 0,0,0 }; // flr[n] : (n+1) 번째 막대에 꽂힌 층의 수 -> 이를 게임의 끝(3번째 막대에 7개의 블럭)을 판정 + 구속 조건과 결합하여 각 막대에 쌓이 각 층의 크기도 나타냄


void initial_state(int height);
void print_tower(int height);
void move_of_block(int height,int from,int to);
void ClearEnterBuffer();
void victory(int height);
void error(int height);
void com(int height, int from, int tmp, int to,int floor);

int i = 0, j = 0, fail=0;
int i1, j1, count1; // 첫번째 막대의 제일 윗층 탑의 좌표 + 크기
int i2, j2, count2; // 두번째 막대의 제일 윗층 탑의 좌표 + 크기


void main()
{
	// 피보나치 수열
	//num은 반복 횟수를 입력받는 변수
	/*int num, pib1=1, pib2=1, pib3; // pib1,pib2는 피보나치 수열 다음 수 계산에 사용될 두 숫자, pib3은 pib1, pib2를 계산해서 다음수로 넘겨줄때 중간에 값을 저장하는 용도
	printf("피보나치 수열\n\n");
	printf("몇번째까지 만들지 입력하시오 : ");
	scanf("%d", &num);
	if (num == 1)
		printf("%d\n\n",pib1);
	else if (num ==2)
		printf("%d %d\n\n", pib1, pib2);
	else if (num > 2)
	{
		printf("%d %d ", pib1, pib2);
		for (int i = 2; i < num; i++)
		{
			pib3 = pib2;
			pib2 = pib1 + pib2;
			printf("%d ", pib2);
			pib1 = pib3;
		}
		printf("\n\n");
	}*/


	// 하노이의 탑
	int height=0, from, to, tmp;
	while (height < 3 || height>12)
	{
		printf("하노이의 탑의 높이를 입력하세요(3 ~ 12) : ");
		scanf("%d", &height);
	}
	flr[0] = height; // 초기에는 1번 막대에 모든 층이 꽂혀있다. 

	char sel='a';
	while (1)
	{
		printf("직접 플레이하시겠습니까? (y / n) : ");
		scanf(" %c", &sel);
		if (sel == 'y' || sel == 'Y' || sel == 'n' || sel == 'N')
			break;
	}

	initial_state(height); // 탑이 최초에 쌓인 상태를 2d 좌표로 배열에 할당함
	print_tower(height); // 탑이 최초에 쌓인 상태를 출력
	PlaySound(TEXT(tetris), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	if (sel == 'y' || sel == 'Y') // 유저가 직접 탑을 옮김
	{
		while (flr[2] < height)
		{
			printf("제일 위의 층을 옮길 막대를 선택하시오 :");
			scanf("%d %d", &from, &to);
			if (from == to || from < 1 || from>3 || to < 1 || to>3)
			{
				error(height);
				ClearEnterBuffer();
				continue;
			}
			move_of_block(height,from,to);
		}
		Sleep(1000);
		victory(height);
	}

	else // 탑이 자동으로 이동
	{
		int floor = height;
		from = 1, to = 3, tmp = 2;
		com(height, from, tmp, to, floor);
		Sleep(1000);
		victory(height);
	}
}

void initial_state(int height)	// 제일 처음 하노이탑이 첫 막대에 쌓인것을 표시 -> 2차원 배열로 이후에 각 좌표에 대한 값을 변경 가능
{
	for (i=0; i < (height + 3); i++) // num+2 만큼 시행 [막대의 길이(탑의 높이+1)이고 그 위에 빈 공간 2칸 존재]
	{
		if (i == 0 || i == 1) // 막대 위의 2줄은 비워둔다
		{
			for (j = 0; j < (6 * height + 3); j++)
				coordinate_of_tower[i][j] = ' ';
		}
		else if (i > 1)
		{
			for (j=0; j < (height - i+2); j++) // 탑의 맨 아랫층 높이의 2배만큼 막대와 막대 사이를 띄우면 탑의 제일 아랫층과 막대가 겹치지 않는다. 
			{
				coordinate_of_tower[i][j] = ' ';
			}
			for (j; j < (height + i -1); j++) // 하노이의 탑을 별로 표현
			{
				if(j== (height - i + 2)&&i==2)
					coordinate_of_tower[i][j] = '|';
				else
					coordinate_of_tower[i][j] = '*';
			}
			for (j; j < (3 * height+1); j++) // 1->2 막대 사이의 간격을 띄워줌
			{
				coordinate_of_tower[i][j] = ' ';
			}
			coordinate_of_tower[i][j] = '|';// 2막대의 점을 찍어줌
			j++; // 이 구문이 없으면 막대가 뒤이어 올 공백으로 덮어씌워짐
			for (j; j < (5 * height+2); j++) // 2->3 막대 사이의 간격을 띄워줌
			{
				coordinate_of_tower[i][j] = ' ';
			}
			coordinate_of_tower[i][j] = '|'; // 3막대의 점을 찍어줌
			j++;
			for (j; j < (6 * height + 3); j++) // 3막대 이후의 모든 공백에 대해 ' '을 지정해줌
			{
				coordinate_of_tower[i][j] = ' ';
			}
		}
	}
}

void print_tower(int height) // 그 전에 출력한 하노이의 탑 + 구문을 지우고 새로운 하노이의 탑 출력
{
	system("cls");
	for (int i = 0; i < (height + 3); i++)
	{
		for (int j = 0; j < (6 * height + 3); j++)
		{
			printf("%c",coordinate_of_tower[i][j]);
		}
		printf("\n");
	}
}

void move_of_block(int height,int from, int to) // 입력한 결과에 따라 제일 위의 블럭을 움직임
{
	if (from == 1) // from번 막대의 제일 윗층 타워의 축으로부터 1칸 왼쪽의 가로축 좌표를 잡음
		j1 = height - 1;
	else if (from == 2)
		j1 = 3 * height;
	else if (from == 3)
		j1 = 5 * height + 1;

	i1 = height + 3 - flr[from - 1]; // from번 막대의 제일 윗층 타워의 세로축 좌표를 잡음

	count1 = 0;
	if (flr[from - 1] != 0)
	{
		while (coordinate_of_tower[i1][j1] == '*')
		{
			count1++; // count1으로 from번 막대의 제일 윗층의 크기를 판정
			j1--; // 타워의 층의 제일 왼쪽 좌표의 -1이 나옴
		}
	}

	if (count1 != 0)
		j1++; //타워의 층의 제일 왼쪽 좌표가 나옴

	if (to == 1) // from번 막대의 제일 윗층 타워의 축으로부터 1칸 왼쪽의 가로축 좌표를 잡음
		j2 = height - 1;
	else if (to == 2)
		j2 = 3 * height;
	else if (to == 3)
		j2 = 5 * height + 1;

	i2 = height + 3 - flr[to - 1]; // sel번 막대의 제일 윗층 타워의 세로축 좌표를 잡음 -> (i2-1) =  이동한 블럭이 쌓이는 세로축 좌표

	count2 = 0;
	if (flr[to - 1] != 0)
	{
		while (coordinate_of_tower[i2][j2] == '*')
		{
			count2++; // count1으로 from번 막대의 제일 윗층의 크기를 판정
			j2--;
		}
	}

	if (count2 != 0)
		j2 += count2;

	if (count1 > count2 && count2 != 0 || count1 == 0) // 이동하려는 막대의 최상층 블록보다 큰 블럭은 위로 옮길 수 없음, 아무층도 놓이지 않은 경우 이동 가능
	{
		error(height);
	}

	else
	{
		for (i = i1; i > 0; i--) // 옮기고자 하는 원반을 위로 올림
		{
			for (int j = 0; j < (count1 * 2 + 1); j++)
			{
				coordinate_of_tower[i][j1 + j] = ' '; // 옮기려는 원반 부분을 지움

				if (((j1 + j) == 3 * height + 1 || (j1 + j) == 5 * height + 2 || (j1 + j) == height) && i != 1) // 막대가 지나는 부분은 *을 표시
					coordinate_of_tower[i][j1 + j] = '|';

				coordinate_of_tower[i - 1][j1 + j] = '*'; // 원반을 1칸 위로 올림
			}
			Sleep(30);
			print_tower(height);
		}

		if (j1 < j2) // 우측으로 이동
		{
			for (int j = j1; j < (j2 - count1 + 1); j++)
			{
				coordinate_of_tower[0][j] = ' ';
				coordinate_of_tower[0][j + i + 2 * count1 + 1] = '*';
				Sleep(30);
				print_tower(height);
			}
		}
		else // (j1>j2) 좌측으로 이동
		{
			for (int j = j1; j > j2 - count1 + 1; j--)
			{
				coordinate_of_tower[0][j + 2 * count1] = ' ';
				coordinate_of_tower[0][j - 1] = '*';
				Sleep(30);
				print_tower(height);
			}
		}

		for (int i = 0; i < i2 - 1; i++) // 목표 막대에 내려줌
		{
			for (int j = 0; j < (count1 * 2 + 1); j++)
			{
				coordinate_of_tower[i][j2 - (count1 - 1) + j] = ' '; // 옮기려는 원반 부분을 지움

				if (((j2 + j - (count1 - 1)) == 3 * height + 1 || (j2 + j - (count1 - 1)) == 5 * height + 2 || (j2 + j - (count1 - 1)) == height) && i != 1 && i != 0) // 막대가 지나는 부분은 |을 표시
					coordinate_of_tower[i][j2 + j - (count1 - 1)] = '|';

				coordinate_of_tower[i + 1][j2 + j - (count1 - 1)] = '*'; // 원반을 1칸 아래로 내림
			}
			Sleep(30);
			print_tower(height);
		}

		flr[to - 1]++; // 원반이 이동한 막대들에 대해 쌓인 층수를 더하고 빼줌
		flr[from - 1]--;
	}
}

void ClearEnterBuffer() // 잘못된 값을 입력할 경우 키보드 버퍼를 날리기 위해 함수 설정
{
	while (getchar() != '\n');
}

void victory(int height)
{
	PlaySound(TEXT(frustration), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	for (int i = 0;i<height + 3;i++)
	{
		for (int j = 0;j<4 * height;j++)
		{
			coordinate_of_tower[i][j] = ' ';
		}
	}
	print_tower(height);
	// 타위 사이즈 3층부터 가능
	coordinate_of_tower[(height +3)/2][2* height -5] = 'C';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height - 4] = 'O';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height - 3] = 'M';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height - 2] = 'P';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height - 1] = 'L';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height] = 'E';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height +1 ] = 'T';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height +2] = 'E';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height + 3] = '!';
	print_tower(height);
	Sleep(500);
	coordinate_of_tower[(height + 3) / 2][2 * height + 4] = '!';
	print_tower(height);
	Sleep(500);
	Sleep(37000);
}
void error(int height)
{	system("cls");
	printf("\n\n\n\n\n");
	printf("     ********************\n");
	printf("     ********************\n");
	printf("     **                **\n");
	printf("     **   잘못된 이동  **\n");
	printf("     **                **\n");
	printf("     ********************\n");
	printf("     ********************\n");
	PlaySound(NULL, 0, 0);
	if (fail == 0)
	{
		fail++;
		PlaySound(TEXT(whoareyou), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		Sleep(1000);
	}
	else if (fail == 1)
	{
		fail++;
		PlaySound(TEXT(ddack), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		Sleep(2000);
	}
	else
	{
		fail = 0;
		PlaySound(TEXT(stupid), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		Sleep(1500);
	}

	
	PlaySound(NULL, 0, 0);
	PlaySound(TEXT(tetris), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	print_tower(height);
}

void com(int height, int from,int tmp, int to,int floor)
{
	if (floor == 1)
		move_of_block(height, from, to);
	else
	{
		com(height, from, to, tmp,floor - 1);
		move_of_block(height, from, to);
		com(height, tmp, from, to, floor - 1);
	}
}