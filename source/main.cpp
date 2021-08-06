#include "FormulaAST.h"
#include "common.h"
#include "tests.h"

inline std::ostream& operator<<(std::ostream& output, Position pos) {
	return output << "(" << pos.row << ", " << pos.col << ")";
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
	return output << "(" << size.rows << ", " << size.cols << ")";
}

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
	std::visit(
			[&](const auto& x) {
				output << x;
			},
			value);
	return output;
}

int main() {
	tests::RunTests();
	
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
	
	std::cout << std::endl;
	
	sheet->PrintTexts(std::cout);
	
	return 0;
}