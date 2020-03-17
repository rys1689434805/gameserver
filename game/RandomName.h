#pragma once
#include <vector>
#include <list>
#include <string>

class FirstName {
public:
	std::string m_surname;  //��
	std::vector<std::string> m_name; //��
};

class RandomName
{
	std::vector<FirstName*> m_namePool; //ʹ��vector������[]����
public:
	RandomName();
	virtual ~RandomName();
	std::string getName();
	void releaseName(std::string _name);  //��
	void loadFile();
};

