#include "Main.h"

static CConsoleLogger Console;
static bool Try_Render = false;
static int RotationCnt = 0;

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
	Log("sisisisi", "\ntype :", block.blocktype, "\ncolor :", block.Color, "\nx :", block.x, "\ny :", block.y);
	Log("sisi", "Weight->x :", block.Weight->x, "\nWeight->y :", block.Weight->y);
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


void thrd_InputDirectionFromUser(Map& Back_Buffer, Map& Front_Buffer, std::mutex& m)
{
	while (true)
	{
		if (Back_Buffer.ActiveShape->y < Back_Buffer.MapY)
		{
			int key = getch();
			if (key == 224)
			{
				key = getch();
				m.lock();
				if (key == 77)			// RIGHT
				{
					Back_Buffer.ActiveShape->x++;
					if(CheckCrash(Back_Buffer, Back_Buffer.ActiveShape->x, Back_Buffer.ActiveShape->y, Back_Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK)
						Back_Buffer.ActiveShape->x--;
				}
				else if (key == 75)	// LEFT
				{
					Back_Buffer.ActiveShape->x--;
					if (CheckCrash(Back_Buffer, Back_Buffer.ActiveShape->x, Back_Buffer.ActiveShape->y, Back_Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK)
						Back_Buffer.ActiveShape->x++;
				}
				else if (key == 80)	// DOWN
				{

					Back_Buffer.ActiveShape->y++;
					if (CheckCrash(Back_Buffer, Back_Buffer.ActiveShape->x, Back_Buffer.ActiveShape->y, Back_Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK)
						Back_Buffer.ActiveShape->y--;
				}
				else if (key == 72)	// UP
				{

					Back_Buffer.ActiveShape->y--;
					if (CheckCrash(Back_Buffer, Back_Buffer.ActiveShape->x, Back_Buffer.ActiveShape->y, Back_Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK)
						Back_Buffer.ActiveShape->y++;
				}
				m.unlock();
				Try_Render = true;
			}
		}
	}
}


FuncReturnType CheckCrash(Map& Buffer, int ShapeX, int ShapeY, Block* block)
{
	if (Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::GROUND ||
		Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::WALL ||

		Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::GROUND ||
		Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::WALL ||

		Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::GROUND ||
		Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::WALL ||

		Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::GROUND||
		Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::WALL)
	{
		return FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK;
	}
}


void SetWeight(Block** block, int RotationIndex, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	block[RotationIndex][0].Weight->x = x1;
	block[RotationIndex][0].Weight->y = y1;
	block[RotationIndex][1].Weight->x = x2;
	block[RotationIndex][1].Weight->y = y2;
	block[RotationIndex][2].Weight->x = x3;
	block[RotationIndex][2].Weight->y = y3;
	block[RotationIndex][3].Weight->x = x4;
	block[RotationIndex][3].Weight->y = y4;
}


Shape* CreateShape() {
	Block** block = new Block * [4];
	for (int k = 0; k < 4; k++)
		block[k] = new Block[4];
	ShapeType type;
	int BlockCnt = 4;
	int r = rand() % 7;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		{
			switch (r)
			{
			case 0:      //Line
				//Log("s", "Line");
				block[i][j].Color = LIGHTBLUE;
				type = ShapeType::eLine;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 0, 2, 0, 3);
					SetWeight(block, 1, 0, 0, 1, 0, 2, 0, 3, 0);
				}
				break;
			case 1:      //RStick
				//Log("s", "RStick");
				block[i][j].Color = LIGHTRED;
				type = ShapeType::eRStick;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 1, 2, 1);
					SetWeight(block, 1, 0, 0, 1, 0, 0, 1, 0, 2);
					SetWeight(block, 2, 0, 0, 1, 0, 2, 0, 2, 1);
					SetWeight(block, 3, 0, 0, 0, 1, 0, 1, -1, 2);
				}
				break;
			case 2:      //LStick
				//Log("s", "LStick");
				block[i][j].Color = LIGHTGREEN;
				type = ShapeType::eLStick;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, -1, 1, -2, 1);
					SetWeight(block, 1, 0, 0, 0, 1, 0, 2, 1, 3);
					SetWeight(block, 2, 0, 0, 0, 1, 1, 0, 2, 0);
					SetWeight(block, 3, 0, 0, -1, 0, 0, 1, 0, 2);
				}
				break;
			case 3:      //RTwo
				//Log("s", "RTwo");
				block[i][j].Color = LIGHTCYAN;
				type = ShapeType::eRTwo;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, -1, 1, -2, 1);
					SetWeight(block, 1, 0, 0, 1, 0, 0, 1, -1, 1);
				}
				break;
			case 4:      //LTwo
				//Log("s", "LTwo");
				block[i][j].Color = LIGHTMAGENTA;
				type = ShapeType::eLTwo;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 1, 2, 1);
					SetWeight(block, 1, 0, 0, -1, 0, 0, 1, 1, 1);
				}
				break;
			case 5:      //Middle
				//Log("s", "Middle");
				block[i][j].Color = LIGHTGRAY;
				type = ShapeType::eMiddle;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, -1, -1, 0, 1, 0);
					SetWeight(block, 1, 0, 0, 1, 0, 0, -1, 0, 1);
					SetWeight(block, 2, 0, 0, 0, 1, -1, 0, 1, 0);
					SetWeight(block, 3, 0, 0, -1, 0, -1, 0, 1, 0);
				}
				break;
			case 6:      //Rect
				//Log("s", "Rect");
				block[i][j].Color = BROWN;
				type = ShapeType::eRect;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 1, 0, 1, 1, 0, 1);
				}
				break;
			}
			block[i][j].blocktype = BlockType::BLOCK;
		}
	}
	Shape* shape = new Shape(block, type);
	return shape;
}


