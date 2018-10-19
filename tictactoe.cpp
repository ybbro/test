#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>


char table[3][3] = { {' ', ' ', ' '}, { ' ', ' ', ' ' } ,{ ' ', ' ', ' ' }}; // 돌이 놓인 위치
char compare[3]; // 승리 조건 판정에 쓰임
int x, y; // 직전에 유저가 돌을 놓은 좌표
int random; // 컴퓨터가 랜덤 선택할 경우 쓰는 변수
int com_x, com_y; // 컴퓨터가 랜덤 선택할 경우 쓰는 좌표
int turns = 0; // 컴퓨터가 몇턴째인지에 따라 행동이 달라짐
void print_present(); // 직전의 판을 지우고 현재의 판을 출력
void enemy_turn(); // 컴퓨터가 돌을 놓는 위치를 계산하여 놓음
void random_select(); // 무승부일 경우 아무렇게나 돌을 놓게 함


int main()
{
	srand((unsigned)time(NULL));
	print_present();
	printf("컴퓨터의 선공입니다...\n");
	Sleep(2000);
	table[1][1] = 'x';// 컴퓨터 선턴
	turns++;
	print_present();

	while (1)
	{
		print_present();

		while (1)
		{
			printf("o를 표시할 좌표를 입력하시오 : ");
			scanf("%d %d", &x, &y); // 입력은 띄워쓰기 하나를 간격으로 x,y좌표를 입력
			if (table[x][y] != 'x' && table[x][y] != 'o' && (x<3&&x>=0)&& (y<3 && y >= 0)) // 범위를 벗어난 입력이나 자신이나 상대 말이 놓인 위치에는 말을 놓을 수 없다.
			{
				break;
			}
			printf("잘못된 입력입니다.\n\n다시 ");
		}

		table[x][y] = 'o';

		print_present();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				compare[j] = table[i][j];
			}
			if (compare[0] == 'o' && compare[1] == 'o' && compare[2] == 'o')
			{
				printf("축하합니다. 당신이 이겼습니다.\n");
				return 0;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				compare[j] = table[j][i];
			}
			if (compare[0] == 'o' && compare[1] =='o' && compare[2] =='o')
			{
				printf("축하합니다. 당신이 이겼습니다.\n");
				return 0;
			}
		}
		if ((table[0][0] == 'o' && table[1][1] == 'o' && table[2][2] == 'o') || (table[2][0] == 'o' && table[1][1] == 'o' && table[0][2] == 'o'))
		{
			printf("축하합니다. 당신이 이겼습니다.\n");
			return 0;
		}

		printf("컴퓨터의 턴입니다...\n");
		Sleep(1000);
		turns++;

		enemy_turn();

		print_present();

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				compare[j] = table[i][j];
			}
			if (compare[0] == 'x' && compare[1] == 'x' && compare[2] == 'x')
			{
				printf("컴퓨터가 이겼습니다.\n");
				return 0;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				compare[j] = table[j][i];
			}
			if (compare[0] == 'x' && compare[1] == 'x' && compare[2] == 'x')
			{
				printf("컴퓨터가 이겼습니다.\n");
				return 0;
			}
		}
		if ((table[0][0] == 'x' && table[1][1] == 'x' && table[2][2] == 'x') || (table[2][0] == 'x' && table[1][1] == 'x' && table[0][2] == 'x'))
		{
			printf("컴퓨터가 이겼습니다.\n");
			return 0;
		}

		if (turns == 5)
		{
			printf("무승부입니다.\n");
			return 0;
		}
	}
}

