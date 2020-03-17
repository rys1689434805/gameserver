#pragma once
#include <list>
#include <vector>

class Palyer {  //玩家类
public:
	virtual int getX() = 0;
	virtual int getY() = 0;
};

class Grid {	//每个格子
public:
	std::list<Palyer*> m_palyer;
};

class AoiWord	//游戏世界
{
	int x_begin = 0, x_end = 0, x_count = 0;
	int y_begin = 0, y_end = 0, y_count = 0;
	int x_weith = 0, y_weith = 0;
	std::vector<Grid> m_grid;
public:
	AoiWord(int _xBegin,int _xEnd,int _xCount,int _yBegin,int _yEnd,int _yCount);
	virtual ~AoiWord();
	std::list<Palyer*> getSrdPalyers(Palyer *_palyer); //获取周围玩家
	bool addPalyer(Palyer *_palyer); //添加玩家到aoi
	void removePalyer(Palyer *_palyer); //摘除aoi中的玩家
};

