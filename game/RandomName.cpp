#include "RandomName.h"
#include <random>
#include <fstream>
using namespace std;

static default_random_engine random_engine(time(NULL));

RandomName::RandomName()
{
}

RandomName::~RandomName()
{
}

std::string RandomName::getName()
{
	int num = random_engine() % m_namePool.size();
	int num_last = random_engine() % m_namePool[num]->m_name.size();
	auto first = m_namePool[num]->m_surname; //取姓
	auto last = m_namePool[num]->m_name[num_last]; //取名
	m_namePool[num]->m_name.erase(m_namePool[num]->m_name.begin() + num_last); //把取到的名删除
	//如果本姓的名全部取玩，就把本姓删除
	if (m_namePool[num]->m_name.size() <= 0) {
		delete m_namePool[num];  //namePool存的是指针对象，会在还的函数中new
		m_namePool.erase(m_namePool.begin() + num);
	}
	return first+" "+last;  //中间加上空格好截取
}

void RandomName::releaseName(std::string _name)
{
	int num = _name.find(" ");
	string surname = _name.substr(0, num);
	string name = _name.substr(num + 1, _name.size() - num - 1);
	bool falg = false;
	for (auto single : m_namePool) {  //遍历
		if (surname == single->m_surname) {
			falg = true;
			single->m_name.push_back(name); //把名放回去
		}
	}
	if (falg == false) {  //表示name池中没有这个对象
		auto first_name = new FirstName();
		first_name->m_name.push_back(name); //添加名
		m_namePool.push_back(first_name); //添加性
	}
}

void RandomName::loadFile()
{
	ifstream first("random_first.txt");
	ifstream last("random_last.txt");
	string last_name;
	vector<string> tmp;
	while (getline(last,last_name))  //循环读名,一行一行读
	{
		tmp.push_back(last_name);
	}
	string first_name;
	while (getline(first, first_name)) //循环读姓,读一行new一个对象赋值
	{
		auto first_name_list = new FirstName();
		first_name_list->m_surname = first_name;
		first_name_list->m_name = tmp;
		m_namePool.push_back(first_name_list);
	}
}
