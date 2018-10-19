//�Ǻ���ġ ����, �ϳ����� ž

#include<stdio.h>
#include<windows.h>
#include<mmsystem.h>

#pragma comment(lib, "winmm.lib")
#define whoareyou "C:\\workspace_vs\\whoareyou.wav"
#define ddack "C:\\workspace_vs\\ddack.wav"
#define frustration "C:\\workspace_vs\\frustration.wav"
#define tetris "C:\\workspace_vs\\tetris.wav"
#define stupid "C:\\workspace_vs\\stupid.wav"


#define height_MAX	12			// 3������ 12������ ���� -> 12 �ʰ��� cmdâ���� ǥ���Ұ�, 3 �̸��� �ǹ̾���
char coordinate_of_tower[(height_MAX+3)][(6*height_MAX+3)];  // 2���� �迭�� ���� �ϳ��� ž ��ü ������ ��ǥ�� ���� 
int flr[3] = { 0,0,0 }; // flr[n] : (n+1) ��° ���뿡 ���� ���� �� -> �̸� ������ ��(3��° ���뿡 7���� ��)�� ���� + ���� ���ǰ� �����Ͽ� �� ���뿡 ���� �� ���� ũ�⵵ ��Ÿ��


void initial_state(int height);
void print_tower(int height);
void move_of_block(int height,int from,int to);
void ClearEnterBuffer();
void victory(int height);
void error(int height);
void com(int height, int from, int tmp, int to,int floor);

int i = 0, j = 0, fail=0;
int i1, j1, count1; // ù��° ������ ���� ���� ž�� ��ǥ + ũ��
int i2, j2, count2; // �ι�° ������ ���� ���� ž�� ��ǥ + ũ��


