#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <array>
#include <map>
#include <set>

class Sheet : public SheetInterface {
public:
	
	~Sheet();
	
	void SetCell(Position pos, std::string text) override;
	
	const CellInterface* GetCell(Position pos) const override;
	CellInterface* GetCell(Position pos) override;
	
	void ClearCell(Position pos) override;
	
	Size GetPrintableSize() const override;
	
	void PrintValues(std::ostream& output) const override;
	void PrintTexts(std::ostream& output) const override;
	
	bool IsCycled(Position this_cell_pos, std::vector<Position> used_cells, std::set<Position>& viewed_cells) const;
	
private:
	std::map<int, std::map<int, std::unique_ptr<CellInterface>>> cells_;
	std::array<int, Position::MAX_ROWS> filled_cells_in_rows_{0};
	std::array<int, Position::MAX_COLS> filled_cells_in_cols_{0};
	std::map<Position, std::set<Position>> dependent_cells_;

};