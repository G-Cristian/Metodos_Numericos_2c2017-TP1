#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "../Include/IVectorImp.h"
#include <assert.h>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <math.h>
#include <memory>

namespace MN {

	template<class T>
	class Vector {
	public:
	/*	Vector(){
			//TODO: Remove this constructor.
			//It's just for finding an error.
			//I should add a breakpoint here and this who is calling this
			int test = 0;
			test++;
			int a = test;
			cout << "Vector constructor " << test << endl;
		}*/
		Vector(IVectorImp<T> *vectorImp) :
			_vectorImp(vectorImp) {
			assert(_vectorImp != nullptr);
		}

		Vector(IVectorImp<T> *vectorImp, std::initializer_list<T> args) :
			_vectorImp(vectorImp) {
			assert(_vectorImp != nullptr && args.size() <= _vectorImp->size());
			size_t i = 0;
			for (auto it = args.begin(); it != args.end(); it++) {
				setValueAtIndex(*it, i++);
			}
		}

		Vector(const Vector<T> &other);

		Vector(Vector<T> &&other) noexcept;

		~Vector() = default;

		//getters
		const T at(size_t index) const {
			return _vectorImp->at(index);
		}

		const T operator[](size_t index) const {
			return this->at(index);
		}

		const size_t size()const {
			return _vectorImp->size();
		}

		//setter
		void setValueAtIndex(const T &value, size_t index) {
			_vectorImp->setValueAtIndex(value, index);
		}

		//operators
		Vector<T>& operator=(const Vector<T> &other);
		Vector<T>& operator=(Vector<T> &&other) noexcept;
		Vector<T>& operator+=(const Vector<T> &other);
		Vector<T> operator+(const Vector<T> &other) const;
		Vector<T> operator-()const;
		Vector<T>& operator-=(const Vector<T> &other);
		Vector<T> operator-(const Vector<T> &other)const;
		T operator*(const Vector<T> &other)const;
		Vector<T> operator*(const T &value)const;
		template<class S>
		friend Vector<S> operator*(const S &value, const Vector<S> &vec);
		Vector<T> operator/(const T &value)const;

		//operations
		T norm() const;

		Vector<T>& normailize();

		bool isZero() const {
			bool zero = true;
			for (size_t i = 0; i < this->size() && zero; ++i) {
				zero = this->at(i) == T();
			}

			return zero;
		}
		T sumOfElements() const {
			T sum = T();
			for (size_t i = 0; i < this->size(); ++i) {
				sum += this->at(i);
			}

			return sum;
		}

		size_t numberOfElements() const{
			return _vectorImp->numberOfElements();
		}

		//helpers
		template<class S>
		friend void swap(Vector<S> &v1, Vector<S> &v2);

		//Iterator
		class VectorConstIterator {
		public:
			typedef std::pair<size_t, T> IteratorReturnType;
			VectorConstIterator(const VectorConstIterator &other):
				_it(other._it->clone().release()){
			}

			~VectorConstIterator() {}

			VectorConstIterator& operator=(const VectorConstIterator &other) {
				_it.reset(other._it->clone().release());
			}

			const IteratorReturnType operator*() const {
				return *(*_it);
			}

			VectorConstIterator& operator++() {
				++(*_it);
				return *this;
			}

			friend Vector;
			
			bool operator==(typename const Vector<T>::VectorConstIterator& it2) const {
				return (*(_it)) == (*(it2._it));
			}

			bool operator!=(typename const Vector<T>::VectorConstIterator& it2) const {
				return !((*this) == it2);
			}
		protected:
			VectorConstIterator(std::shared_ptr<typename IVectorImp<T>::IVectorImpConstIterator> &it) {
				_it = it;
			}

		private:
			std::shared_ptr<typename IVectorImp<T>::IVectorImpConstIterator> _it;
		};

		//iterator methods
		VectorConstIterator begin() const {
			return VectorConstIterator(_vectorImp->begin());
		}

		VectorConstIterator end() const{
			return VectorConstIterator(_vectorImp->end());
		}

	private:
		//helpers
		void applyToElements(std::function<T(size_t, T)> func);

