#pragma once
#include <iostream>
#include <windows.h>
#include "ConsoleLogger.h"
#include <thread>
#include <conio.h>
#include <cassert>
#include <vector>

#pragma region Color Definition

#define BLACK 0 
#define BLUE 1 
#define GREEN 2 
#define CYAN 3 
#define RED 4 
#define MAGENTA 5 
#define BROWN 6 
#define LIGHTGRAY 7 
#define DARKGRAY 8 
#define LIGHTBLUE 9 
#define LIGHTGREEN 10 
#define LIGHTCYAN 11 
#define LIGHTRED 12 
#define LIGHTMAGENTA 13 
#define YELLOW 14 
#define WHITE 15 

#pragma endregion

#define INACTBLOCK 0
#define BLOCK 1
#define BLANK 2
#define WALL 3	
#define WEIGHT_NULL -9	

void textcolor(int foreground, int background);
void Log(const char* types, ...);
void gotoxy(int x, int y);
POINT getXY();

enum class BlockedType
{
	BLOCKED_BY_ENDLINE,
	BLOCKED_BY_BLOCK,
	BLOCKED_BY_WALL,
	eNULL
};

enum class BlockDirection {
	eDOWN, eUP, eRIGHT, eLEFT, yFULLDUPLEX, xFULLDUPLEX, eNULL
};

enum class ShapeType {
	eLine, eRStick, eLStick, eRTwo, eLTwo, eMiddle, eRect, eNULL
};

class Block {
private:
	struct st_Weight
	{
		int x, y;
	};
public:
	int Color, x, y;
	int BlockType;
	st_Weight Weight[4];
	Block() : Color(-1), x(-1), y(-1), BlockType(-1){
		Weight[0] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[1] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[2] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[3] = { WEIGHT_NULL, WEIGHT_NULL };
	};
	Block(int _color, int _BlockType, int _x, int _y) :
		Color(_color), BlockType(_BlockType), x(_x), y(_y) {
		Weight[0] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[1] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[2] = { WEIGHT_NULL, WEIGHT_NULL };
		Weight[3] = { WEIGHT_NULL, WEIGHT_NULL };
	};

	bool operator==(const Block& rvalue) {
		return this->BlockType == rvalue.BlockType &&
			this->Color == rvalue.Color &&
			this->x == this->x &&
			this->y == this->y;
	}

	Block& operator=(Block rvalue) {
		this->BlockType = rvalue.BlockType;
		this->Color = rvalue.Color;
		this->Weight->x = rvalue.Weight->x;
		this->Weight->y = rvalue.Weight->y;
			

		if (rvalue.x == -1)
		{
			rvalue.x = this->x;
			rvalue.y = this->y;
		}
		else
		{
			this->x = rvalue.x;
			this->y = rvalue.y;
		}
		return *this;
	}

};

class Shape {
public:
	Block* ElementBlocks;
	ShapeType mShapeType;
	int BlockCnt;
	Shape() : ElementBlocks(NULL), mShapeType(ShapeType::eNULL), BlockCnt(-1) {}
	Shape(Block* Blocks, ShapeType _mShapeType) {
		assert(Blocks != NULL, "Blocks is NULL");
		BlockCnt = 4;
		ElementBlocks = Blocks;
	}

	Block& operator[](int index) {
		return ElementBlocks[index];
	}
};


class Map {
private:
	class Proxy {
	public:
		Block* _block;
		Proxy(Block* pblock) : _block(pblock) {}
		Block& operator[](int index) {
			return _block[index];
		}
	};
public:
	Block** Buffer;
	Shape* ActiveShape;
	std::vector<Shape> InActiveShapes;
	int MapX, MapY;

	Map& operator=(const Map& rvalue) {
		this->MapX = rvalue.MapX;
		this->MapY = rvalue.MapY;
		if (rvalue.Buffer == NULL)
			if (this->Buffer == NULL)
				this->Buffer = NULL;
			else {
				for (int i = 0; i < rvalue.MapY; i++)
					delete[] this->Buffer[i];
				delete[] this->Buffer;
				this->Buffer = NULL;
			}

		if (this->Buffer == NULL) {
			this->Buffer = new Block * [rvalue.MapY];
			for (int i = 0; i < rvalue.MapY; i++)
				this->Buffer[i] = new Block[rvalue.MapX];
			for (int i = 0; i < rvalue.MapY; i++)
				for (int j = 0; j < rvalue.MapX; j++)
					this->Buffer[i][j] = rvalue.Buffer[i][j];
		}
		else
			for (int i = 0; i < rvalue.MapY; i++)
				for (int j = 0; j < rvalue.MapX; j++)
					this->Buffer[i][j] = rvalue.Buffer[i][j];
		return *this;
	}

	Proxy operator[](int index) {
		return Proxy(Buffer[index]);
	}

	void SetActiveShape(Shape* _shape) {
		this->ActiveShape = _shape;
	}

	Map(int x, int y);
	Map();
};

void Renderer(Block Buffer, int x, int y);
void ShowBlockData(Block block);
BlockedType Start(Map& Buffer);
void PrintMapXY(Map map);
void thrd_InputDirectionFromUser();
Shape* CreateShape();
BlockedType MoveShape(Map& Buffer);