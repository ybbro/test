#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<windows.h>


char table[3][3] = { {' ', ' ', ' '}, { ' ', ' ', ' ' } ,{ ' ', ' ', ' ' }}; // ���� ���� ��ġ
char compare[3]; // �¸� ���� ������ ����
int x, y; // ������ ������ ���� ���� ��ǥ
int random; // ��ǻ�Ͱ� ���� ������ ��� ���� ����
int com_x, com_y; // ��ǻ�Ͱ� ���� ������ ��� ���� ��ǥ
int turns = 0; // ��ǻ�Ͱ� ����°������ ���� �ൿ�� �޶���
void print_present(); // ������ ���� ����� ������ ���� ���
void enemy_turn(); // ��ǻ�Ͱ� ���� ���� ��ġ�� ����Ͽ� ����
void random_select(); // ���º��� ��� �ƹ����Գ� ���� ���� ��


int main()
{
	srand((unsigned)time(NULL));
	print_present();
	printf("��ǻ���� �����Դϴ�...\n");
	Sleep(2000);
	table[1][1] = 'x';// ��ǻ�� ����
	turns++;
	print_present();

	while (1)
	{
		print_present();

		while (1)
		{
			printf("o�� ǥ���� ��ǥ�� �Է��Ͻÿ� : ");
			scanf("%d %d", &x, &y); // �Է��� ������� �ϳ��� �������� x,y��ǥ�� �Է�
			if (table[x][y] != 'x' && table[x][y] != 'o' && (x<3&&x>=0)&& (y<3 && y >= 0)) // ������ ��� �Է��̳� �ڽ��̳� ��� ���� ���� ��ġ���� ���� ���� �� ����.
			{
				break;
			}
			printf("�߸��� �Է��Դϴ�.\n\n�ٽ� ");
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
				printf("�����մϴ�. ����� �̰���ϴ�.\n");
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
				printf("�����մϴ�. ����� �̰���ϴ�.\n");
				return 0;
			}
		}
		if ((table[0][0] == 'o' && table[1][1] == 'o' && table[2][2] == 'o') || (table[2][0] == 'o' && table[1][1] == 'o' && table[0][2] == 'o'))
		{
			printf("�����մϴ�. ����� �̰���ϴ�.\n");
			return 0;
		}

		printf("��ǻ���� ���Դϴ�...\n");
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
				printf("��ǻ�Ͱ� �̰���ϴ�.\n");
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
				printf("��ǻ�Ͱ� �̰���ϴ�.\n");
				return 0;
			}
		}
		if ((table[0][0] == 'x' && table[1][1] == 'x' && table[2][2] == 'x') || (table[2][0] == 'x' && table[1][1] == 'x' && table[0][2] == 'x'))
		{
			printf("��ǻ�Ͱ� �̰���ϴ�.\n");
			return 0;
		}

		if (turns == 5)
		{
			printf("���º��Դϴ�.\n");
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
	random = rand() % 3; // ���ºΰ� �������� �ñ�� ��ǻ�� ���� ���� 4��°�� �����Ҷ�(6���� ���� ����)�̴�. ���� ���� �ڸ��� 3�ڸ���
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
	//�����϶�(ù���� ������ �߾�)
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
				table[2][2] = 'x'; // �� ��� com ��
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
				table[2][0] = 'x'; // �� ��� com ��
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
				table[2][2] = 'x'; // �� ��� com ��
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
				table[0][2] = 'x'; // �� ��� com ��
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
				table[2][0] = 'x'; // �� ��� com ��
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
				table[0][0] = 'x'; // �� ��� com ��
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
				table[0][2] = 'x'; // �� ��� com ��
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
				table[0][0] = 'x'; // �� ��� com ��
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



	// �İ��϶�(2��° ����� ���� ����)
	/*if (turns == 1) // ù ��
	{
		if (table[1][1] == ' ') // �߾��� ��� �߾ӿ� �α�
			table[1][1] == 'x';

		else // ù�Ͽ� �÷��̾ �߾ӿ� �ξ��� ��� ������ �κп� �α�
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

	else if (turns == 2) // ��° ��
	{
		if (table[1][1] == 'o' && random==0) // �÷��̾ ù �Ͽ� �߾��� �����ϰ� ���� [0 0]�� �ξ��� ���
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

