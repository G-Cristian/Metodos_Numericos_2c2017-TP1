#ifndef _MATRIZESPARSA_H_
#define _MATRIZESPARSA_H_

#include "AbstractMatrix.h"
#include "vector.h"
#include "MapVectorImp.h"

#include <map>
#include <utility>
#include <vector>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

namespace MN {
	template<class T>
	class SparceMatrix :public AbstractMatrix<T> {
	public:
		typedef AbstractMatrix<T>::RowType RowType;
		typedef AbstractMatrix<T>::ColumnType ColumnType;
		typedef std::map<int, RowType> MatrixType;

		explicit SparceMatrix(int height, int width) :
			AbstractMatrix<T>(height, width){
		}

		explicit SparceMatrix(int height, int width, const T *m) :
			AbstractMatrix<T>(height, width) {

			for (int i = 0; i < _height; i++) {
				RowType r(new MapVectorImp<T>(width));
				for (int j = 0; j < width; j++) {
					r.setValueAtIndex(m[i*width + j], j);
				}
				_matrix.insert(std:pair<int, RowType>(i, std::move(r)));
			}
		}

		explicit SparceMatrix(const MatrixType &rows) :
			AbstractMatrix<T>(rows.size(), ((rows.size() > 0) ? rows[0].size() : 0)),
			_matrix(rows) {
		}

		SparceMatrix(const AbstractMatrix<T> &other) :
			AbstractMatrix<T>(other.height(), other.width()){

			auto copy = [&other](int row, int column, const T &value) {
				return  other.atRowColumn(row, column);
			};
			this->applyFunctionToElements(copy);
		}

		SparceMatrix(const SparceMatrix<T> &other) :
			AbstractMatrix<T>(other._height, other._width),
			_matrix(other._matrix) {
		}

		SparceMatrix(SparceMatrix<T> &&other) noexcept :
			AbstractMatrix<T>(other._height, other._width) {
			_matrix.insert(std::make_move_iterator(other._matrix.begin()), std::make_move_iterator(other._matrix.end()));
		}

		virtual ~SparceMatrix() {};

		std::unique_ptr<AbstractMatrix<T> > operator*(const AbstractMatrix<T> &other) const override final;


		RowType getRow(int rowIndex) const override final{
			auto it = _matrix.find(rowIndex);
			if (it != _matrix.end()) {
				return it->second;
			}
			else {
				return RowType(new MapVectorImp<T>(_width));
			}
		}

		ColumnType getColumn(int columnIndex) const override final;

		const T atRowColumn(int row, int column) const override final {
			assert(indexesInRange(row, column));
			return getRow(row).at(column);
		}

		const RowType operator[](int rowIndex) const override final {
			assert(rowInRange(rowIndex));
			return getRow(rowIndex);
		}

		void setValueInRowColumn(const T &value, int row, int column) override final {
			MatrixType::iterator it = _matrix.find(row);
			if (value == T()) {
				if (it != _matrix.end()) {
					//we are inserting a 'zero' value in an existing row
					RowType &r = it->second;
					r.setValueAtIndex(value, column);
					//since we inserted a 'zero' value, there is the posibility that we removed the elements if the vector is implemented with a MapVectorImp
					if (r.numberOfElements() == 0) {
						//if we removed the element in the row and it's empty now, we have to remove it
						_matrix.erase(it);
					}
				}
				//if we are inserting a 'zero' value in a non-existing row we don't have to add one
			}
			else {
				if (it != _matrix.end()) {
					it->second.setValueAtIndex(value, column);
				}
				else {
					//we are inserting a non-'zero' value in a non-existing row
					//so we have to create a new one.
					RowType newRow(new MapVectorImp<T>(_width));
					newRow.setValueAtIndex(value, column);
					_matrix.insert(std::pair<int, RowType>(row, std::move(newRow)));
				}
			}
		}

		SparceMatrix<T>& operator=(const SparceMatrix<T> &other) {
			if (&other != this) {
				_matrix = other._matrix();
			}
			return *this;
		}

