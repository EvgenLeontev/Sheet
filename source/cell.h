#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
	Cell(SheetInterface &sheet);
	
	~Cell();
	
	void Set(std::string text) override ;
	
	void Clear();
	void ClearCache() override;
	
	Value GetValue() const override;
	
	std::string GetText() const override;
	
	std::vector<Position> GetReferencedCells() const override;


private:
	class Impl {
	public:
		virtual Value GetValue() = 0;
		virtual std::string GetText() = 0;
		virtual std::vector<Position> GetReferencedCells() const = 0;
		virtual void ClearCache() = 0;
		virtual ~Impl() = default;
	};
	
	class EmptyImpl : public Impl {
	public:
		EmptyImpl() = default;
		Value GetValue() override {
			return value_;
		}
		std::string GetText() override {
			return "0";
		}
		std::vector<Position> GetReferencedCells() const override {
			return {};
		}
		void ClearCache() override {}
	
	private:
		double value_ = 0;
	};
	
	class TextImpl : public Impl {
	public:
		explicit TextImpl(std::string text) : text_(std::move(text)) {}
		
		Value GetValue() override {
			
			if (text_.front() == ESCAPE_SIGN) {
				return std::string{next(text_.begin()), text_.end()};
			}
			return text_;
		}
		std::string GetText() override {
			return text_;
		}
		std::vector<Position> GetReferencedCells() const override {
			return {};
		}
		void ClearCache() override {}
	
	private:
		std::string text_;
	};
	
	class FormulaImpl : public Impl {
	public:
		explicit FormulaImpl(SheetInterface& sheet, std::string text)
			: sheet_(sheet) {
			formula_ptr_ = ParseFormula({next(text.begin()), text.end()});
		}
		
		Value GetValue() override {
			if (!cache_) {
				FormulaInterface::Value v = formula_ptr_->Evaluate(sheet_);
				if (std::holds_alternative<double>(v)) {
					cache_.emplace(std::get<double>(v));
				}
				else {
					cache_.emplace(FormulaError(FormulaError::Category::Div0));
				}
			}
			return cache_.value();
		}
		
		void ClearCache() override {
			cache_.reset();
		}
		
		std::string GetText() override {
			return FORMULA_SIGN + formula_ptr_->GetExpression();
		}
		std::vector<Position> GetReferencedCells() const override {
			return formula_ptr_->GetReferencedCells();
		}
	
	private:
		std::unique_ptr<FormulaInterface> formula_ptr_;
		SheetInterface& sheet_;
		std::optional<Value> cache_;
	};
	
	std::unique_ptr<Impl> impl_;
	std::vector<Position> used_cells_;
	SheetInterface& sheet_;
};