		//members
		std::unique_ptr<IVectorImp<T> > _vectorImp;
	};

	template<class S>
	inline void swap(Vector<S> &v1, Vector<S> &v2) {
		using std::swap;

		swap(v1._vectorImp, v2._vectorImp);
	}

	template<class T>
	void Vector<T>::applyToElements(std::function<T(size_t, T)> function) {
		size_t size = this->size();
		for (size_t i = 0; i < size; ++i) {
			_vectorImp->setValueAtIndex(function(i, _vectorImp->at(i)), i);
		}
	}

	template<class T>
	Vector<T>::Vector(const Vector<T> &other){
#ifdef _DEBUG
		//std::cout << "Vector copy constructor." << std::endl;
#endif
		_vectorImp = std::move(other._vectorImp->clone());
	}

	template<class T>
	Vector<T>::Vector(Vector<T> &&other) noexcept:
		_vectorImp(std::move(other._vectorImp)) {
#ifdef _DEBUG
		//std::cout << "Vector move constructor." << std::endl;
#endif
	}

	//operators
	template<class T>
	Vector<T>& Vector<T>::operator=(const Vector<T> &other) {
#ifdef _DEBUG
		//std::cout << "Vector copy assigment operator." << std::endl;
#endif
		std::unique_ptr<IVectorImp<T> > copy = other._vectorImp->clone();
		_vectorImp.reset(copy.release());

		return *this;
	}
	
	template<class T>
	Vector<T>& Vector<T>::operator=(Vector<T> &&other) noexcept {
#ifdef _DEBUG
		//std::cout << "Vector move assigment operator." << std::endl;
#endif
		if (this != &other) {
			_vectorImp.reset();
			_vectorImp = std::move(other._vectorImp);
		}

		return *this;
	}
	
	template<class T>
	Vector<T>& Vector<T>::operator+=(const Vector<T> &other) {
		assert(size() == other.size());
		auto sumFunc = [&other](size_t index, T value) {
			return value + other.at(index);
		};
		this->applyToElements(sumFunc);
		return *this;
	}

	template<class T>
	Vector<T> Vector<T>::operator+(const Vector<T> &other) const {
		assert(size() == other.size());
		Vector<T> sum(*this);
		sum += other;
		return sum;
	}
	
	template<class T>
	Vector<T> Vector<T>::operator-() const {
		Vector<T> neg(*this);
		auto negFunc = [](size_t index, T value) {
			return  -value;
		};
		neg.applyToElements(negFunc);

		return neg;
	}

	template<class T>
	Vector<T>& Vector<T>::operator-=(const Vector<T> &other) {
		assert(size() == other.size());
		this->operator+=(-other);
		return *this;
	}
	
	template<class T>
	Vector<T> Vector<T>::operator-(const Vector<T> &other) const {
		assert(size() == other.size());
		return *this + (-other);
	}
	
	template<class T>
	T Vector<T>::operator*(const Vector<T> &other) const {
		assert(this->size() == other.size());
		size_t size = this->size();
		T value = 0.0;
		for (size_t i = 0; i < size; ++i) {
			value += (this->at(i)*other.at(i));
		}

		return value;
	}
	
	template<class T>
	Vector<T> Vector<T>::operator*(const T &value)const {
		Vector<T> ret(*this);
		auto prodFunc = [value](size_t index, T vecValue) {
			return  vecValue * value;
		};
		ret.applyToElements(prodFunc);

		return ret;
	}

	template<class S>
	inline Vector<S> operator*(const S &value, const Vector<S> &vec) {
		return vec*value;
	}

	template<class T>
	Vector<T> Vector<T>::operator/(const T &value) const {
		Vector<T> ret(*this);
		auto divFunc = [value](size_t index, T vecValue) {
			return  vecValue / value;
		};
		ret.applyToElements(divFunc);

		return ret;
	}

	//operations
	template<class T>
	T Vector<T>::norm() const {
		T aux = (*this)*(*this);
		return std::sqrt(aux);
	}

	template<class T>
	Vector<T>& Vector<T>::normailize() {
		*this = (*this) / norm();
		return *this;
	}

	typedef Vector<double> Vector3D;
}

#endif // !_VECTOR_H_