		//other operations

		std::unique_ptr<AbstractMatrix<T> > transpose() const override final;

		T norm() const override final {
			assert(_height == 1);

			return getRow(0).norm();
		}

		AbstractMatrix<T>* createInstance(int height, int width) const override final {
			return new SparceMatrix<T>(height, width);
		}

		AbstractMatrix<T>* clone(const AbstractMatrix<T> &other) const override final {
			return new SparceMatrix<T>(other);
		}

	private:
		MatrixType _matrix;
	};

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > SparceMatrix<T>::operator*(const AbstractMatrix<T> &other) const {
		assert(_width == other.height());

		SparceMatrix<T> *mat = new SparceMatrix<T>(_height, other.width());
		T aux = 0;

		for (auto rowIt = _matrix.begin(); rowIt != _matrix.end(); ++rowIt) {
			for (Vector<T>::VectorConstIterator columnIt = rowIt->second.begin(); columnIt != rowIt->second.end(); ++columnIt) {
				std::pair<size_t, T> tempColumn1 = *columnIt;
				int row1 = rowIt->first;
				int col1 = tempColumn1.first;
				const RowType &rowOfOther = other.getRow(col1);
				for (Vector<T>::VectorConstIterator columnItOther = rowOfOther.begin(); columnItOther != rowOfOther.end(); ++columnItOther) {
					std::pair<size_t, T> tempColumn2 = *columnItOther;
					int col2 = tempColumn2.first;
					T tempMultiplication = tempColumn1.second *tempColumn2.second;
					mat->setValueInRowColumn(mat->atRowColumn(row1, col2) + tempMultiplication, row1, col2);
				}
			}
		}

		return std::unique_ptr<AbstractMatrix<T> >(mat);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > SparceMatrix<T>::transpose() const  {
		SparceMatrix<T> *aux = new SparceMatrix<T>(_width, _height);

		for (auto rowIt = _matrix.begin(); rowIt != _matrix.end(); ++rowIt) {
			for (Vector<T>::VectorConstIterator columnIt = rowIt->second.begin(); columnIt != rowIt->second.end(); ++columnIt) {
				aux->setValueInRowColumn((*columnIt).second, (*columnIt).first, rowIt->first);
			}
		}

		return std::unique_ptr<AbstractMatrix<T> >(aux);
	}

	template<class T>
	Vector<T> SparceMatrix<T>::getColumn(int columnIndex) const {
		assert(columnInRange(columnIndex));
		Vector<T> column(new MapVectorImp<T>(_height));

		for (auto it = _matrix.begin(); it != _matrix.end(); ++it) {
			column.setValueAtIndex(it->second.at(columnIndex), it->first);
		}

		return column;
	}

	/*
	public:
		explicit SparceMatrix(int height, int width) :
			AbstractMatrix<T>(height, width),
			_matrix(height, RowType(new VectorVectorImp<T>(width))) {
		}

		virtual ~SparceMatrix() = default;
	private:
	};

	class MatrizEsparsa {
	public:
		MatrizEsparsa(int alto, int ancho);
		template<class T> MatrizEsparsa(const Matrix<T> &otra);

		~MatrizEsparsa();

		inline int ancho() const { return _ancho; }
		inline int alto() const { return _alto; }

		tipoElementos enYX(int y, int x) const;
		void insertarEnYX(int y, int x, tipoElementos elemento);
		MatrizEsparsa operator*(const MatrizEsparsa &otra) const;
		MatrizEsparsa transpuesta() const;
		MatrizEsparsa transpuestaPorOtra(const MatrizEsparsa &otra) const;
		inline const map<int, tipoElementos>& filaI(int i) const { return _columnasConElementosEnFilaI[i]; }
	private:
		vector<map<int, tipoElementos> > _columnasConElementosEnFilaI;
		int _alto;
		int _ancho;
	};*/
}

#endif // !_MATRIZESPARSA_H_
