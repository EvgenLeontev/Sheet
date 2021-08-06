#include "formula.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}


Formula::Formula(const std::string& expression)
	: ast_(ParseFormulaAST(expression))
	{}
	
FormulaInterface::Value Formula::Evaluate(const SheetInterface& sheet) const {
	Value value;
	try {
		double v = ast_.Execute(sheet);
		value = v;
	}
	catch (FormulaError& er) {
		value = er;
	}
	return value;
}

std::string Formula::GetExpression() const {
	std::stringstream out;
	ast_.PrintFormula(out);
	return out.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
	return {ast_.GetCells().begin(), ast_.GetCells().end()};
}


std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}