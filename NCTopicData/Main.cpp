#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
using json = nlohmann::json;
using namespace std;

string cutData(string _data)	//Обрезаем данные до топиков
{
	string findSubStr_from = "<div class=\"entry first\">";
	string findSubStr_to = "<!--Donate-->";
	return _data.substr(_data.find(findSubStr_from), _data.find(findSubStr_to) - _data.find(findSubStr_from));
}

void parseData()
{
	//Задаём число страниц для парсинга
	std::cout << "Type number of pages to parse: ";
	int numOfPagesToParse;
	std::cin >> numOfPagesToParse;
	if (cin.fail() || numOfPagesToParse <= 0) { std::cout << "Wrong number" << endl; return; }

	cpr::Url url;	//URL страницы
	string data;	//Ответ сервера, обрезанный до топиков (функцией cutData)
	string id, title, author, date;	//Распарсенные данные
	json topicData;		//Собранные в словарь Лохманна данные
	
	int ind_one, ind_two;		//Индексы поиска данных в полученном ответе сервера
	string find;				//Что требуется найти
	int numOfCurrentPage = 0;			//Текущий итератор страницы для формирования ссылки

	ofstream file;	//Файл, куда записываем распарсеные данные
	file.open("parsedData.txt");
	if (!file.is_open()) std::cout << "Error opening file.";	//Ошибка открытия файла
	else
	{
		//Проверяем на отсутствие строки "fatal_error", которая означает, что мы за пределами тем сайта,
		//а также на то, что мы не превысили заданное число страниц
		while (data.find("fatal_error") == string::npos && (numOfCurrentPage / 15) < numOfPagesToParse)
		{
			url = "https://www.noob-club.ru/index.php?frontpage;p=" + to_string(numOfCurrentPage);
			data = cutData(cpr::Get(url).text);	//Обрезаем данные до топиков

			while (data.find("entry first") != string::npos)
			{
				//id темы
				find = "index.php?topic=";
				ind_one = data.find(find) + find.length();
				ind_two = data.find(".0");
				id = data.substr(ind_one, ind_two - ind_one);
				data = data.substr(data.find("0\">") + 3);

				//title темы
				ind_two = data.find("</a></h1>");
				title = data.substr(0, ind_two);
				data = data.substr(data.find("title=") + 6);

				//author темы
				ind_two = data.find("\"");
				author = data.substr(0, ind_two);
				data = data.substr(data.find("</a> ") + 5);

				//date темы
				ind_two = data.find("</span>");
				date = data.substr(0, ind_two);
				if (date.find("strong") != string::npos)
					date.replace(0, 32, u8"Сегодня ");
				data = data.substr(data.find("<div class=\"entry first\">") + 12);

				//Собираем данные
				topicData[id] = { {"title", title}, {"author", author}, {"date", date} };
			}
			numOfCurrentPage += 15;	//На одной странице 15 топиков, сдвигаем итератор на 15
			std::cout << url << " was sucsessfully parced." << endl;
		}
		//Записываем данные в файл и закрываем его
		file << topicData;
		file.close();
	}
}

void viewData()
{
	ifstream file;	//Файл, откуда берём данные
	file.open("parsedData.txt");
	if (!file.is_open()) std::cout << "Error opening file." << endl << endl;	//Ошибка открытия файла
	else
	{
		json data;
		//Складываем данные в массив, чтобы вывести их только по факту чтения, если оно прошло без ошибок
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

		//Если лохманн не поймал ошибку и нормально распарсил файл, но количество полей в итоге не сходится, выводим ошибку
		//Такое произойдёт, если удалить не целый блок данных из файла
		if (id.size() != author.size() || id.size() != title.size() || id.size() != date.size()) { std::cout << "Data file corrupted." << endl; return; }

		//Вывод результата, если всё в порядке
		system("cls");	//Очистка окна
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
	//Настройка консоли на кодировку UTF8 для вывода русского языка
	SetConsoleOutputCP(CP_UTF8);

	string userInput;
	while (true)
	{
		cout << "Type \"Parse\" to parse data" << endl;
		cout << "Type \"View\" to view data if it exists." << endl;
		cout << "Type \"Exit\" to exist." << endl;

		cin >> userInput;
		if (userInput == "Parse" || userInput == "parse") parseData();	//Парсим сайт
		else if (userInput == "View" || userInput == "view") viewData();	//Просмотр результата
		else if (userInput == "Exit" || userInput == "exit") break;		//Выход
		else cout << "Error typing" << endl;	//Ошибка ввода
	}
}