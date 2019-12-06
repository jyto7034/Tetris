#include "Main.h"

static CConsoleLogger Console;
static int RotationCnt = 0;
static int MaxRotationCnt = 0;
static int ThrdMaxDownCnt = 2;
static bool forceRender = false;
std::vector<RenderType> RendererQue;
CRITICAL_SECTION g_cs;
Block blank(WHITE, BlockType::BLANK, -1, -1);

#pragma region Tools

void ConsoleInitialize() {
	Console.Create("Log");
	srand(time(NULL));
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}


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


void ShowBlockType(Block block) {
	switch (block.blocktype)
	{
	case BlockType::INACTBLOCK:
		Log("s", "BlockType::INACTBLOCK");
		break;
	case BlockType::GROUND:
		Log("s", "BlockType::GROUND");
		break;
	case BlockType::BLOCK:
		Log("s", "BlockType::BLOCK");
		break;
	case BlockType::BLANK:
		Log("s", "BlockType::BLANK");
		break;
	case BlockType::_NULL:
		Log("s", "BlockType::NULL");
		break;
	case BlockType::WALL:
		Log("s", "BlockType::WALL");
		break;
	}
}


void ShowBlockData(Block block) {
	ShowBlockType(block);
	Log("sisisi", "\ncolor :", block.Color, "\nx :", block.x, "\ny :", block.y);
	Log("sisis", "Weight->x :", block.Weight->x, "\nWeight->y :", block.Weight->y, "\n\n");
}
#pragma endregion

FuncReturnType CheckCrash(Map& Buffer, Block* block)
{
	int ShapeX = Buffer.ActiveShape->x, ShapeY = Buffer.ActiveShape->y;
	int y1 = ShapeY + block[0].Weight->y;
	int x1 = ShapeX + block[0].Weight->x;
	int y2 = ShapeY + block[1].Weight->y;
	int x2 = ShapeX + block[1].Weight->x;
	int y3 = ShapeY + block[2].Weight->y;
	int x3 = ShapeX + block[2].Weight->x;
	int y4 = ShapeY + block[3].Weight->y;
	int x4 = ShapeX + block[3].Weight->x;
	if (x1 + y1 <= 0 || x2 + y2 <= 0 || x3 + y3 <= 0 || x4 + y4 <= 0)
		return FuncReturnType::CONTINUE;

	if ((Buffer[y1][x1].blocktype == BlockType::INACTBLOCK ||
		Buffer[y1][x1].blocktype == BlockType::GROUND ||
		Buffer[y1][x1].blocktype == BlockType::WALL) ||

		(Buffer[y2][x2].blocktype == BlockType::INACTBLOCK ||
			Buffer[y2][x2].blocktype == BlockType::GROUND ||
			Buffer[y2][x2].blocktype == BlockType::WALL) ||

			(Buffer[y3][x3].blocktype == BlockType::INACTBLOCK ||
				Buffer[y3][x3].blocktype == BlockType::GROUND ||
				Buffer[y3][x3].blocktype == BlockType::WALL) ||

				(Buffer[y4][x4].blocktype == BlockType::INACTBLOCK ||
					Buffer[y4][x4].blocktype == BlockType::GROUND ||
					Buffer[y4][x4].blocktype == BlockType::WALL))
	{
		return  FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL;
	}
	return FuncReturnType::CONTINUE;
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
			{
				//Log("s", "Line");
				block[i][j].Color = LIGHTBLUE;
				type = ShapeType::eLine;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 0, 2, 0, 3);
					SetWeight(block, 1, 0, 0, -1, 0, 1, 0, 2, 0);
					MaxRotationCnt = 2;
				}
				break;
			}
			case 1:      //RStick
			{
				//Log("s", "RStick");
				block[i][j].Color = LIGHTRED;
				type = ShapeType::eRStick;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, -1, -1, 0, -2, 0);
					SetWeight(block, 1, 0, -1, 1, -1, 0, -2, 0, -3);
					SetWeight(block, 2, 0, -1, -1, -1, 1, -1, -1, 0);
					SetWeight(block, 3, 0, 0, 0, -1, 0, 1, -1, -1);
					MaxRotationCnt = 4;
				}
				break;
			}
			case 2:      //LStick
			   //Log("s", "LStick");
				block[i][j].Color = LIGHTGREEN;
				type = ShapeType::eLStick;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 1, 2, 1);
					SetWeight(block, 1, 0, 0, 0, -1, 0, -2, 1, -2);
					SetWeight(block, 2, 0, 0, 1, 0, 2, 0, 2, 1);
					SetWeight(block, 3, 0, 0, -1, 0, 0, -1, 0, -2);
					MaxRotationCnt = 4;
				}
				break;
			case 3:      //RTwo
			   //Log("s", "RTwo");
				block[i][j].Color = LIGHTCYAN;
				type = ShapeType::eRTwo;
				if (i == 0) {
					SetWeight(block, 0, -1, 0, -1, -1, 0, 0, 0, 1);
					SetWeight(block, 1, 0, 0, 1, 0, 0, 1, -1, 1);
					MaxRotationCnt = 2;
				}
				break;
			case 4:      //LTwo
			   //Log("s", "LTwo");
				block[i][j].Color = LIGHTMAGENTA;
				type = ShapeType::eLTwo;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 0, 1, -1);
					SetWeight(block, 1, 1, 0, 1, -1, 0, -1, 2, 0);
					MaxRotationCnt = 2;
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
					SetWeight(block, 3, 0, 0, 0, -1, 0, 1, -1, 0);
					MaxRotationCnt = 4;
				}
				break;
			case 6:      //Rect
			   //Log("s", "Rect");
				block[i][j].Color = BROWN;
				type = ShapeType::eRect;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 1, 0, 1, 1, 0, 1);
					MaxRotationCnt = 1;
				}
				break;
			}
			block[i][j].blocktype = BlockType::BLOCK;
		}
	}
	Shape* shape = new Shape(block, type);
	return shape;
}


