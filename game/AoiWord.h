#pragma once
#include <list>
#include <vector>

class Palyer {  //�����
public:
	virtual int getX() = 0;
	virtual int getY() = 0;
};

class Grid {	//ÿ������
public:
	std::list<Palyer*> m_palyer;
};

class AoiWord	//��Ϸ����
{
	int x_begin = 0, x_end = 0, x_count = 0;
	int y_begin = 0, y_end = 0, y_count = 0;
	int x_weith = 0, y_weith = 0;
	std::vector<Grid> m_grid;
public:
	AoiWord(int _xBegin,int _xEnd,int _xCount,int _yBegin,int _yEnd,int _yCount);
	virtual ~AoiWord();
	std::list<Palyer*> getSrdPalyers(Palyer *_palyer); //��ȡ��Χ���
	bool addPalyer(Palyer *_palyer); //�����ҵ�aoi
	void removePalyer(Palyer *_palyer); //ժ��aoi�е����
};

