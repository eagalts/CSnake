
#include <stdio.h> //Ñòàíäðàòíàÿ áèáëèîòåêà ñè
#include <stdlib.h> //Ââîä-âûâîä
#include <time.h> //Âðåìÿ è ðàíäîì
#include "main.h" //Îòêðûòèå ôàéëà çàãîëîâêîâ

#ifdef _WIN32 ///Windows - ïëàòôîðìà
#include <Windows.h> //Áèáëèîòåêà íóæíà äëÿ èñïîëüçîâàíèÿ ôóíêöèè Sleep(); 
#include <conio.h> //Áèáëèîòåêà íóæíà äëÿ èñïîëüçîâàíèÿ ôóíêöèè getch() è kbhit() (îòñëåæèâàíèå êëàâèø)

#else ///UNIX - Ïëàòôîðìà
#include <unistd.h>
#include <terminos.h>
#include <sys/select.h>

//Ïåðåîïðåäåëåíèå äëÿ UNIX

#define STDIN_FILENO 0
#define NB_DISABLE 0
#define NB_DISABLE 1

#define Sleep(x) usleep(x*1000)

int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
	struct terminos ttystate;
	tcgetattr(STD_INFILENO, &ttystate);

	if (state == NB_ENABLE)
	{
		ttystate.c_flag &= ~ICANON;
		ttystate.c_cc[VMIN] = 1;
	}
	else if (state == NB_DISABLE)
	{
		ttystate.c_flag |= ICANON;
	}
	tcgetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int getch()
{
	return fgetch(stdin);
}

#endif
#define SIZE 1000

const int H = 13, W = 17, INTERVAL = 200; //îáíîâëåíèå ýêðàíà

int snake_size; //ðàçìåð çìåéêè
int change_x, change_y, coordinates_x[SIZE], coordinates_y[SIZE]; //êîîðäèíàòû
int food_x = -1, food_y = -1; //êîîðäèíàòû åäû
char symbol, a[SIZE][SIZE];


int main(void)
{
	standart_settings();
	memset(a, ' ', sizeof(a));
#ifndef _WIN32
	nonblock(NB_ENABLE);
#endif // !_WIN32

	while (TRUE)
	{
		if (kbhit()) change_direction();
		next_step();

		if (!food_check()) place_food();
		show_table();
		Sleep(INTERVAL);
	}
	return 0;
}


void change_direction() //Îòñëåæèâàíèå íàæàòîé êëàâèøè è èçìåíåíèå íàïðàâëåíèÿ
{
	symbol = getch();
	switch (symbol)
	{
	case 'w':
		if (change_x != 1 || change_y != 0)
		{
			change_x = -1;
			change_y = 0;
		}
		break;
+


	case 'a':
		if (change_x != 0 || change_y != 1)
		{
			change_x = 0;
			change_y = -1;
		}
		break;



	case 's':
		if (change_x != -1 || change_y != 0)
		{
			change_x = 1;
			change_y = 0;
		}
		break;



	case 'd':
		if (change_x != 0 || change_y != -1)
		{
			change_x = 0;
			change_y = 1;
		}
		break;

	case 'q':
#ifndef _WIN32
		nonblock(NB_DISABLE);
#endif // !_WIN32
		exit(0);
		
	default:
		break;


	}
}

void next_step() //Îòñëåæèâàåò øàã çìåéêè
{
	int i;
	clear_snake_on_table(); 
	for (i = snake_size; i >= 2; --i)
	{
		coordinates_x[i] = coordinates_x[i - 1];
		coordinates_y[i] = coordinates_y[i - 1];
	}
	coordinates_x[1] += change_x;
	coordinates_y[1] += change_y;

	check_coordination();

	if (coordinates_x[1] == food_x
		&& coordinates_y[1] == food_y)
	{
		snake_size++;
		food_x = -1;
		food_y = -1;
	}

	show_snake_on_table();

	if (game_over())
	{
		printf("Hmm...\n");
#ifdef _WIN32
		system("pause");
#endif // _WIN32
		exit(0);
	}
}

void place_food() //ðàññòàíîâêà ôðóêòîâ ðàíäîìíî
{
	int i, x, y;
	srand(time(NULL));
	for (i = 1; i < 10; ++i)
	{
		x = rand();
		y = rand();
		if (x < 0) x *= -1;
		if (y < 0) y *= -1;
		x %= (H + 1);
		y %= (W + 1);
		if (x == 0) ++x;
		if (y == 0) ++y;
		if (a[x][y] != '@'
			&& a[x][y] != 'v'
			&& a[x][y] != '^'
			&& a[x][y] != '<'
			&& a[x][y] != '>')
		{
			food_x = x;
			food_y = y;
			a[x][y] = '+';
			return;
		}
	}
}

void show_table() //Ðàñ÷åðòèòü ïîëå èãðîêà
{
	int i, j;
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN

	for (i = 0; i <= H + 1; ++i)
	{
		for (j = 0; j <= W + 1; ++j)
		{
			(i == 0 || j == 0 || i == H + 1|| j == W + 1) ?
				printf("#") : printf("%c", a[i][j]);
			(j <= W) ? printf("") : printf("\n");
		}
	}
}

void clear_snake_on_table() //Óáèðàåì çìåéêó ïîñëå ïðîèãðûøà
{
	int i;
	for (i = 1; i <= snake_size; ++i)
		a[coordinates_x[i]][coordinates_y[i]] = ' ';
}

void check_coordination() //ïðîâåðêà êîîðäèíàò 
{
	if (coordinates_x[1] > H) coordinates_x[1] = 1;
	if (coordinates_x[1] < 1) coordinates_x[1] = H;
	if (coordinates_y[1] > W) coordinates_y[1] = 1;
	if (coordinates_y[1] < 1) coordinates_y[1] = W;
}

void show_snake_on_table() //ïîêàçûâàåì çìåéêó
{
	int i;
	if (change_x == 1 && change_y == 0)
		a[coordinates_x[1]][coordinates_y[1]] = 'v';
	if (change_x == -1 && change_y == 0)
		a[coordinates_x[1]][coordinates_y[1]] = '^';
	if (change_x == 0 && change_y == 1)
		a[coordinates_x[1]][coordinates_y[1]] = '>';
	if (change_x == 0 && change_y == -1)
		a[coordinates_x[1]][coordinates_y[1]] = '<';

	for(i = 2; i < snake_size; ++i)
			a[coordinates_x[i]][coordinates_y[i]] = '@';
}

int game_over() //çìåéêà ñêóøàëà ñàìà ñåáÿ
{
	int i;
	for (i = 2; i < snake_size; ++i)
	{
		if (coordinates_x[1] == coordinates_x[i]
			&& coordinates_y[1] == coordinates_y[i])
			return TRUE;
	}
	return FALSE;
}

void standart_settings() //ñòàíäàðòíûå íàñòðîéêè
{
	snake_size = 2;
	coordinates_x[1] = 1;
	coordinates_y[1] = 2;
	coordinates_x[2] = 1;
	coordinates_y[2] = 1;
	change_x = 0;
	change_y = 1;
}

int food_check() //ïðîâåðÿåì åäó
{
	if (food_x == -1 && food_y == -1) return FALSE;
	return TRUE;
}