void main()
{
	// �Ǻ���ġ ����
	//num�� �ݺ� Ƚ���� �Է¹޴� ����
	/*int num, pib1=1, pib2=1, pib3; // pib1,pib2�� �Ǻ���ġ ���� ���� �� ��꿡 ���� �� ����, pib3�� pib1, pib2�� ����ؼ� �������� �Ѱ��ٶ� �߰��� ���� �����ϴ� �뵵
	printf("�Ǻ���ġ ����\n\n");
	printf("���°���� ������ �Է��Ͻÿ� : ");
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


	// �ϳ����� ž
	int height=0, from, to, tmp;
	while (height < 3 || height>12)
	{
		printf("�ϳ����� ž�� ���̸� �Է��ϼ���(3 ~ 12) : ");
		scanf("%d", &height);
	}
	flr[0] = height; // �ʱ⿡�� 1�� ���뿡 ��� ���� �����ִ�. 

	char sel='a';
	while (1)
	{
		printf("���� �÷����Ͻðڽ��ϱ�? (y / n) : ");
		scanf(" %c", &sel);
		if (sel == 'y' || sel == 'Y' || sel == 'n' || sel == 'N')
			break;
	}

	initial_state(height); // ž�� ���ʿ� ���� ���¸� 2d ��ǥ�� �迭�� �Ҵ���
	print_tower(height); // ž�� ���ʿ� ���� ���¸� ���
	PlaySound(TEXT(tetris), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	if (sel == 'y' || sel == 'Y') // ������ ���� ž�� �ű�
	{
		while (flr[2] < height)
		{
			printf("���� ���� ���� �ű� ���븦 �����Ͻÿ� :");
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

	else // ž�� �ڵ����� �̵�
	{
		int floor = height;
		from = 1, to = 3, tmp = 2;
		com(height, from, tmp, to, floor);
		Sleep(1000);
		victory(height);
	}
}

void initial_state(int height)	// ���� ó�� �ϳ���ž�� ù ���뿡 ���ΰ��� ǥ�� -> 2���� �迭�� ���Ŀ� �� ��ǥ�� ���� ���� ���� ����
{
	for (i=0; i < (height + 3); i++) // num+2 ��ŭ ���� [������ ����(ž�� ����+1)�̰� �� ���� �� ���� 2ĭ ����]
	{
		if (i == 0 || i == 1) // ���� ���� 2���� ����д�
		{
			for (j = 0; j < (6 * height + 3); j++)
				coordinate_of_tower[i][j] = ' ';
		}
		else if (i > 1)
		{
			for (j=0; j < (height - i+2); j++) // ž�� �� �Ʒ��� ������ 2�踸ŭ ����� ���� ���̸� ���� ž�� ���� �Ʒ����� ���밡 ��ġ�� �ʴ´�. 
			{
				coordinate_of_tower[i][j] = ' ';
			}
			for (j; j < (height + i -1); j++) // �ϳ����� ž�� ���� ǥ��
			{
				if(j== (height - i + 2)&&i==2)
					coordinate_of_tower[i][j] = '|';
				else
					coordinate_of_tower[i][j] = '*';
			}
			for (j; j < (3 * height+1); j++) // 1->2 ���� ������ ������ �����
			{
				coordinate_of_tower[i][j] = ' ';
			}
			coordinate_of_tower[i][j] = '|';// 2������ ���� �����
			j++; // �� ������ ������ ���밡 ���̾� �� �������� �������
			for (j; j < (5 * height+2); j++) // 2->3 ���� ������ ������ �����
			{
				coordinate_of_tower[i][j] = ' ';
			}
			coordinate_of_tower[i][j] = '|'; // 3������ ���� �����
			j++;
			for (j; j < (6 * height + 3); j++) // 3���� ������ ��� ���鿡 ���� ' '�� ��������
			{
				coordinate_of_tower[i][j] = ' ';
			}
		}
	}
}

void print_tower(int height) // �� ���� ����� �ϳ����� ž + ������ ����� ���ο� �ϳ����� ž ���
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

void move_of_block(int height,int from, int to) // �Է��� ����� ���� ���� ���� ���� ������
{
	if (from == 1) // from�� ������ ���� ���� Ÿ���� �����κ��� 1ĭ ������ ������ ��ǥ�� ����
		j1 = height - 1;
	else if (from == 2)
		j1 = 3 * height;
	else if (from == 3)
		j1 = 5 * height + 1;

	i1 = height + 3 - flr[from - 1]; // from�� ������ ���� ���� Ÿ���� ������ ��ǥ�� ����

	count1 = 0;
	if (flr[from - 1] != 0)
	{
		while (coordinate_of_tower[i1][j1] == '*')
		{
			count1++; // count1���� from�� ������ ���� ������ ũ�⸦ ����
			j1--; // Ÿ���� ���� ���� ���� ��ǥ�� -1�� ����
		}
	}

	if (count1 != 0)
		j1++; //Ÿ���� ���� ���� ���� ��ǥ�� ����

	if (to == 1) // from�� ������ ���� ���� Ÿ���� �����κ��� 1ĭ ������ ������ ��ǥ�� ����
		j2 = height - 1;
	else if (to == 2)
		j2 = 3 * height;
	else if (to == 3)
		j2 = 5 * height + 1;

	i2 = height + 3 - flr[to - 1]; // sel�� ������ ���� ���� Ÿ���� ������ ��ǥ�� ���� -> (i2-1) =  �̵��� ���� ���̴� ������ ��ǥ

	count2 = 0;
	if (flr[to - 1] != 0)
	{
		while (coordinate_of_tower[i2][j2] == '*')
		{
			count2++; // count1���� from�� ������ ���� ������ ũ�⸦ ����
			j2--;
		}
	}

	if (count2 != 0)
		j2 += count2;

	if (count1 > count2 && count2 != 0 || count1 == 0) // �̵��Ϸ��� ������ �ֻ��� ��Ϻ��� ū ���� ���� �ű� �� ����, �ƹ����� ������ ���� ��� �̵� ����
	{
		error(height);
	}

	else
	{
		for (i = i1; i > 0; i--) // �ű���� �ϴ� ������ ���� �ø�
		{
			for (int j = 0; j < (count1 * 2 + 1); j++)
			{
				coordinate_of_tower[i][j1 + j] = ' '; // �ű���� ���� �κ��� ����

				if (((j1 + j) == 3 * height + 1 || (j1 + j) == 5 * height + 2 || (j1 + j) == height) && i != 1) // ���밡 ������ �κ��� *�� ǥ��
					coordinate_of_tower[i][j1 + j] = '|';

				coordinate_of_tower[i - 1][j1 + j] = '*'; // ������ 1ĭ ���� �ø�
			}
			Sleep(30);
			print_tower(height);
		}

		if (j1 < j2) // �������� �̵�
		{
			for (int j = j1; j < (j2 - count1 + 1); j++)
			{
				coordinate_of_tower[0][j] = ' ';
				coordinate_of_tower[0][j + i + 2 * count1 + 1] = '*';
				Sleep(30);
				print_tower(height);
			}
		}
		else // (j1>j2) �������� �̵�
		{
			for (int j = j1; j > j2 - count1 + 1; j--)
			{
				coordinate_of_tower[0][j + 2 * count1] = ' ';
				coordinate_of_tower[0][j - 1] = '*';
				Sleep(30);
				print_tower(height);
			}
		}

		for (int i = 0; i < i2 - 1; i++) // ��ǥ ���뿡 ������
		{
			for (int j = 0; j < (count1 * 2 + 1); j++)
			{
				coordinate_of_tower[i][j2 - (count1 - 1) + j] = ' '; // �ű���� ���� �κ��� ����

				if (((j2 + j - (count1 - 1)) == 3 * height + 1 || (j2 + j - (count1 - 1)) == 5 * height + 2 || (j2 + j - (count1 - 1)) == height) && i != 1 && i != 0) // ���밡 ������ �κ��� |�� ǥ��
					coordinate_of_tower[i][j2 + j - (count1 - 1)] = '|';

				coordinate_of_tower[i + 1][j2 + j - (count1 - 1)] = '*'; // ������ 1ĭ �Ʒ��� ����
			}
			Sleep(30);
			print_tower(height);
		}

		flr[to - 1]++; // ������ �̵��� ����鿡 ���� ���� ������ ���ϰ� ����
		flr[from - 1]--;
	}
}

void ClearEnterBuffer() // �߸��� ���� �Է��� ��� Ű���� ���۸� ������ ���� �Լ� ����
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
	// Ÿ�� ������ 3������ ����
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
	printf("     **   �߸��� �̵�  **\n");
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