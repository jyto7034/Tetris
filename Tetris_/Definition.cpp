#include "Main.h"

static CConsoleLogger Console;

#pragma region Tools

void Log(const char* types, ...) {
	POINT p = getXY();
	va_list ap;
	int i = 0;

	va_start(ap, types);
	while (types[i] != '\0')
	{
		switch (types[i])
		{
		case 'i':
			Console.printf("%d", va_arg(ap, int));
			break;

		case 's':
			Console.printf("%s", va_arg(ap, char*));
			break;

		default:
			abort();
			break;
		}
		i++;
	}
	va_end(ap);

	Console.printf("\n");
}

void gotoxy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

POINT getXY()
{
	POINT pos;
	CONSOLE_SCREEN_BUFFER_INFO buf;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
	pos.x = buf.dwCursorPosition.X;
	pos.y = buf.dwCursorPosition.Y;
	return pos;
}

void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void ShowBlockData(Block block) {
	Log("sisisisi", "\ntype :", block.BlockType, "\ncolor :", block.Color, "\nx :", block.x, "\ny :", block.y);
	Log("sisi", "Weight->x :", block.Weight->x,"\nWeight->y :", block.Weight->y);
} 

void PrintMapXY(Map map) {
	for (int i = 0; i < map.MapY; i++)
	{
		for (int j = 0; j < map.MapX; j++)
		{
			std::cout << " " << j << ", " << i << " ";
		}
		std::cout << std::endl;
	}
}
#pragma endregion

void thrd_InputDirectionFromUser()
{
	while (true)
	{
		if (kbhit()) {
			int keycode = getchar();
			switch (keycode)
			{
			case 80:   // Down
				break;

			case 75:   // Left
				break;

			case 77:   // Right
				break;

			default:
				break;
			}
		}
		Sleep(10);
	}
}


Shape* CreateShape() {
	Block* block = new Block[4];
	ShapeType type;
	int BlockCnt = 4;
	int r = rand() % 7;
	for (int i = 0; i < 4; i++) {
		switch (r)
		{
		case 0:		//Line
			block[i].Color = LIGHTBLUE;
			type = ShapeType::eLine;
			if (i == 0) {
				block[0].Weight->y = 0;
				block[1].Weight->y = 1;
				block[2].Weight->y = 2;
				block[3].Weight->y = 3;
			}
			break;
		case 1:		//RStick
			block[i].Color = LIGHTRED;
			type = ShapeType::eRStick;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = -1;
				block[2].Weight->y = 1;

				block[3].Weight->x = -2;
				block[3].Weight->y = 1;
			}
			break;
		case 2:		//LStick
			block[i].Color = LIGHTGREEN;
			type = ShapeType::eLStick;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = 1;
				block[2].Weight->y = 1;

				block[3].Weight->x = 2;
				block[3].Weight->y = 1;
			}
			break;
		case 3:		//RTwo
			block[i].Color = LIGHTCYAN;
			type = ShapeType::eRTwo;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = 1;
				block[2].Weight->y = 1;

				block[3].Weight->x = 1;
				block[3].Weight->y = 2;
			}
			break;
		case 4:		//LTwo
			block[i].Color = LIGHTMAGENTA;
			type = ShapeType::eLTwo;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = -1;
				block[2].Weight->y = 1;

				block[3].Weight->x = -1;
				block[3].Weight->y = 2;
			}
			break;
		case 5:		//Middle
			block[i].Color = LIGHTGRAY;
			type = ShapeType::eMiddle;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = -1;
				block[2].Weight->y = 1;

				block[3].Weight->x = 1;
				block[3].Weight->y = 1;
			}
			break;
		case 6:		//Rect
			block[i].Color = BROWN;
			type = ShapeType::eRect;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 1;
				block[1].Weight->y = 0;

				block[2].Weight->x = 1;
				block[2].Weight->y = 1;

				block[3].Weight->x = 0;
				block[3].Weight->y = 1;
			}
			break;
		}
		block[i].BlockType = BLOCK;
	}
	Shape* shape = new Shape(block, type);
	return shape;
}


BlockedType MoveShape(Map& Buffer) {
	Block* block = Buffer.ActiveShape->ElementBlocks;
	Buffer[5][6] = block[0];
	Buffer[5 + block[1].Weight->x][6 + block[1].Weight->y] = block[1];
	Buffer[5 + block[2].Weight->x][6 + block[2].Weight->y] = block[2];
	Buffer[5 + block[3].Weight->x][6 + block[3].Weight->y] = block[3];
	return BlockedType::BLOCKED_BY_BLOCK;
}

BlockedType Start(Map& Buffer) {
	
	Console.Create("Log");
	Map Front_Buffer;
	Map Back_Buffer;

	Shape* instance = CreateShape();

	Back_Buffer = Buffer;

	int size = Buffer.MapX * Buffer.MapY;

	while (true)
	{
		for (int i = 0; i < Buffer.MapY; i++)
		{
			for (int j = 0; j < Buffer.MapX; j++)
			{
				if (j % Buffer.MapX == 0) std::cout << std::endl;
				if (Front_Buffer.Buffer != NULL)
				{
					if (Front_Buffer[i][j] == Back_Buffer[i][j]) {
						gotoxy(getXY().x + 2, getXY().y);
					}
					else
					{
						Renderer(Back_Buffer.Buffer[i][j], Back_Buffer.MapX, Back_Buffer.MapY);
						Front_Buffer[i][j] = Back_Buffer[i][j];
					}
				}
				else
				{
					if (((i + 1) * (j + 1)) == size)
						Front_Buffer = Back_Buffer;
					Renderer(Back_Buffer.Buffer[i][j], Back_Buffer.MapX, Back_Buffer.MapY);
				}
			}
		}
		gotoxy(0, 0);
		Sleep(500);
		Back_Buffer.SetActiveShape(instance);
		MoveShape(Back_Buffer);
	}
	return BlockedType::BLOCKED_BY_ENDLINE;
}

void Renderer(Block Buffer, int x, int y) {
	switch (Buffer.BlockType)
	{
	case BLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		break;
	}
	case BLANK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "д¤";
		break;
	}
	case WALL: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		break;
	}
	case INACTBLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бр";
		break;
	}
	default:
		break;
	}
}

Map::Map(int x, int y) {
	this->ActiveShape = NULL;
	this->MapX = x;
	this->MapY = y;
	this->Buffer = new Block * [y];
	for (int i = 0; i < y; i++)
		this->Buffer[i] = new Block[x];

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			if (j == 0 || j == x - 1 || i == 0 || i == y - 1)
				this->Buffer[i][j] = Block(WHITE, BLOCK, j, i);
			else
				this->Buffer[i][j] = Block(WHITE, BLANK, j, i);
		}
	}
}

Map::Map() {
	this->ActiveShape = NULL; 
	this->Buffer = NULL;
	this->MapX = -1;
	this->MapY = -1;
}