#include "cell.h"

#include <cassert>
#include <optional>


Cell::Cell(SheetInterface& sheet)
	: sheet_(sheet) {
	impl_ = std::make_unique<EmptyImpl>();
}

Cell::~Cell() {}

void Cell::Set(std::string text) {
	if (text.front() == FORMULA_SIGN && text.size() > 1) {
		try {
			impl_ = std::make_unique<FormulaImpl>(sheet_, std::move(text));
			used_cells_ = impl_->GetReferencedCells();
		}
		catch (...) {
			throw FormulaException("Синтаксически некорректная формула");
		}
	}
	else {
		impl_ = std::make_unique<TextImpl>(std::move(text));
	}
}

std::vector<Position> Cell::GetReferencedCells() const {
	return used_cells_;
}

void Cell::Clear() {
	impl_ = std::make_unique<EmptyImpl>();
}

void Cell::ClearCache() {
	impl_->ClearCache();
}

Cell::Value Cell::GetValue() const {
	return impl_->GetValue();
}
std::string Cell::GetText() const {
	return impl_->GetText();
}