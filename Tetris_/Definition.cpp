#include "Main.h"

static CConsoleLogger Console;
static bool isRendering = false;
static int RotationCnt = 0;
static int MaxRotationCnt = 0;
static int ThrdMaxDownCnt = 2;

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


void thrd_InputDirectionFromUser(Map& BackBuffer, Map& Front_Buffer, int& RotationMax, std::mutex& m)
{
	int Cnt = 0;
	while (true)
	{
		if (BackBuffer.ActiveShape->y < BackBuffer.MapY - 1)
		{
			int key = getch();
			if (key == 224)
			{
				key = getch();
				m.lock();
				if (key == 77)         // RIGHT
				{
					BackBuffer.ActiveShape->x++;
					if (CheckCrash(BackBuffer, BackBuffer.ActiveShape->x, BackBuffer.ActiveShape->y, BackBuffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						BackBuffer.ActiveShape->x--;
				}
				else if (key == 75)   // LEFT
				{
					BackBuffer.ActiveShape->x--;
					if (CheckCrash(BackBuffer, BackBuffer.ActiveShape->x, BackBuffer.ActiveShape->y, BackBuffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						BackBuffer.ActiveShape->x++;
				}
				else if (key == 80)   // DOWN
				{
					if (Cnt < ThrdMaxDownCnt) {
						if (CheckCrash(BackBuffer, BackBuffer.ActiveShape->x, BackBuffer.ActiveShape->y, BackBuffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						{
							BackBuffer.ActiveShape->y--;
							Cnt++;
						}
						else
						{
							BackBuffer.ActiveShape->y++;
							Cnt = 0;
						}
					}
				}
				else if (key == 72)   // UP
				{
					if (CheckCrash(BackBuffer, BackBuffer.ActiveShape->x, BackBuffer.ActiveShape->y, BackBuffer.ActiveShape->ElementBlocks[RotationCnt + 1 < RotationMax ? RotationCnt + 1 : 0]) != FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
						if (RotationCnt + 1 < RotationMax)
							RotationCnt++;
						else
							RotationCnt = 0;
				}
				ThrdTryRender:
				if (isRendering == FALSE)
				{
					isRendering = TRUE;
					gotoxy(0, 0);
					MoveShape(BackBuffer, TRUE);
					for (int i = 0; i < BackBuffer.MapY; i++)
					{
						for (int j = 0; j < BackBuffer.MapX; j++)
						{
							if (j % BackBuffer.MapX == 0) std::cout << std::endl;
							Renderer(BackBuffer.Buffer[i][j]);
						}
					}
					isRendering = FALSE;
				}
				else
					goto ThrdTryRender;
				m.unlock();
			}
		}
	}
}


FuncReturnType CheckCrash(Map& Buffer, int ShapeX, int ShapeY, Block* block)
{
	Log("s", "##########Check Start###########");
	//Log("s", "Block 1 :");
	//ShowBlockData(block[0]);
	//Log("s", "\n\nBlock 2 :");
	//ShowBlockData(block[1]);
	//Log("s", "\n\nBlock 3 :");
	//ShowBlockData(block[2]);
	//Log("s", "\n\nBlock 4 :");
	//ShowBlockData(block[3]);

	//Log("sisi", "ShapeX :", ShapeX, "   ShapeY :", ShapeY);

	//Log("ii", ShapeY + block[0].Weight->y, ShapeX + block[0].Weight->x);
	//Log("ii", ShapeY + block[1].Weight->y, ShapeX + block[1].Weight->x);
	//Log("ii", ShapeY + block[2].Weight->y, ShapeX + block[2].Weight->x);
	//Log("ii", ShapeY + block[3].Weight->y, ShapeX + block[3].Weight->x);


	//Log("s", "\n\nBuffer 1 :");
	//ShowBlockData(Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x]);
	//Log("s", "\n\nBuffer 2 :");
	//ShowBlockData(Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x]);
	//Log("s", "\n\nBuffer 3 :");
	//ShowBlockData(Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x]);
	//Log("s", "\n\nBuffer 4 :");
	//ShowBlockData(Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x]);

	if ((Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::INACTBLOCK ||
		Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::GROUND ||
		Buffer[ShapeY + block[0].Weight->y][ShapeX + block[0].Weight->x].blocktype == BlockType::WALL) ||

		(Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::INACTBLOCK ||
			Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::GROUND ||
			Buffer[ShapeY + block[1].Weight->y][ShapeX + block[1].Weight->x].blocktype == BlockType::WALL) ||

			(Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::INACTBLOCK ||
				Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::GROUND ||
				Buffer[ShapeY + block[2].Weight->y][ShapeX + block[2].Weight->x].blocktype == BlockType::WALL) ||

				(Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::INACTBLOCK ||
					Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::GROUND ||
					Buffer[ShapeY + block[3].Weight->y][ShapeX + block[3].Weight->x].blocktype == BlockType::WALL))
	{
		Log("s", "##########CheckEnd###########");
		return  FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL;
	}
	Log("s", "##########CheckEnd###########");
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
			switch (0)
			{
			case 0:      //Line
			   //Log("s", "Line");
				block[i][j].Color = LIGHTBLUE;
				type = ShapeType::eLine;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 0, 2, 0, 3);
					SetWeight(block, 1, 0, 0, 1, 0, 2, 0, 3, 0);
					MaxRotationCnt = 2;
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
					SetWeight(block, 3, 0, 0, 0, 1, 0, 2, -1, 2);
					MaxRotationCnt = 4;
				}
				break;
			case 2:      //LStick
			   //Log("s", "LStick");
				block[i][j].Color = LIGHTGREEN;
				type = ShapeType::eLStick;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, -1, 1, -2, 1);
					SetWeight(block, 1, 0, 0, -1, 0, -1, -1, -1, -2);
					SetWeight(block, 2, 0, 0, 0, -1, 1, -1, 2, -1);
					SetWeight(block, 3, 0, 0, 1, 0, 1, 1, 1, 2);
					MaxRotationCnt = 4;
				}
				break;
			case 3:      //RTwo
			   //Log("s", "RTwo");
				block[i][j].Color = LIGHTCYAN;
				type = ShapeType::eRTwo;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 1, 1, 2);
					SetWeight(block, 1, 0, 0, 1, 0, 0, 1, -1, 1);
					MaxRotationCnt = 2;
				}
				break;
			case 4:      //LTwo
			   //Log("s", "LTwo");
				block[i][j].Color = LIGHTMAGENTA;
				type = ShapeType::eLTwo;
				if (i == 0) {
					SetWeight(block, 0, 0, 0, 0, 1, 1, 1, 1, 2);
					SetWeight(block, 1, 0, 0, -1, 0, 0, 1, 1, 1);
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
	Block block0 = Buffer.ActiveShape->ElementBlocks[RotationCnt][0];
	Block block1 = Buffer.ActiveShape->ElementBlocks[RotationCnt][1];
	Block block2 = Buffer.ActiveShape->ElementBlocks[RotationCnt][2];
	Block block3 = Buffer.ActiveShape->ElementBlocks[RotationCnt][3];
	Shape* shape = Buffer.ActiveShape;
	Block blank(WHITE, BlockType::BLANK, -1, -1);
	if (Buffer.ActiveShape->x == 0) {
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

	if (CheckCrash(Buffer, shape->x, shape->y, Buffer.ActiveShape->ElementBlocks[RotationCnt]) == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL)
	{
		shape->y--;
		(Buffer[shape->y][shape->x] = block0).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block1.Weight->y][shape->x + block1.Weight->x] = block1).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block2.Weight->y][shape->x + block2.Weight->x] = block2).blocktype = BlockType::INACTBLOCK;
		(Buffer[shape->y + block3.Weight->y][shape->x + block3.Weight->x] = block3).blocktype = BlockType::INACTBLOCK;
		RotationCnt = 0;
		return  FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL;
	}

	Buffer[shape->y][shape->x] = block0;
	Buffer[shape->y + block1.Weight->y][shape->x + block1.Weight->x] = block1;
	Buffer[shape->y + block2.Weight->y][shape->x + block2.Weight->x] = block2;
	Buffer[shape->y + block3.Weight->y][shape->x + block3.Weight->x] = block3;
	if(DisplayUpdate == FALSE)
		shape->y++;

	return FuncReturnType::CONTINUE;
}


