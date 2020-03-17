#include "AoiWord.h"
using namespace std;

AoiWord::AoiWord(int _xBegin, int _xEnd, int _xCount, int _yBegin, int _yEnd, int _yCount)
	:x_begin(_xBegin),x_end(_xEnd),x_count(_xCount),y_begin(_yBegin),y_end(_yEnd),y_count(_yCount)
{
	x_weith = (x_end - x_begin) / x_count; //x轴网格的宽度
	y_weith = (y_end - y_begin) / y_count; //y轴网格的宽度
	for (int i = 0; i < x_count * y_count; i++) {
		Grid grid;
		m_grid.push_back(grid);  //创建网格格子
	}
}

AoiWord::~AoiWord()
{
}

std::list<Palyer*> AoiWord::getSrdPalyers(Palyer *_palyer)
{
	list<Palyer*> ret;
	int grid_id = (_palyer->getX() - x_begin) / x_weith + (_palyer->getY() - y_begin) / y_weith * x_count;
	int x_index = grid_id % x_count;
	int y_index = grid_id / x_count;

	if (x_index > 0 && y_index > 0)
	{
		list<Palyer*>& cur_list = m_grid[grid_id - 1 - x_count].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (y_index > 0)
	{
		list<Palyer*>& cur_list = m_grid[grid_id - x_count].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index < x_count - 1 && y_index > 0)
	{
		list<Palyer*>& cur_list = m_grid[grid_id - x_count + 1].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index > 0)
	{
		list<Palyer*>& cur_list = m_grid[grid_id - 1].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	list<Palyer*>& cur_list = m_grid[grid_id].m_palyer;
	ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	if (x_index < x_count - 1)
	{
		list<Palyer*>& cur_list = m_grid[grid_id + 1].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index > 0 && y_index < y_count - 1)
	{
		list<Palyer*>& cur_list = m_grid[grid_id - 1 + x_count].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (y_index < y_count - 1)
	{
		list<Palyer*>& cur_list = m_grid[grid_id + x_count].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	if (x_index < x_count - 1 && y_index < y_count - 1)
	{
		list<Palyer*>& cur_list = m_grid[grid_id + 1 + x_count].m_palyer;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	return ret;
}

bool AoiWord::addPalyer(Palyer* _palyer)
{
	//计算在玩家属于几号格子
	int grid_id = (_palyer->getX() - x_begin) / x_weith + 
				(_palyer->getY() - y_begin) / y_weith * x_count;
	m_grid[grid_id].m_palyer.push_back(_palyer);	//添加到网格中
	return true;
}

void AoiWord::removePalyer(Palyer* _palyer)
{
	//计算在玩家属于几号格子
	int grid_id = (_palyer->getX() - x_begin) / x_weith +
		(_palyer->getY() - y_begin) / y_weith * x_count;
	m_grid[grid_id].m_palyer.remove(_palyer);	//摘除对应玩家
}
