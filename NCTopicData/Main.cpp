#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
using json = nlohmann::json;
using namespace std;

string cutData(string _data)	//�������� ������ �� �������
{
	string findSubStr_from = "<div class=\"entry first\">";
	string findSubStr_to = "<!--Donate-->";
	return _data.substr(_data.find(findSubStr_from), _data.find(findSubStr_to) - _data.find(findSubStr_from));
}

void parseData()
{
	//����� ����� ������� ��� ��������
	std::cout << "Type number of pages to parse: ";
	int numOfPagesToParse;
	std::cin >> numOfPagesToParse;
	if (cin.fail() || numOfPagesToParse <= 0) { std::cout << "Wrong number" << endl; return; }

	cpr::Url url;	//URL ��������
	string data;	//����� �������, ���������� �� ������� (�������� cutData)
	string id, title, author, date;	//������������ ������
	json topicData;		//��������� � ������� �������� ������
	
	int ind_one, ind_two;		//������� ������ ������ � ���������� ������ �������
	string find;				//��� ��������� �����
	int numOfCurrentPage = 0;			//������� �������� �������� ��� ������������ ������

	ofstream file;	//����, ���� ���������� ����������� ������
	file.open("parsedData.txt");
	if (!file.is_open()) std::cout << "Error opening file.";	//������ �������� �����
	else
	{
		//��������� �� ���������� ������ "fatal_error", ������� ��������, ��� �� �� ��������� ��� �����,
		//� ����� �� ��, ��� �� �� ��������� �������� ����� �������
		while (data.find("fatal_error") == string::npos && (numOfCurrentPage / 15) < numOfPagesToParse)
		{
			url = "https://www.noob-club.ru/index.php?frontpage;p=" + to_string(numOfCurrentPage);
			data = cutData(cpr::Get(url).text);	//�������� ������ �� �������

			while (data.find("entry first") != string::npos)
			{
				//id ����
				find = "index.php?topic=";
				ind_one = data.find(find) + find.length();
				ind_two = data.find(".0");
				id = data.substr(ind_one, ind_two - ind_one);
				data = data.substr(data.find("0\">") + 3);

				//title ����
				ind_two = data.find("</a></h1>");
				title = data.substr(0, ind_two);
				data = data.substr(data.find("title=") + 6);

				//author ����
				ind_two = data.find("\"");
				author = data.substr(0, ind_two);
				data = data.substr(data.find("</a> ") + 5);

				//date ����
				ind_two = data.find("</span>");
				date = data.substr(0, ind_two);
				if (date.find("strong") != string::npos)
					date.replace(0, 32, u8"������� ");
				data = data.substr(data.find("<div class=\"entry first\">") + 12);

				//�������� ������
				topicData[id] = { {"title", title}, {"author", author}, {"date", date} };
			}
			numOfCurrentPage += 15;	//�� ����� �������� 15 �������, �������� �������� �� 15
			std::cout << url << " was sucsessfully parced." << endl;
		}
		//���������� ������ � ���� � ��������� ���
		file << topicData;
		file.close();
	}
}

void viewData()
{
	ifstream file;	//����, ������ ���� ������
	file.open("parsedData.txt");
	if (!file.is_open()) std::cout << "Error opening file." << endl << endl;	//������ �������� �����
	else
	{
		json data;
		//���������� ������ � ������, ����� ������� �� ������ �� ����� ������, ���� ��� ������ ��� ������
		vector<string> id, author, title, date;
		try 
		{
			file >> data;
			for (json::iterator it_id = data.begin(); it_id != data.end(); ++it_id)
			{
				id.push_back(it_id.key());
				for (json::iterator it_topic = it_id->begin(); it_topic != it_id->end(); ++it_topic)
				{
					if (it_topic.key() == "author") author.push_back(it_topic.value());
					else if (it_topic.key() == "title") title.push_back(it_topic.value());
					else if (it_topic.key() == "date") date.push_back(it_topic.value());
				}
			}
		}
		catch (exception& ex) { cerr << "Data file corrupted." << endl; }

		//���� ������� �� ������ ������ � ��������� ��������� ����, �� ���������� ����� � ����� �� ��������, ������� ������
		//����� ���������, ���� ������� �� ����� ���� ������ �� �����
		if (id.size() != author.size() || id.size() != title.size() || id.size() != date.size()) { std::cout << "Data file corrupted." << endl; return; }

		//����� ����������, ���� �� � �������
		system("cls");	//������� ����
		cout << "Parsed data:" << endl << endl;
		for (int i = 0; i < data.size(); i++)
			cout << "ID " << id[i] << ": \"" << title[i] << "\"" << endl << 
					"Author: " << author[i] << endl << 
					"Date: " << date[i] << endl << endl;
	}
	file.close();
}

int main()
{
	//��������� ������� �� ��������� UTF8 ��� ������ �������� �����
	SetConsoleOutputCP(CP_UTF8);

	string userInput;
	while (true)
	{
		cout << "Type \"Parse\" to parse data" << endl;
		cout << "Type \"View\" to view data if it exists." << endl;
		cout << "Type \"Exit\" to exist." << endl;

		cin >> userInput;
		if (userInput == "Parse" || userInput == "parse") parseData();	//������ ����
		else if (userInput == "View" || userInput == "view") viewData();	//�������� ����������
		else if (userInput == "Exit" || userInput == "exit") break;		//�����
		else cout << "Error typing" << endl;	//������ �����
	}
}