#ifndef _ABSTRACTMATRIX_H_
#define _ABSTRACTMATRIX_H_

#include "vector.h"
#include "VectorVecrorImp.h"

#include <assert.h>
#include <functional>
#include <memory>

namespace MN {
	template<class T>
	class AbstractMatrix {
	public:
		typedef Vector<T> RowType;
		typedef Vector<T> ColumnType;
		virtual ~AbstractMatrix() {};

		virtual const T atRowColumn(int row, int column) const = 0;
		virtual void setValueInRowColumn(const T &value, int row, int column) = 0;
		virtual const Vector<T> operator[](int rowIndex) const = 0;

		//getters
		int width() const { return _width; }
		int height() const { return _height; }

		virtual RowType getRow(int rowIndex) const = 0;
		virtual ColumnType getColumn(int columnIndex) const = 0;

		//operators
		std::unique_ptr<AbstractMatrix<T> > operator+(const AbstractMatrix<T> &other) const;
		std::unique_ptr<AbstractMatrix<T> > operator-(const AbstractMatrix<T> &other) const;
		virtual std::unique_ptr<AbstractMatrix<T> > operator*(const AbstractMatrix<T> &other) const;
		std::unique_ptr<AbstractMatrix<T> > operator*(const T& value) const;
		std::unique_ptr<AbstractMatrix<T> > operator/(const T& value) const;
		friend std::unique_ptr<AbstractMatrix<T> > operator*(const T& value, const AbstractMatrix<T> &matrix);

		//other operations
		Vector<T> rangeOfThreeElementsAtYX(int y, int x) const;
		void writeThreeElementsInRowColumn(int row, int column, const Vector<T> &elements);
		virtual std::unique_ptr<AbstractMatrix<T> > transpose() const;

		std::unique_ptr<AbstractMatrix<T> > submatrix(int topRow, int leftColumn, int bottomRow, int rightColumn) const;

		virtual T norm() const = 0;

		//helpers
		void swapValuesInPositions(int row1, int column1, int row2, int column2);

	protected:
		explicit AbstractMatrix(int height, int width) :_height(height), _width(width) {
		};

		AbstractMatrix<T>& operator=(const AbstractMatrix<T> &other) {
			_height = other._height;
			_width = other._width;

			return *this;
		}

		virtual AbstractMatrix<T>* createInstance(int height, int width) const = 0;
		virtual AbstractMatrix<T>* clone(const AbstractMatrix<T> &other) const = 0;

		void applyFunctionToElements(std::function < T(int, int, const T&)> funct);

		bool indexesInRange(int row, int column) const {
			return rowInRange(row) && columnInRange(column);
		};

		bool rowInRange(int row) const {
			return 0 <= row && row < _height;
		};

		bool columnInRange(int column) const {
			return 0 <= column && column < _width;
		};

		int _height;
		int _width;
	};

