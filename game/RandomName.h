#pragma once
#include <vector>
#include <list>
#include <string>

class FirstName {
public:
	std::string m_surname;  //性
	std::vector<std::string> m_name; //名
};

class RandomName
{
	std::vector<FirstName*> m_namePool; //使用vector可以用[]访问
public:
	RandomName();
	virtual ~RandomName();
	std::string getName();
	void releaseName(std::string _name);  //还
	void loadFile();
};