void print_present()
{
	system("cls");

	printf("\n");
	printf("\n	-------------\n");
	for (int i = 0; i < 3; i++)
	{
		printf("	|");
		for (int j = 0; j < 3; j++)
		{
			printf(" %c |" ,table[i][j]);
		}
		printf("\n	-------------\n");
	}
	printf("\n");
}
void random_select()
{
	random = rand() % 3; // 무승부가 결정나는 시기는 컴퓨터 선턴 기준 4턴째를 시작할때(6개의 돌이 놓임)이다. 따라서 남는 자리는 3자리뿐
	int count=0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (random == count && table[i][j] == ' ')
				table[i][j] = 'x';

			if (table[i][j] == ' ')
				count++;

		}
	}
}
void enemy_turn()
{
	//선공일때(첫턴은 무조건 중앙)
	if (turns == 2)
	{
		if (table[0][0] == 'o')
		{
			table[2][2] = 'x';
		}
		else if (table[0][2] == 'o')
		{
			table[2][0] = 'x';
		}
		else if (table[2][0] == 'o')
		{
			table[0][2] = 'x';
		}
		else if (table[2][2] == 'o')
		{
			table[0][0] = 'x';
		}

		else if (table[0][1] == 'o')
		{
			random = rand() % 2;
			if (random == 0)
				table[0][0] = 'x';
			else
				table[0][2] = 'x';
		}
		else if (table[1][0] == 'o')
		{
			random = rand() % 2;
			if (random == 0)
				table[0][0] = 'x';
			else
				table[2][0] = 'x';
		}
		else if (table[1][2] == 'o')
		{
			random = rand() % 2;
			if (random == 0)
				table[0][2] = 'x';
			else
				table[2][2] = 'x';
		}
		else
		{
			random = rand() % 2;
			if (random == 0)
				table[2][0] = 'x';
			else
				table[2][2] = 'x';
		}
	}

	else if (turns == 3)
	{
		if (table[0][0] == 'o' && table[2][2] == 'x')
		{
			if (table[1][0] == 'o')
			{
				table[2][0] = 'x';
			}
			else if (table[0][1] == 'o')
			{
				table[0][2] = 'x';
			}
			else if (table[0][2] == 'o')
			{
				table[0][1] = 'x';
			}
			else if (table[2][0] == 'o')
			{
				table[1][0] = 'x';
			}
			else if (table[1][2] == 'o')
			{
				table[2][1] = 'x';
			}
			else
			{
				table[1][2] = 'x';
			}
		}

		else if (table[0][2] == 'o' && table[2][0] == 'x')
		{
			if (table[0][0] == 'o')
			{
				table[0][1] = 'x';
			}
			else if (table[0][1] == 'o')
			{
				table[0][0] = 'x';
			}
			else if (table[1][2] == 'o')
			{
				table[2][2] = 'x';
			}
			else if (table[2][2] == 'o')
			{
				table[1][2] = 'x';
			}
			else if(table[1][0] == 'o')
			{
				table[2][1] = 'x';
			}
			else if (table[2][1] == 'o')
			{
				table[1][0] = 'x';
			}
		}

		else if (table[2][0] == 'o' && table[0][2] == 'x')
		{
			if (table[0][0] == 'o')
			{
				table[1][0] = 'x';
			}
			else if (table[1][0] == 'o')
			{
				table[0][0] = 'x';
			}
			else if (table[2][2] == 'o')
			{
				table[2][1] = 'x';
			}
			else if (table[2][1] == 'o')
			{
				table[2][2] = 'x';
			}
			else if (table[0][1] == 'o')
			{
				table[2][2] = 'x';
			}
			else if (table[1][2] == 'o')
			{
				table[0][1] = 'x';
			}
		}

		else if (table[2][2] == 'o' && table[0][0] == 'x')
		{
			if (table[2][0] == 'o')
			{
				table[2][1] = 'x';
			}
			else if (table[2][1] == 'o')
			{
				table[2][0] = 'x';
			}
			else if (table[0][2] == 'o')
			{
				table[1][2] = 'x';
			}
			else if (table[1][2] == 'o')
			{
				table[0][2] = 'x';
			}
			else if(table[0][1] == 'o')
			{
				table[1][0] = 'x';
			}
			else if (table[1][0] == 'o')
			{
				table[0][1] = 'x';
			}
		}

		else if (table[0][1] == 'o' && table[0][0] == 'x')
		{
			if (table[2][2] == ' ')
			{
				table[2][2] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[1][0] = 'x';
			}
		}
		else if (table[0][1] == 'o' && table[0][2] == 'x')
		{
			if (table[2][0] == ' ')
			{
				table[2][0] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[1][2] = 'x';
			}
		}

		else if (table[1][0] == 'o' && table[0][0] == 'x')
		{
			if (table[2][2] == ' ')
			{
				table[2][2] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[0][1] = 'x';
			}
		}
		else if (table[1][0] == 'o' && table[2][0] == 'x')
		{
			if (table[0][2]== ' ')
			{
				table[0][2] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[2][1] = 'x';
			}
		}

		else if (table[1][2] == 'o' && table[0][2] == 'x')
		{
			if (table[2][0] == ' ')
			{
				table[2][0] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[0][1] = 'x';
			}
		}
		else if (table[1][2] == 'o' && table[2][2] == 'x')
		{
			if (table[0][0] == ' ')
			{
				table[0][0] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[2][1] = 'x';
			}
		}

		else if (table[2][1] == 'o' && table[2][0] == 'x')
		{
			if (table[0][2] == ' ')
			{
				table[0][2] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[1][0] = 'x';
			}
		}
		else if (table[2][1] == 'o' && table[2][2] == 'x')
		{
			if (table[0][0] == ' ')
			{
				table[0][0] = 'x'; // 이 경우 com 승
			}
			else
			{
				table[1][2] = 'x';
			}
		}
	}
	
	else if (turns == 4)
	{
		if (table[0][0] == 'o' && table[1][0] == 'o' && table[2][0] == 'x'&& table[2][2] == 'x')
		{
			if (table[2][1] == ' ')
				table[2][1] = 'x';
			else
				table[0][2] = 'x';
		}
		else if (table[0][0] == 'o' && table[0][1] == 'o' && table[0][2] == 'x'&& table[2][2] == 'x')
		{
			if (table[2][0] == ' ')
				table[2][0] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[0][0] == 'o' && table[0][2] == 'o' && table[0][1] == 'x'&& table[2][2] == 'x')
		{
			if (table[2][1] == ' ')
				table[2][1] = 'x';
			else
				table[1][0] = 'x';
		}
		else if (table[0][0] == 'o' && table[2][0] == 'o' && table[1][0] == 'x'&& table[2][2] == 'x')
		{
			if (table[1][2] == ' ')
				table[1][2] = 'x';
			else
				table[0][1] = 'x';
		}
		else if (table[0][0] == 'o' && table[1][2] == 'o' && table[2][1] == 'x'&& table[2][2] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[0][0] == 'o' && table[2][1] == 'o' && table[1][2] == 'x'&& table[2][2] == 'x')
		{
			if (table[1][0] == ' ')
				table[1][0] = 'x';
			else
				table[0][2] = 'x';
		}

		else if (table[0][0] == 'o' && table[0][2] == 'o' && table[0][1] == 'x'&& table[2][0] == 'x')
		{
			if (table[2][1] == ' ')
				table[2][1] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[0][1] == 'o' && table[0][2] == 'o' && table[0][0] == 'x'&& table[2][0] == 'x')
		{
			if (table[2][2] == ' ')
				table[2][2] = 'x';
			else
				table[1][0] = 'x';
		}
		else if (table[0][2] == 'o' && table[1][2] == 'o' && table[2][0] == 'x'&& table[2][2] == 'x')
		{
			if (table[2][1] == ' ')
				table[2][1] = 'x';
			else
				table[0][0] = 'x';
		}
		else if (table[0][2] == 'o' && table[2][2] == 'o' && table[2][0] == 'x'&& table[1][2] == 'x')
		{
			if (table[1][0] == ' ')
				table[1][0] = 'x';
			else
				table[0][1] = 'x';
		}
		else if (table[0][2] == 'o' && table[1][0] == 'o' && table[2][0] == 'x'&& table[2][1] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[2][2] = 'x';
		}
		else if (table[0][2] == 'o' && table[2][1] == 'o' && table[2][0] == 'x'&& table[1][0] == 'x')
		{
			if (table[0][0] == ' ')
				table[0][0] = 'x';
			else
				table[1][2] = 'x';
		}

		else if (table[2][0] == 'o' && table[0][0] == 'o' && table[0][2] == 'x'&& table[1][0] == 'x')
		{
			if (table[1][2] == ' ')
				table[1][2] = 'x';
			else
				table[2][1] = 'x';
		}
		else if (table[2][0] == 'o' && table[1][0] == 'o' && table[0][2] == 'x'&& table[0][0] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[2][2] = 'x';
		}
		else if (table[2][0] == 'o' && table[2][2] == 'o' && table[0][2] == 'x'&& table[2][1] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[1][0] = 'x';
		}
		else if (table[2][0] == 'o' && table[2][1] == 'o' && table[0][2] == 'x'&& table[2][2] == 'x')
		{
			if (table[1][2] == ' ')
				table[1][2] = 'x';
			else
				table[0][0] = 'x';
		}
		else if (table[2][0] == 'o' && table[0][1] == 'o' && table[0][2] == 'x'&& table[2][2] == 'x')
		{
			if (table[0][0] == ' ')
				table[0][0] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[2][0] == 'o' && table[1][2] == 'o' && table[0][2] == 'x'&& table[0][1] == 'x')
		{
			if (table[0][0] == ' ')
				table[0][0] = 'x';
			else
				table[2][1] = 'x';
		}

		else if (table[2][0] == 'o' && table[2][1] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[2][1] == 'o' && table[2][0] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[0][2] == ' ')
				table[0][2] = 'x';
			else
				table[1][0] = 'x';
		}
		else if (table[0][2] == 'o' && table[1][2] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[1][0] == ' ')
				table[1][0] = 'x';
			else
				table[2][1] = 'x';
		}
		else if (table[1][2] == 'o' && table[0][2] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[2][0] = 'x';
		}
		else if (table[0][1] == 'o' && table[1][0] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[2][0] == ' ')
				table[2][0] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[1][0] == 'o' && table[0][1] == 'x' && table[2][2] == 'o'&& table[0][0] == 'x')
		{
			if (table[0][2] == ' ')
				table[0][2] = 'x';
			else
				table[2][1] = 'x';
		}

		else if (table[0][1] == 'o' && table[2][2] == 'o' && table[0][0] == 'x'&& table[1][0] == 'x')
		{
			if (table[1][2] == ' ')
				table[1][2] = 'x';
			else
				table[2][0] = 'x';
		}
		else if (table[0][1] == 'o' && table[0][2] == 'o' && table[0][2] == 'x'&& table[1][2] == 'x')
		{
			if (table[1][0] == ' ')
				table[1][0] = 'x';
			else
				table[2][2] = 'x';
		}
		else if (table[1][0] == 'o' && table[2][2] == 'o' && table[0][0] == 'x'&& table[0][1] == 'x')
		{
			if (table[0][2] == ' ')
				table[0][2] = 'x';
			else
				table[2][1] = 'x';
		}
		else if (table[0][2] == 'o' && table[1][0] == 'o' && table[2][0] == 'x'&& table[2][1] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[2][2] = 'x';
		}
		else if (table[1][2] == 'o' && table[2][0] == 'o' && table[0][1] == 'x'&& table[0][2] == 'x')
		{
			if (table[0][0] == ' ')
				table[0][0] = 'x';
			else
				table[2][1] = 'x';
		}
		else if (table[0][0] == 'o' && table[1][2] == 'o' && table[2][1] == 'x'&& table[2][2] == 'x')
		{
			if (table[0][1] == ' ')
				table[0][1] = 'x';
			else
				table[2][0] = 'x';
		}
		else if (table[0][2] == 'o' && table[2][1] == 'o' && table[2][0] == 'x'&& table[1][0] == 'x')
		{
			if (table[0][0] == ' ')
				table[0][0] = 'x';
			else
				table[1][2] = 'x';
		}
		else if (table[0][0] == 'o' && table[2][1] == 'o' && table[2][2] == 'x'&& table[1][2] == 'x')
		{
			if (table[0][2] == ' ')
				table[0][2] = 'x';
			else
				table[1][0] = 'x';
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (table[i][j] == ' ')
				{
					table[i][j] = 'x';
				}
			}
		}
	}
}



	// 후공일때(2턴째 만드는 도중 멈춤)
	/*if (turns == 1) // 첫 턴
	{
		if (table[1][1] == ' ') // 중앙이 비면 중앙에 두기
			table[1][1] == 'x';

		else // 첫턴에 플레이어가 중앙에 두었을 경우 꼭짓점 부분에 두기
		{
			random = rand() % 4;
			if (random == 0)
				table[0][0] == 'x';
			else if (random == 1)
				table[0][2] == 'x';
			else if (random == 2)
				table[2][0] == 'x';
			else
				table[2][2] == 'x';

		}
	}

	else if (turns == 2) // 둘째 턴
	{
		if (table[1][1] == 'o' && random==0) // 플레이어가 첫 턴에 중앙을 차지하고 컴이 [0 0]에 두었을 경우
		{
			if (table[0][1] == 'o')
			{
				table[2][1] == 'x';
			}
			else if (table[0][2] == 'o')
			{
				table[2][0] == 'x';
			}
			else if (table[1][0] == 'o')
			{
				table[1][2] == 'x';
			}
			else if (table[1][2] == 'o')
			{
				table[1][0] == 'x';
			}
			else if (table[2][0] == 'o')
			{
				table[0][2] == 'x';
			}
			else if (table[2][1] == 'o')
			{
				table[0][1] == 'x';
			}
			else if (table[2][2] == 'o')
			{
				table[0][1] == 'x';
			}
		}
	}*/

