#pragma once

#include "FormulaAST.h"
#include "common.h"
#include "test_runner_p.h"

namespace tests{
	
	void TestEmpty() {
		auto sheet = CreateSheet();
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
	}
	
	void TestInvalidPosition() {
		auto sheet = CreateSheet();
		try {
			sheet->SetCell(Position{-1, 0}, "");
		} catch (const InvalidPositionException &) {
		}
		try {
			sheet->GetCell(Position{0, -2});
		} catch (const InvalidPositionException &) {
		}
		try {
			sheet->ClearCell(Position{Position::MAX_ROWS, 0});
		} catch (const InvalidPositionException &) {
		}
	}
	
	void TestSetCellPlainText() {
		auto sheet = CreateSheet();
		
		auto checkCell = [&](Position pos, std::string expression, std::string text, std::string value) {
			sheet->SetCell(pos, expression);
			CellInterface *cell = sheet->GetCell(pos);
			ASSERT(cell != nullptr);
			auto res_text = cell->GetText();
			ASSERT_EQUAL(res_text, text);
			auto res = cell->GetValue();
			std::string res_val;
			if (std::holds_alternative<double>(res)) {
				res_val = std::to_string(std::get<double>(res));
			}
			else if (std::holds_alternative<FormulaError>(res)) {
				res_val = std::get<FormulaError>(res).ToString();
			}
			else {
				res_val = std::get<std::string>(res);
			}
			ASSERT_EQUAL(res_val, value);
		};
		
		checkCell("A1"_pos, "Hello", "Hello", "Hello");
		checkCell("A1"_pos, "World", "World", "World");
		checkCell("B2"_pos, "Purr", "Purr", "Purr");
		checkCell("A5"_pos, "=(1+2)*3", "=(1+2)*3", "9.000000");
		checkCell("A5"_pos, "=1+(2*3)", "=1+2*3", "7.000000");
		checkCell("A5"_pos, "=1*(2*3)", "=1*2*3", "6.000000");
		checkCell("A5"_pos, "=1/(2+3)", "=1/(2+3)", "0.200000");
		checkCell("A5"_pos, "=6/(6/3)", "=6/(6/3)", "3.000000");
		checkCell("A5"_pos, "=36/6/6", "=36/6/6", "1.000000");
		checkCell("A5"_pos, "=123+456*(238-14)-(17+5)/(327+3*(10+1)/15)", "=123+456*(238-14)-(17+5)/(327+3*(10+1)/15)",
		          "102266.933171");
		checkCell("A5"_pos, "=1/0", "=1/0", "DIV/0");
		
		const SheetInterface &constSheet = *sheet;
		ASSERT_EQUAL(constSheet.GetCell("B2"_pos)->GetText(), "Purr");
		
		sheet->SetCell("A3"_pos, "'=escaped");
		CellInterface *cell = sheet->GetCell("A3"_pos);
		ASSERT_EQUAL(cell->GetText(), "'=escaped");
		ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), "=escaped");
	}
	
	void TestClearCell() {
		auto sheet = CreateSheet();
		
		sheet->SetCell("C2"_pos, "Me gusta");
		sheet->ClearCell("C2"_pos);
		ASSERT(sheet->GetCell("C2"_pos) == nullptr);
		
		sheet->ClearCell("A1"_pos);
		sheet->ClearCell("J10"_pos);
	}
	
	void TestPrint() {
		auto sheet = CreateSheet();
		sheet->SetCell("A2"_pos, "meow");
		sheet->SetCell("B2"_pos, "=1+2");
		sheet->SetCell("A1"_pos, "=1/0");
		
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));
		
		std::ostringstream texts;
		sheet->PrintTexts(texts);
		ASSERT_EQUAL(texts.str(), "=1/0\t\nmeow\t=1+2\n");
		
		std::ostringstream values;
		sheet->PrintValues(values);
		ASSERT_EQUAL(values.str(), "#DIV/0!\t\nmeow\t   3\n");
		
		sheet->ClearCell("B2"_pos);
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 1}));
	}
	
	void TestExample() {
		auto sheet = CreateSheet();
		sheet->SetCell("A1"_pos, "=(1+2)*3");
		sheet->SetCell("B1"_pos, "=1+(2*3)");
		sheet->SetCell("A2"_pos, "some");
		sheet->SetCell("B2"_pos, "text");
		sheet->SetCell("C2"_pos, "here");
		sheet->SetCell("C3"_pos, "'and");
		sheet->SetCell("D3"_pos, "'here");
		sheet->SetCell("B5"_pos, "=1/0");
		
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 4}));
		
		std::ostringstream texts;
		sheet->PrintTexts(texts);
		ASSERT_EQUAL(texts.str(), "=(1+2)*3\t=1+2*3\t\t\n    some\t  text\there\t\n\t\t'and\t'here\n\n\t  =1/0\t\t\n");
		
		std::ostringstream values;
		sheet->PrintValues(values);
		ASSERT_EQUAL(values.str(), "       9\t     7\t\t\n    some\t  text\there\t\n\t\t and\t here\n\n\t#DIV/0!\t\t\n");
		
		sheet->ClearCell("B5"_pos);
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{3, 4}));
		
		sheet->ClearCell("A1"_pos);
		sheet->ClearCell("A2"_pos);
		sheet->ClearCell("B1"_pos);
		sheet->ClearCell("B2"_pos);
		sheet->ClearCell("C3"_pos);
		sheet->ClearCell("C2"_pos);
		sheet->ClearCell("D3"_pos);
		ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
		
		std::ostringstream texts1;
		sheet->PrintTexts(texts1);
		ASSERT_EQUAL(texts1.str(), "");
		
		std::ostringstream values1;
		sheet->PrintValues(values1);
		ASSERT_EQUAL(values1.str(), "");
	}
	
	void TestInvalidIndex() {
		auto sheet = CreateSheet();
		
		
		auto checkCell = [&](Position pos, std::string expression, bool is_correct) {
			try {
				sheet->SetCell(pos, expression);
			}
			catch (InvalidPositionException &ex) {
				if (is_correct) {
					throw ex;
				}
			}
		};
		
		checkCell("A1"_pos, "ratata", true);
		checkCell("2A"_pos, "ratata", false);
		checkCell("A"_pos, "ratata", false);
		checkCell("2A"_pos, "ratata", false);
		checkCell("e2"_pos, "ratata", false);
		checkCell("A0"_pos, "ratata", false);
		checkCell("XFD16385"_pos, "ratata", false);
		checkCell("XFE16384"_pos, "ratata", false);
		checkCell("A-1"_pos, "ratata", false);
		checkCell("A/1"_pos, "ratata", false);
		checkCell("A1234567890123456789"_pos, "ratata", false);
		checkCell("ABCDEFGHIJKLMNOPQRS8"_pos, "ratata", false);
		
	}
	
	void TestGetCell() {
		auto sheet = CreateSheet();
		std::ostringstream values;
		
		sheet->SetCell("A1"_pos, "=(1+2)*3");
		sheet->PrintValues(values);
		
		sheet->SetCell("A1"_pos, "=1+(2*3)");
		sheet->PrintValues(values);
		
		ASSERT_EQUAL(values.str(), "       9\n     7\n");
		
		sheet->SetCell("B1"_pos, "=1+(2*3)");
		sheet->SetCell("A2"_pos, "some");
		sheet->SetCell("B2"_pos, "text");
		sheet->SetCell("C2"_pos, "here");
		sheet->SetCell("C3"_pos, "'and");
		sheet->SetCell("D3"_pos, "'here");
		sheet->SetCell("B5"_pos, "=1/0");
	}
	
	void TestValidFormula() {
		auto sheet = CreateSheet();
		std::ostringstream values;
		
		sheet->SetCell("A1"_pos, "=(1+2)*3");
		sheet->PrintValues(values);
		
		sheet->SetCell("A1"_pos, "=1+(2*3)");
		sheet->PrintValues(values);
		
		ASSERT_EQUAL(values.str(), "       9\n     7\n");
		
		sheet->SetCell("B1"_pos, "=1+(2*3)");
		sheet->SetCell("A2"_pos, "some");
		sheet->SetCell("B2"_pos, "text");
		sheet->SetCell("C2"_pos, "here");
		sheet->SetCell("C3"_pos, "'and");
		sheet->SetCell("D3"_pos, "'here");
		sheet->SetCell("B5"_pos, "=1/0");
	}
	
	void TestSetCellPlainText2() {
		auto sheet = CreateSheet();
		
		auto checkCell = [&](Position pos, std::string expression, std::string text, double value) {
			sheet->SetCell(pos, expression);
			CellInterface *cell = sheet->GetCell(pos);
			ASSERT(cell != nullptr);
			auto res_text = cell->GetText();
			ASSERT_EQUAL(res_text, text);
			auto res = cell->GetValue();
			double res_val;
			if (std::holds_alternative<double>(res)) {
				res_val = std::get<double>(res);
			}
			
			ASSERT_EQUAL(res_val, value);
		};
		
		checkCell("A5"_pos, "=(1+2)*3", "=(1+2)*3", 9);
		checkCell("A5"_pos, "=1+(2*3)", "=1+2*3", 7);
		checkCell("A5"_pos, "=1*(2*3)", "=1*2*3", 6);
		checkCell("A5"_pos, "=1/(2+3)", "=1/(2+3)", 0.2);//0.20000000000000001
		checkCell("A5"_pos, "=6/(6/3)", "=6/(6/3)", 3);
		checkCell("A5"_pos, "=36/6/6", "=36/6/6", 1);
		checkCell("A5"_pos, "=(1+2)+3", "=1+2+3", 6);
		checkCell("A5"_pos, "=1+(2+3)", "=1+2+3", 6);
		checkCell("A5"_pos, "=(1*2)+3", "=1*2+3", 5);
		checkCell("A5"_pos, "=1*2+3", "=1*2+3", 5);
		checkCell("A5"_pos, "=1*(2+3)", "=1*(2+3)", 5);
		checkCell("A5"_pos, "=1*2*3", "=1*2*3", 6);
		checkCell("A5"_pos, "=(1*2*3)", "=1*2*3", 6);
		checkCell("A5"_pos, "=1*(2*3)", "=1*2*3", 6);
		checkCell("A5"_pos, "=(1/2)+3", "=1/2+3", 3.5);
		checkCell("A5"_pos, "=1/2+3", "=1/2+3", 3.5);
		checkCell("A5"_pos, "=1-2+3", "=1-2+3", 2);
		checkCell("A5"_pos, "=(1-2)-3", "=1-2-3", -4);
		checkCell("A5"_pos, "=-(-1)", "=--1", 1); // ?
		checkCell("A5"_pos, "=(-1)", "=-1", -1);
		checkCell("A5"_pos, "=-1", "=-1", -1);
		checkCell("A5"_pos, "=2-(-1)", "=2--1", 3); // ?
		checkCell("A5"_pos, "=2*(-1)", "=2*-1", -2); // ?
		checkCell("A5"_pos, "=2/(-1)", "=2/-1", -2); // ?
		
		checkCell("A5"_pos, "=123+456*(238-14)-(17+5)/(327+3*(10+1)/15)", "=123+456*(238-14)-(17+5)/(327+3*(10+1)/15)",
		          102266.93317132442);
		
		sheet->SetCell("B2"_pos, "Purr");
		const SheetInterface &constSheet = *sheet;
		ASSERT_EQUAL(constSheet.GetCell("B2"_pos)->GetText(), "Purr");
		
		sheet->SetCell("A3"_pos, "'=escaped");
		CellInterface *cell = sheet->GetCell("A3"_pos);
		ASSERT_EQUAL(cell->GetText(), "'=escaped");
		ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), "=escaped");
	}
	
	void RunTests() {
		TestRunner tr;
		RUN_TEST(tr, TestEmpty);
		RUN_TEST(tr, TestInvalidPosition);
		RUN_TEST(tr, TestSetCellPlainText);
		RUN_TEST(tr, TestSetCellPlainText2);
		RUN_TEST(tr, TestClearCell);
		RUN_TEST(tr, TestPrint);
		RUN_TEST(tr, TestExample);
		RUN_TEST(tr, TestInvalidIndex);
		RUN_TEST(tr, TestGetCell);
		RUN_TEST(tr, TestValidFormula);
	}

} // end tests
