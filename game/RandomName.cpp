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
	auto first = m_namePool[num]->m_surname; //ȡ��
	auto last = m_namePool[num]->m_name[num_last]; //ȡ��
	m_namePool[num]->m_name.erase(m_namePool[num]->m_name.begin() + num_last); //��ȡ������ɾ��
	//������յ���ȫ��ȡ�棬�Ͱѱ���ɾ��
	if (m_namePool[num]->m_name.size() <= 0) {
		delete m_namePool[num];  //namePool�����ָ����󣬻��ڻ��ĺ�����new
		m_namePool.erase(m_namePool.begin() + num);
	}
	return first+" "+last;  //�м���Ͽո�ý�ȡ
}

void RandomName::releaseName(std::string _name)
{
	int num = _name.find(" ");
	string surname = _name.substr(0, num);
	string name = _name.substr(num + 1, _name.size() - num - 1);
	bool falg = false;
	for (auto single : m_namePool) {  //����
		if (surname == single->m_surname) {
			falg = true;
			single->m_name.push_back(name); //�����Ż�ȥ
		}
	}
	if (falg == false) {  //��ʾname����û���������
		auto first_name = new FirstName();
		first_name->m_name.push_back(name); //�����
		m_namePool.push_back(first_name); //�����
	}
}

void RandomName::loadFile()
{
	ifstream first("random_first.txt");
	ifstream last("random_last.txt");
	string last_name;
	vector<string> tmp;
	while (getline(last,last_name))  //ѭ������,һ��һ�ж�
	{
		tmp.push_back(last_name);
	}
	string first_name;
	while (getline(first, first_name)) //ѭ������,��һ��newһ������ֵ
	{
		auto first_name_list = new FirstName();
		first_name_list->m_surname = first_name;
		first_name_list->m_name = tmp;
		m_namePool.push_back(first_name_list);
	}
}