FuncReturnType Start(Map& Buffer) {
	srand(time(NULL));
	std::mutex m;
	Console.Create("Log");
	Map FrontBuffer;
	Map BackBuffer;

	Shape* Shapeinstance = CreateShape();
	BackBuffer.SetActiveShape(Shapeinstance);
	FuncReturnType result = FuncReturnType::eNULL;

	BackBuffer = Buffer;
	std::thread Thrd_UserInput(
		thrd_InputDirectionFromUser, std::ref(BackBuffer), std::ref(FrontBuffer), std::ref(MaxRotationCnt), std::ref(m)
	);
	Thrd_UserInput.detach();

	while (true)
	{
		Log("s", "########Main#########");

		Log("s", "Entry Check");
		if (result == FuncReturnType::BLOCKED_BY_GROUND_OR_INACTIVEBLOCK_OR_WALL) {
			BackBuffer.AddInActiveShape(Shapeinstance);
			Log("s", "Entry CreateShape");
			Shapeinstance = CreateShape();
			Log("s", "End   CreateShape");
		}
		Log("s", "End   Check");
		BackBuffer.SetActiveShape(Shapeinstance);
		Log("s", "Entry MoveShape");
		TryRender:
		if (isRendering == FALSE)
		{
			isRendering = TRUE;
			result = MoveShape(BackBuffer);
			Log("s", "End   MoveShape");
			Log("s", "Entry CheckBuff");
			CheckBufferAndRender(BackBuffer, FrontBuffer);
			Log("s", "End   CheckBuff");
		}
		else
			goto TryRender;

		Sleep(1000);
		//for (int i = 0; i < 100; i++)
		//   if (Try_Render) {
		//      Try_Render = false;
		//      break;
		//   }
		//   else
		//      Sleep(5);
		Log("s", "########Main End#########");
	}
	return FuncReturnType::BLOCKED_BY_ENDLINE;
}


void CheckBufferAndRender(Map& Back_Buffer, Map& Front_Buffer) {
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
	isRendering = FALSE;
}


void Renderer(Block& Buffer) {
	int x = Buffer.x, y = Buffer.y;
	switch (Buffer.blocktype)
	{
	case BlockType::BLOCK: {
		textcolor(Buffer.Color, BLACK);
		std::cout << "бс";
		//std::cout.width(4);
		//std::cout << Buffer.y;
		break;
	}
	case BlockType::BLANK: {
		textcolor(Buffer.Color, BLACK);
		//std::cout.width(4);
		//std::cout << Buffer.y;
		std::cout << "д¤";
		break;
	}
	case BlockType::WALL: {
		textcolor(Buffer.Color, BLACK);
		//std::cout.width(4);
		//std::cout << Buffer.y;
		std::cout << "бс";
		break;
	}
	case BlockType::GROUND: {
		textcolor(Buffer.Color, BLACK);
		//std::cout.width(4);
		//std::cout << Buffer.y;
		std::cout << "бс";
		break;
	}
	case BlockType::INACTBLOCK: {
		textcolor(Buffer.Color, BLACK);
		//std::cout.width(4);
		//std::cout << Buffer.y;
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