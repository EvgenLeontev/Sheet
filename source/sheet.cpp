#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
	if (pos.IsValid()) {
		std::unique_ptr<CellInterface> cell = std::make_unique<Cell>(*this);
		cell->Set(text);
		std::set<Position> viewed_cells;
		if (IsCycled(pos, cell->GetReferencedCells(), viewed_cells)){
			throw CircularDependencyException("Circular dependency");
		}
		else {
			if (cells_[pos.row][pos.col] == nullptr) {
				++filled_cells_in_rows_[pos.row];
				++filled_cells_in_cols_[pos.col];
			}
			cells_[pos.row][pos.col] = std::move(cell);
			for (auto& ref_pos : cells_[pos.row][pos.col]->GetReferencedCells()) {
				dependent_cells_[ref_pos].insert(pos);
			}
		}
	}
	else {
		throw InvalidPositionException("Invalid position");
	}
}

bool Sheet::IsCycled(Position this_cell_pos, std::vector<Position> used_cells, std::set<Position>& viewed_cells) const {
	if (used_cells.empty()) {
		return false;
	}
	for (auto& pos : used_cells) {
		if (viewed_cells.count(pos) == 0) {
			if (pos == this_cell_pos) {
				return true;
			}
			viewed_cells.insert(pos);
			
			if (IsCycled(this_cell_pos, GetCell(pos)->GetReferencedCells(), viewed_cells)) {
				return true;
			}
		}
	}
	return false;
}

const CellInterface* Sheet::GetCell(Position pos) const {
	if (pos.IsValid()) {
		return cells_.at(pos.row).at(pos.col).get();
	}
	throw InvalidPositionException("Invalid position");
}
CellInterface* Sheet::GetCell(Position pos) {
	if (pos.IsValid()) {
		return cells_[pos.row][pos.col].get();
	}
	throw InvalidPositionException("Invalid position");
}

void Sheet::ClearCell(Position pos) {
	if (pos.IsValid()) {
		if (cells_.count(pos.row) != 0 && !cells_.at(pos.row).empty()) {
			if (cells_.at(pos.row).count(pos.col) != 0 && cells_.at(pos.row).at(pos.col) != nullptr) {
				for (auto &dep_pos : dependent_cells_[pos]) {
					GetCell(dep_pos)->ClearCache();
				}
				dependent_cells_.erase(pos);
				cells_[pos.row][pos.col] = nullptr;
				--filled_cells_in_cols_[pos.col];
				--filled_cells_in_rows_[pos.row];
			}
		}
	}
	else {
		throw InvalidPositionException("Invalid position");
	}
}

Size Sheet::GetPrintableSize() const {
	auto last_row = std::find_if(filled_cells_in_rows_.rbegin(), filled_cells_in_rows_.rend(), [](int i) {return i != 0;});
	int rows = 0;
	if (last_row != filled_cells_in_rows_.rend()) {
		rows = std::distance(last_row, filled_cells_in_rows_.rend());
	}
	auto last_col = std::find_if(filled_cells_in_cols_.rbegin(), filled_cells_in_cols_.rend(), [](int i) {return i != 0;});
	int cols = 0;
	if (last_col != filled_cells_in_cols_.rend()) {
		cols = std::distance(last_col, filled_cells_in_cols_.rend());
	}
	return {rows, cols};
}

void Sheet::PrintValues(std::ostream& output) const {
	Size size(GetPrintableSize());
	
	std::vector<int> max_cells_widths(size.cols);
	
	for (int row = 0; row < size.rows; ++row) {
		if (cells_.count(row) != 0 && !cells_.at(row).empty()) {
			for (int col = 0; col < size.cols; ++col) {
				if (cells_.at(row).count(col) != 0 && cells_.at(row).at(col) != nullptr) {
					int cell_wigth = cells_.at(row).at(col)->GetText().size();
					if (cell_wigth > max_cells_widths[col]) {
						max_cells_widths[col] = cell_wigth;
					}
				}
			}
		}
	}

	for (int row = 0; row < size.rows; ++row) {
		if (cells_.count(row) != 0 && !cells_.at(row).empty()) {
			for (int col = 0; col < size.cols; ++col) {
				if (cells_.at(row).count(col) != 0 && cells_.at(row).at(col) != nullptr) {
					CellInterface::Value value = cells_.at(row).at(col)->GetValue();
					if (std::holds_alternative<double>(value)) {
						output << std::setw(max_cells_widths[col]) << std::get<double>(value);
					}
					else if (std::holds_alternative<std::string>(value)) {
						output << std::setw(max_cells_widths[col]) << std::get<std::string>(value);
					}
					else if (std::holds_alternative<FormulaError>(value)) {
						output << std::setw(max_cells_widths[col]) << std::get<FormulaError>(value);
					}
				}
				if (col != size.cols - 1) {
					output << "\t";
				}
			}
		}
		output << "\n";
	}
}

void Sheet::PrintTexts(std::ostream& output) const {
	Size size(GetPrintableSize());
	
	std::vector<int> max_cells_widths(size.cols);
	
	for (int row = 0; row < size.rows; ++row) {
		if (cells_.count(row) != 0 && !cells_.at(row).empty()) {
			for (int col = 0; col < size.cols; ++col) {
				if (cells_.at(row).count(col) != 0 && cells_.at(row).at(col) != nullptr) {
					int cell_wigth = cells_.at(row).at(col)->GetText().size();
					if (cell_wigth > max_cells_widths[col]) {
						max_cells_widths[col] = cell_wigth;
					}
				}
			}
		}
	}
	
	for (int row = 0; row < size.rows; ++row) {
		if (cells_.count(row) != 0 && !cells_.at(row).empty()) {
			for (int col = 0; col < size.cols; ++col) {
				if (cells_.at(row).count(col) != 0 && cells_.at(row).at(col) != nullptr) {
					output << std::setw(max_cells_widths[col]) << cells_.at(row).at(col)->GetText();
				}
				if (col != size.cols - 1) {
					output << "\t";
				}
			}
		}
		output << "\n";
	}
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}