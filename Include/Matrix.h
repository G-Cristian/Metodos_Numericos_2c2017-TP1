#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "AbstractMatrix.h"
#include "vector.h"
#include "VectorVecrorImp.h"

#include <assert.h>
#include <memory>
#include <vector>

namespace MN {
	template<class T>
	class Matrix :public AbstractMatrix<T> {
	public:
		typedef AbstractMatrix<T>::RowType RowType;
		typedef AbstractMatrix<T>::ColumnType ColumnType;

		typedef std::vector<RowType> MatrixType;

		explicit Matrix(int height, int width) :
			AbstractMatrix<T>(height, width),
			_matrix(height, RowType(new VectorVectorImp<T>(width))){
		}

		explicit Matrix(int height, int width, const T &initValue) :
			AbstractMatrix<T>(height, width),
			_matrix(height, RowType(new VectorVectorImp<T>(width, initValue))) {
		}

		explicit Matrix(int height, int width, const T *m):
			AbstractMatrix<T>(height, width),
			_matrix(height, RowType(new VectorVectorImp<T>(width))) {

			for (int i = 0; i < _height; i++) {
				for (int j = 0; j < _width; j++) {
					_matrix[i].setValueAtIndex(m[i*_width + j], j);
				}
			}
		}

		explicit Matrix(const std::vector<Vector<T> > &rows) :
			AbstractMatrix<T>(rows.size(), ((rows.size() > 0) ? rows[0].size() : 0 )),
			_matrix(rows) {
		}

		Matrix(const AbstractMatrix<T> &other) :
			AbstractMatrix<T>(other.height(), other.width()),
			_matrix(other.height(), RowType(new VectorVectorImp<T>(other.width()))) {

			auto copy = [&other](int row, int column, const T &value) {
				return other.atRowColumn(row, column);
			};
			this->applyFunctionToElements(copy);
		}

		Matrix(const Matrix<T> &other) :
			AbstractMatrix<T>(other._height, other._width),
			_matrix(other._matrix) {
		}

		Matrix(Matrix<T> &&other) noexcept :
			AbstractMatrix<T>(other._height, other._width){
			_matrix.reserve(other._height);
			auto end = std::make_move_iterator(other._matrix.end());
			for (auto it = std::make_move_iterator(other._matrix.begin()); it != end; it++) {
				_matrix.push_back(*it);
			}

			cout << "Matrix move constructor" << endl;
		}

		virtual ~Matrix() {};

		RowType getRow(int rowIndex) const override final {
			assert(rowInRange(rowIndex));
			return _matrix[rowIndex];
		}

		ColumnType getColumn(int columnIndex) const override final;

		RowType& getRow(int rowIndex) {
			assert(rowInRange(rowIndex));
			return _matrix[rowIndex];
		}

		const T atRowColumn(int row, int column) const override final {
			assert(indexesInRange(row, column));
			return _matrix[row][column];
		}

		template<class S>
		operator Matrix<S>() const {
			Matrix<S> other(_height, _width);
			for (int row = 0; row < _height; ++row) {
				for (int column = 0; column < _width; ++column) {
					other.setValueInRowColumn(static_cast<S>(this->atRowColumn(row, column)), row, column);
				}
			}

			return other;
		}

		const Vector<T> operator[](int rowIndex) const override final {
			assert(rowInRange(rowIndex));
			return _matrix[rowIndex];
		}

		Vector<T>& operator[](int rowIndex)  {
			assert(rowInRange(rowIndex));
			return _matrix[rowIndex];
		}

		void setValueInRowColumn(const T &value, int row, int column) override final {
			_matrix[row].setValueAtIndex(value, column);
		}

		Matrix<T>& operator=(const Matrix<T> &other) {
			if (&other != this) {
				AbstractMatrix<T>::operator=(other);
				_matrix = other._matrix;
			}
			return *this;
		}

		Matrix<T>& operator=(Matrix<T> &&other) noexcept{
			if (&other != this) {
				AbstractMatrix<T>::operator=(other);
				_matrix = std::move(other._matrix);
			}
			return *this;
		}

		//other operations
		std::unique_ptr<T> convertToPointer() const;

		T norm() const override final {
			assert(_matrix.size() == _height);
			Vector<T> vec(new VectorVectorImp<T>(_height));
			for (int i = 0; i < _height; ++i) {
				assert(_matrix[i].size() == 1);
				vec.setValueAtIndex(_matrix[i][0], i);
			}
			return vec.norm();
		}

		AbstractMatrix<T>* createInstance(int height, int width) const override final {
			return new Matrix<T>(height, width);
		}

		AbstractMatrix<T>* clone(const AbstractMatrix<T> &other) const override final {
			return new Matrix<T>(other);
		}

	private:
		MatrixType _matrix;
	};

	template<class T>
	std::unique_ptr<T> Matrix<T>::convertToPointer() const {
		T *ret = new T[_width*_height];

		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				ret[i*_width + j] = _matrix[i][j];
			}
		}

		return std::unique_ptr<T>(ret);
	}

	template<class T>
	Vector<T> Matrix<T>::getColumn(int columnIndex) const {
		assert(columnInRange(columnIndex));
		Vector<T> column(new VectorVectorImp<T>(_height));

		for (int i = 0; i < _height; ++i) {
			column.setValueAtIndex(_matrix[i][columnIndex], i);
		}

		return column;
	}
}

#endif // !_MATRIX_H_
