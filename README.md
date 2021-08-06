# Sheet

Sheet - электронная таблица, аналог электронной таблицы MS Excel или Google Sheets. Лист электронной таблицы содержит 
ячейки с данными. 

# Реализация

Ячейки могут быть формульными и текстовыми. Если текст начинается со знака "=", то ячейка считается формульной, 
а то, что следует после знака "=" - выражением формулы. Формула может содержать индексы ячеек. Все остальные ячейки 
трактуются как текстовые.

Формула представлена в виде дерева, по нему удобно рекурсивно вычислять значение формулы. Во внутренних узлах дерева 
находятся операции, а в листьях — операнды. Лексический анализ и синтаксический анализ — то есть составление дерева 
разбора — создаёт ANTLR. Метод ParseFormulaAST(std::istream&) создаёт дерево из текста формулы. Он поочерёдно 
вызывает лексический и синтаксический анализаторы, а затем рекурсивно обходит дерево разбора и строит заготовку 
дерева для вычислений. За основу взято абстрактное синтаксическое дерево. 

# Запуск

Установите:  
1. Скачайте проект
2. [Java SE Runtime Environment 8](https://www.oracle.com/java/technologies/javase-jre8-downloads.html)  
3. [Java SE Development Kit 8](https://www.oracle.com/java/technologies/javase/javase-jdk8-downloads.html)  
4. [ANTLR 4](https://www.antlr.org/)  
Нам нужны программы antlr4 и grun. Возможно, у вас antlr4 будет называться просто antlr. Убедитесь, 
что JAR-файл antlr*.jar находится в переменной среды CLASSPATH, это нужно для компиляции Java-кода. 
Если вы следовали инструкциям на сайте ANTLR, CLASSPATH должен уже быть правильным.
Библиотека ANTLR 4 C++ Runtime и CMake-файлы для её интеграции.
5. Запустите

# Использование

Прежде всего следует создать объект таблицы.  
 
```auto sheet = CreateSheet();```

Далее возможна работа с таблицей с помощью следущих методов:
* SetCell - задаёт содержимое ячейки.
* GetCell - возвращает содержимое ячейки.
* ClearCell - очищает ячейку.
* GetPrintableSize - вычисляет размер области, которая участвует в печати.
* PrintValues и PrintTexts - выводят всю таблицу в переданный поток. 

# Пример программы

Представим, что мы владеем портфелем из некоторого количества ценных бумаг и хотим вести их учёт.
У нас есть 10 акций компании Северсталь по цене 1600 рублей за штуку, 100 акций компании Сбер Банк по цене 300 рублей за штуку и 5 акций компании Яндекс по цене 5000 рублей за штуку. Мы хотим видеть стоимость всех акций каждой компании, какую долю в портфеле они занимают и стоимость всего портфеля в целом. Справиться с этим поможет программа Sheet.

Пример кода:

```C++
int main() {
	auto sheet = CreateSheet();
	
	sheet->SetCell("A2"_pos, "Portfolio value");
	
	sheet->SetCell("A4"_pos, "Number");
	sheet->SetCell("A5"_pos, "Company");
	sheet->SetCell("A6"_pos, "Ticker");
	sheet->SetCell("A7"_pos, "Actual price");
	sheet->SetCell("A8"_pos, "Currency");
	sheet->SetCell("A9"_pos, "Quantity");
	sheet->SetCell("A10"_pos, "Value");
	sheet->SetCell("A11"_pos, "Weigth");
	
	sheet->SetCell("B4"_pos, "1");
	sheet->SetCell("B5"_pos, "Severstal");
	sheet->SetCell("B6"_pos, "CHMF");
	sheet->SetCell("B7"_pos, "= 1600");
	sheet->SetCell("B8"_pos, "Rub");
	sheet->SetCell("B9"_pos, "= 10");
	sheet->SetCell("B10"_pos, "= B7 * B9");
	
	sheet->SetCell("C4"_pos, "2");
	sheet->SetCell("C5"_pos, "SberBank");
	sheet->SetCell("C6"_pos, "SBER");
	sheet->SetCell("C7"_pos, "= 300");
	sheet->SetCell("C8"_pos, "Rub");
	sheet->SetCell("C9"_pos, "= 100");
	sheet->SetCell("C10"_pos, "= C7 * C9");
	
	sheet->SetCell("D4"_pos, "3");
	sheet->SetCell("D5"_pos, "Yandex");
	sheet->SetCell("D6"_pos, "YNDX");
	sheet->SetCell("D7"_pos, "= 5000");
	sheet->SetCell("D8"_pos, "Rub");
	sheet->SetCell("D9"_pos, "= 5");
	sheet->SetCell("D10"_pos, "= D7 * D9");
	
	sheet->SetCell("B2"_pos, "= B10 + C10 + D10");
	sheet->SetCell("B11"_pos, "= B10 / B2 * 100");
	sheet->SetCell("C11"_pos, "= C10 / B2 * 100");
	sheet->SetCell("D11"_pos, "= D10 / B2 * 100");
	
	sheet->PrintValues(std::cout);
	
	return 0;
}
```
Пример вывода метода PrintValues

![PrintValues](https://github.com/EvgenLeontev/Sheet/blob/main/readme_files/PrintValues.png)

Пример вывода метода PrintTexts

![PrintTexts](https://github.com/EvgenLeontev/Sheet/blob/main/readme_files/PrintTexts.png)

# Требования

* C++17 
* Java SE Runtime Environment 8
* Java SE Development Kit 8
* ANTLR 4