FuncReturnType MoveShape(Map& Buffer) {
	Block** block = Buffer.ActiveShape->ElementBlocks;
	Shape* shape = Buffer.ActiveShape;
	Block blank(WHITE, BlockType::BLANK, -1, -1);
	if (Buffer.ActiveShape->x == 0) {
		Buffer.ActiveShape->y = 1;
		Buffer.ActiveShape->x = 6;
	}
	else
	{
		for (int y = 0; y < Buffer.MapY; y++)
			for (int x = 0; x < Buffer.MapX; x++)
				if (Buffer[y][x].blocktype == BlockType::BLOCK)
					Buffer[y][x] = blank;
	}

	if (CheckCrash(Buffer, shape->x, shape->y, block[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK)
	{
		shape->y--;
		(Buffer[shape->y][shape->x] = block[RotationCnt][0]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[RotationCnt][1].Weight->y][shape->x + block[RotationCnt][1].Weight->x] = block[RotationCnt][1]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[RotationCnt][2].Weight->y][shape->x + block[RotationCnt][2].Weight->x] = block[RotationCnt][2]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[RotationCnt][3].Weight->y][shape->x + block[RotationCnt][3].Weight->x] = block[RotationCnt][3]).blocktype = BlockType::INACTBLOCK;
		return FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK;
	}

	//Buffer[y][x]
	Buffer[shape->y][shape->x] = block[RotationCnt][0];
	Buffer[shape->y + block[RotationCnt][1].Weight->y][shape->x + block[RotationCnt][1].Weight->x] = block[RotationCnt][1];
	Buffer[shape->y + block[RotationCnt][2].Weight->y][shape->x + block[RotationCnt][2].Weight->x] = block[RotationCnt][2];
	Buffer[shape->y + block[RotationCnt][3].Weight->y][shape->x + block[RotationCnt][3].Weight->x] = block[RotationCnt][3];
	shape->y++;

	return FuncReturnType::CONTINUE;
}


FuncReturnType Start(Map& Buffer) {
	std::mutex m;
	Console.Create("Log");
	Map Front_Buffer;
	Map Back_Buffer;

	Shape* Shapeinstance = CreateShape();
	Back_Buffer.SetActiveShape(Shapeinstance);
	FuncReturnType result = FuncReturnType::eNULL;

	Back_Buffer = Buffer;
	std::thread Thrd_UserInput(
		thrd_InputDirectionFromUser, std::ref(Back_Buffer), std::ref(Front_Buffer), std::ref(m)
	);
	Thrd_UserInput.detach();

	while (true)
	{
		if (result == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK) {
			Back_Buffer.AddInActiveShape(Shapeinstance);
			Shapeinstance = CreateShape();
		}
		Back_Buffer.SetActiveShape(Shapeinstance);
		result = MoveShape(Back_Buffer);
		CheckBufferAndRender(Back_Buffer, Front_Buffer, Back_Buffer.MapX, Back_Buffer.MapY);
		for (int i = 0; i < 100; i++)
			if (Try_Render) {
				Try_Render = false;
				break;
			}
			else
				Sleep(5);
	}
	return FuncReturnType::BLOCKED_BY_ENDLINE;
}


void CheckBufferAndRender(Map& Back_Buffer, Map& Front_Buffer, int MapX, int MapY) {
	for (int i = 0; i < MapY; i++)
	{
		for (int j = 0; j < MapX; j++)
		{
			if (j % MapX == 0) std::cout << std::endl;
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
				if (((i + 1) * (j + 1)) == MapX * MapY)
					Front_Buffer = Back_Buffer;
				Renderer(Back_Buffer.Buffer[i][j], Back_Buffer.MapX, Back_Buffer.MapY);
			}
		}
	}
	Try_Render = FALSE;
	gotoxy(0, 0);
}


void Renderer(Block Buffer, int x, int y) {
	switch (Buffer.blocktype)
	{
	case BlockType::BLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		break;
	}
	case BlockType::BLANK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "д¤";
		break;
	}
	case BlockType::WALL: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		break;
	}
	case BlockType::GROUND: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		break;
	}
	case BlockType::INACTBLOCK: {
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
				if (j == 0 && i == y - 1 || i == y - 1)
					this->Buffer[i][j] = Block(WHITE, BlockType::GROUND, j, i);
				else
					this->Buffer[i][j] = Block(WHITE, BlockType::WALL, j, i);
			else
				this->Buffer[i][j] = Block(WHITE, BlockType::BLANK, j, i);

		}
	}
}

Map::Map() {
	this->ActiveShape = NULL;
	this->Buffer = NULL;
	this->MapX = -1;
	this->MapY = -1;
}