FuncReturnType MoveShape(Map& Buffer, bool DisplayUpdate = FALSE) {
	EnterCriticalSection(&g_cs);
	Block block1 = Buffer.ActiveShape->ElementBlocks[RotationCnt][0];
	Block block2 = Buffer.ActiveShape->ElementBlocks[RotationCnt][1];
	Block block3 = Buffer.ActiveShape->ElementBlocks[RotationCnt][2];
	Block block4 = Buffer.ActiveShape->ElementBlocks[RotationCnt][3];
	Shape* shape = Buffer.ActiveShape;

	if (Buffer.ActiveShape->x == 0)
	{
		Buffer.ActiveShape->y = 2;
		Buffer.ActiveShape->x = Buffer.MapX / 2;
	}
	else
	{
		for (int y = 0; y < Buffer.MapY; y++)
			for (int x = 0; x < Buffer.MapX; x++)
				if (Buffer[y][x].blocktype == BlockType::BLOCK)
					Buffer[y][x] = blank;
	}

	int y1 = shape->y + block1.Weight->y;
	int x1 = shape->x + block1.Weight->x;
	int y2 = shape->y + block2.Weight->y;
	int x2 = shape->x + block2.Weight->x;
	int y3 = shape->y + block3.Weight->y;
	int x3 = shape->x + block3.Weight->x;
	int y4 = shape->y + block4.Weight->y;
	int x4 = shape->x + block4.Weight->x;

	if (CheckCrash(Buffer, Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
	{
		shape->y--;
		(Buffer[y1 - 1][x1] = block1).blocktype = BlockType::INACTBLOCK;
		(Buffer[y2 - 1][x2] = block2).blocktype = BlockType::INACTBLOCK;
		(Buffer[y3 - 1][x3] = block3).blocktype = BlockType::INACTBLOCK;
		(Buffer[y4 - 1][x4] = block4).blocktype = BlockType::INACTBLOCK;
		block1.blocktype = BlockType::INACTBLOCK;
		block2.blocktype = BlockType::INACTBLOCK;
		block3.blocktype = BlockType::INACTBLOCK;
		block4.blocktype = BlockType::INACTBLOCK;
		RotationCnt = 0;
		LeaveCriticalSection(&g_cs);
		return  FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL;
	}

	Buffer[y1][x1] = block1;
	Buffer[y2][x2] = block2;
	Buffer[y3][x3] = block3;
	Buffer[y4][x4] = block4;
	if (DisplayUpdate == FALSE)
		shape->y++;

	LeaveCriticalSection(&g_cs);
	return FuncReturnType::CONTINUE;
}


void CheckUserInput(Map& Buffer, bool& UserInput)
{
	if (kbhit())
	{
		if (Buffer.ActiveShape->ElementBlocks[RotationCnt]->blocktype != BlockType::INACTBLOCK)
		{
			int keycode = getch();
			if (keycode == 224)
			{
				keycode = getch();
				if (keycode == 77)         // RIGHT
				{
					Buffer.ActiveShape->x++;
					if (CheckCrash(Buffer, Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						Buffer.ActiveShape->x--;
					UserInput = TRUE;
				}
				else if (keycode == 75)   // LEFT
				{
					Buffer.ActiveShape->x--;
					if (CheckCrash(Buffer, Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						Buffer.ActiveShape->x++;
					UserInput = TRUE;
				}
				else if (keycode == 80)   // DOWN
				{
					if (CheckCrash(Buffer, Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						Buffer.ActiveShape->y--;
					else
						Buffer.ActiveShape->y++;
					UserInput = TRUE;
				}
				else if (keycode == 72)   // UP
				{
					if (CheckCrash(Buffer, Buffer.ActiveShape->ElementBlocks[RotationCnt + 1 < MaxRotationCnt ? RotationCnt + 1 : 0]) != FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						if (RotationCnt + 1 < MaxRotationCnt)
							RotationCnt++;
						else
							RotationCnt = 0;
					UserInput = TRUE;
				}
				else if (keycode == 32)   // LEFT
				{
					UserInput = TRUE;
				}
			}
		}
	}
}


void Thrd_Render(Map& BackBuffer, Map& FrontBuffer, FuncReturnType& result)
{
	while (true)
	{
		if (BackBuffer.ActiveShape->ElementBlocks[RotationCnt]->blocktype != BlockType::INACTBLOCK)
		{
			Sleep(1000);
			result = MoveShape(BackBuffer);
			CheckBingoAndHoldDown(BackBuffer);
			CheckBufferAndRender(BackBuffer, FrontBuffer);
		}
	}
}

void DeleteLine(Map& Buffer, int line)
{
	int My = Buffer.MapY, Mx = Buffer.MapX;
	for (int i = 0; i < Buffer.MapX; i++)
		if(Buffer[line][i].blocktype == BlockType::INACTBLOCK)
			Buffer[line][i] = blank;
	for (int y = 0; y < My; y++)
	{
		for (int x = 0; x < Mx; x++)
		{
			if (Buffer[My - y - 1][x].blocktype == BlockType::INACTBLOCK)
			{
				Buffer[My - y][x] = Buffer[My - y - 1][x];
				Buffer[My - y - 1][x] = blank;
			}
		}
	}
}

void CheckBingoAndHoldDown(Map& Buffer) {
	EnterCriticalSection(&g_cs);
	int My = Buffer.MapY, Mx = Buffer.MapX;
	for (int y = 0; y < My; y++)
	{
		for (int x = 0; x < Mx; x++)
		{
			if (!(x == 0 || x == Mx - 1 || y == 0 || y == My - 1))
			{
				if (Buffer[y][x].blocktype == BlockType::INACTBLOCK)
				{
					if (x == Mx - 2)
						DeleteLine(Buffer, y);
				}
				else
					break;
			}
		}
	}
	LeaveCriticalSection(&g_cs);
}


FuncReturnType Start(Map& Buffer) {
	InitializeCriticalSection(&g_cs);
	ConsoleInitialize();
	std::mutex m;
	Map FrontBuffer;
	Map BackBuffer;

	bool UserInput = FALSE;

	Shape* Shapeinstance = CreateShape();
	BackBuffer.SetActiveShape(Shapeinstance);
	FuncReturnType result = FuncReturnType::eNULL;
	RenderType render = RenderType::CALL_BY_MAIN;

	BackBuffer = Buffer;
	std::thread thrd_Render(
		Thrd_Render, std::ref(BackBuffer), std::ref(FrontBuffer), std::ref(result)
	);
	thrd_Render.detach();

	while (true)
	{
		CheckUserInput(BackBuffer, UserInput);
		if (UserInput)
		{
			result = MoveShape(BackBuffer, UserInput);
			CheckBingoAndHoldDown(BackBuffer);
			CheckBufferAndRender(BackBuffer, FrontBuffer);
			UserInput = FALSE;
		}
		if (result == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL) {
			BackBuffer.AddInActiveShape(Shapeinstance);
			Shapeinstance = CreateShape();
		}
		BackBuffer.SetActiveShape(Shapeinstance);
		Sleep(50);
	}
	DeleteCriticalSection(&g_cs);
	return FuncReturnType::BLOCKED_BY_ENDLINE;
}


void CheckBufferAndRender(Map& Back_Buffer, Map& Front_Buffer) {
	EnterCriticalSection(&g_cs);
	gotoxy(0, 0);
	int MapX = Back_Buffer.MapX, MapY = Back_Buffer.MapY; 
	for (int i = 0; i < MapY; i++)
	{
		for (int j = 0; j < MapX; j++)
		{
			if (j % MapX == 0) std::cout << std::endl;
			if (Front_Buffer.Buffer != NULL)
			{
				Renderer(Back_Buffer.Buffer[i][j]);
				Front_Buffer[i][j] = Back_Buffer[i][j];
			}
			else
			{
				if (((i + 1) * (j + 1)) == MapX * MapY)
					Front_Buffer = Back_Buffer;
				Renderer(Back_Buffer[i][j]);
			}
		}
	}
	LeaveCriticalSection(&g_cs);
}


void Renderer(Block& Buffer) {
	int x = Buffer.x, y = Buffer.y;
	switch (Buffer.blocktype)
	{
	case BlockType::BLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "��";
		break;
	}
	case BlockType::BLANK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "��";
		break;
	}
	case BlockType::WALL: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "��";
		break;
	}
	case BlockType::GROUND: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "��";
		break;
	}
	case BlockType::INACTBLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "��";
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