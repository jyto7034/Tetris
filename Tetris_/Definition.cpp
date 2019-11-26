#include "Main.h"

static CConsoleLogger Console;
static bool Try_Render = false;
static bool StopMove = false;

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
		if (Back_Buffer.ActiveShape->y < 20) 
		{
			int key = getch();
			if (key == 224) {
				key = getch();
				m.lock();
				if (key == 77 && StopMove == false)
					Back_Buffer.ActiveShape->x++;
				if (key == 75 && StopMove == false)
					Back_Buffer.ActiveShape->x--;
				if (key == 80 && StopMove == false)
					Back_Buffer.ActiveShape->y++;
				if (key == 72 && StopMove == false)
					Back_Buffer.ActiveShape->y--;
				m.unlock();
				Try_Render = true;
			}
		}
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
		case 0:      //Line
			//Log("s", "Line");
			block[i].Color = LIGHTBLUE;
			type = ShapeType::eLine;
			if (i == 0) {
				block[0].Weight->x = 0;
				block[0].Weight->y = 0;

				block[1].Weight->x = 0;
				block[1].Weight->y = 1;

				block[2].Weight->x = 0;
				block[2].Weight->y = 2;

				block[3].Weight->x = 0;
				block[3].Weight->y = 3;
			}
			break;
		case 1:      //RStick
			//Log("s", "RStick");
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
		case 2:      //LStick
			//Log("s", "LStick");
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
		case 3:      //RTwo
			//Log("s", "RTwo");
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
		case 4:      //LTwo
			//Log("s", "LTwo");
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
		case 5:      //Middle
			//Log("s", "Middle");
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
		case 6:      //Rect
			//Log("s", "Rect");
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
		block[i].blocktype = BlockType::BLOCK;
	}
	Shape* shape = new Shape(block, type);
	return shape;
}


FuncReturnType MoveShape(Map& Buffer) {
	Block* block = Buffer.ActiveShape->ElementBlocks;
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
				if(Buffer[y][x].blocktype == BlockType::BLOCK)
					Buffer[y][x] = blank;
	}


	if (Buffer[shape->y + block[0].Weight->y][shape->x + block[0].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[shape->y + block[0].Weight->y][shape->x + block[0].Weight->x].blocktype == BlockType::GROUND ||

		Buffer[shape->y + block[1].Weight->y][shape->x + block[1].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[shape->y + block[1].Weight->y][shape->x + block[1].Weight->x].blocktype == BlockType::GROUND ||

		Buffer[shape->y + block[2].Weight->y][shape->x + block[2].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[shape->y + block[2].Weight->y][shape->x + block[2].Weight->x].blocktype == BlockType::GROUND ||

		Buffer[shape->y + block[3].Weight->y][shape->x + block[3].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[shape->y + block[3].Weight->y][shape->x + block[3].Weight->x].blocktype == BlockType::GROUND)
	{
		shape->y--;
		StopMove = true;
		(Buffer[shape->y][shape->x] = block[0]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[1].Weight->y][shape->x + block[1].Weight->x] = block[1]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[2].Weight->y][shape->x + block[2].Weight->x] = block[2]).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block[3].Weight->y][shape->x + block[3].Weight->x] = block[3]).blocktype = BlockType::INACTBLOCK;
		return FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK;
	}

	//Buffer[y][x]
	Buffer[shape->y][shape->x] = block[0];
	Buffer[shape->y + block[1].Weight->y][shape->x + block[1].Weight->x] = block[1];
	Buffer[shape->y + block[2].Weight->y][shape->x + block[2].Weight->x] = block[2];
	Buffer[shape->y + block[3].Weight->y][shape->x + block[3].Weight->x] = block[3];
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