	///This method apllies a function to every element in the matrix
	///@param funct: represents the funtion to be apllied. It has the form T funct(int row, int column, const T& value)
	///where 'row' and 'column' are the coordinates of the values in the matrix for which the function is being apllied,
	///'value' is the value of the matrix in that position and returns the value of row,column modified by funct
	template<class T>
	void AbstractMatrix<T>::applyFunctionToElements(std::function<T(int, int, const T&)> funct) {
		for (int i = 0; i < _height; ++i) {
			for (int j = 0; j < _width; ++j) {
				setValueInRowColumn(funct(i, j, atRowColumn(i, j)), i, j);
			}
		}
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::operator+(const AbstractMatrix<T> &other)const {
		AbstractMatrix<T> *r = clone(other);
		auto sum = [=](int row, int column, const T &value) {
			return this->_matrix[row][column] + value;
		};
		r->applyFunctionToElements(sum);
		return std::unique_ptr< AbstractMatrix<T> >(r);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::operator-(const AbstractMatrix<T> &other)const {
		AbstractMatrix<T> *r = clone(other);
		auto minus = [=](int row, int column, const T &value) {
			return this->_matrix[row][column] - value;
		};
		r->applyFunctionToElements(minus);
		return std::unique_ptr< AbstractMatrix<T> >(r);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::operator*(const AbstractMatrix<T> &other)const {
		assert(_width == other.height());
		int m = _height;
		int n = _width;
		int s = other.width();

		AbstractMatrix<T> *ret = clone(other);
		T sum = 0;
		for (int r = 0; r < m; r++) {
			for (int c = 0; c < s; c++) {
				sum = 0;
				for (int k = 0; k < n; k++) {
					sum += (atRowColumn(r, k) * other.atRowColumn(k, c));
				}

				ret->setValueInRowColumn(sum, r, c);
			}
		}

		return std::unique_ptr< AbstractMatrix<T> >(ret);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::operator*(const T& value)const {
		AbstractMatrix<T> *r = clone(*this);
		auto timesScalar = [=](int  row, int column, const T &v) {
			return this->_matrix[row][column] * value;
		};
		r->applyFunctionToElements(timesScalar);
		return std::unique_ptr< AbstractMatrix<T> >(r);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::operator/(const T& value)const {
		assert(value != 0);
		AbstractMatrix<T> *r = clone(*this);
		auto divScalar = [=](int  row, int column, const T &v) {
			return this->_matrix[row][column] / value;
		};
		r->applyFunctionToElements(divScalar);
		return std::unique_ptr< AbstractMatrix<T> >(r);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > operator*(const T& value, const AbstractMatrix<T> &matrix) {
		return matrix*value;
	}

	template<class T>
	Vector<T> AbstractMatrix<T>::rangeOfThreeElementsAtYX(int y, int x) const {
		assert(indexesInRange(y, x));

		T a = (this->atRowColumn(y,x));
		T b = T();
		T c = T();

		if (x + 1 < _width) {
			b = (this->atRowColumn(y, x + 1));
			if (x + 2 < _width)
				c = (double)(this->atRowColumn(y, x + 2));
		}

		return Vector<T>(new VectorVectorImp<T>({ a, b, c }));
	}

	template<class T>
	void AbstractMatrix<T>::writeThreeElementsInRowColumn(int row, int column, const Vector<T> &elements) {
		assert(indexesInRange(row, column) && elements.size() == 3);

		this->setValueInRowColumn(elements[0], row, column);

		if (column + 1 < _width) {
			this->setValueInRowColumn(elements[1], row, column + 1);
			if (column + 2 < _width)
				this->setValueInRowColumn(elements[2], row, column + 2);
		}
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::transpose() const {
		AbstractMatrix<T>* ret = createInstance(_width, _height);

		for (int row = 0; row < _height; ++row) {
			for (int column = 0; column < _width; ++column) {
				ret->setValueInRowColumn(this->atRowColumn(row, column), column, row);
			}
		}

		return std::unique_ptr<AbstractMatrix<T> >(ret);
	}

	template<class T>
	std::unique_ptr<AbstractMatrix<T> > AbstractMatrix<T>::submatrix(int topRow, int leftColumn, int bottomRow, int rightColumn) const {
		assert(indexesInRange(topRow,leftColumn) && indexesInRange(bottomRow, rightColumn) && topRow <= bottomRow && leftColumn <= rightColumn);
		int subMatrixHeight = bottomRow - topRow + 1;
		int subMatrixWidth = rightColumn - leftColumn + 1;
		AbstractMatrix<T>* mat = createInstance(subMatrixHeight, subMatrixWidth);

		for (int i = 0; i < subMatrixHeight; i++) {
			for (int j = 0; j < subMatrixWidth; j++) {
				mat->setValueInRowColumn(atRowColumn(topRow + i, leftColumn + j), i, j);
			}
		}

		return std::unique_ptr<AbstractMatrix<T> >(mat);
	}

	template<class T>
	inline void AbstractMatrix<T>::swapValuesInPositions(int row1, int column1, int row2, int column2) {
		T aux = this->atRowColumn(row1, column1);
		this->setValueInRowColumn(this->atRowColumn(row2, column2), row1, column1);
		this->setValueInRowColumn(aux, row2, column2);
	}
}
#endif // !_ABSTRACTMATRIX_H_
