#ifndef _VECTORVECTORIMP_H_
#define _VECTORVECTORIMP_H_

#include "../Include/IVectorImp.h"
#include <initializer_list>
#include <memory>
#include <utility>
#include <vector>

namespace MN {

	template<class T>
	class VectorVectorImp : public IVectorImp<T> {
	public:
		explicit VectorVectorImp(size_t size, const T &initValue) :
			IVectorImp<T>(size),
			_elements(size, initValue){
		}

		explicit VectorVectorImp(size_t size) :
			IVectorImp<T>(size),
			_elements(size) {
		}

		explicit VectorVectorImp(std::initializer_list<T> args) :
			IVectorImp<T>(args.size()),
			_elements(args) {
		}

		virtual ~VectorVectorImp() = default;

		//accessors
		const T at(size_t index) const override final {
			assert(_size == _elements.size() && isInRange(index));
			return _elements[index];
		}

		//setters
		void setValueAtIndex(const T &value, size_t index) override final {
			assert(_size == _elements.size() && isInRange(index));
			_elements[index] = value;
		}

		//other methosd
		std::unique_ptr<IVectorImp> clone() const override final {
			IVectorImp *copy = new VectorVectorImp(_size);
			size_t index = 0;
			for (auto it = _elements.begin(); it != _elements.end(); it++) {
				copy->setValueAtIndex(*it, index++);
			}

			return std::unique_ptr<IVectorImp>(copy);
		}

		size_t numberOfElements() const override final {
			return _elements.size();
		}

		//Iterator
		class VectorVectorImpConstIterator :public IVectorImp::IVectorImpConstIterator {
		public:
			typedef std::pair<size_t, T> IteratorReturnType;
			VectorVectorImpConstIterator(const VectorVectorImpConstIterator &other):
				_index(other._index),
				_it(other._it){
			}

			virtual ~VectorVectorImpConstIterator() {}

			IVectorImpConstIterator& operator=(const VectorVectorImpConstIterator &other) {
				_index = other._index;
				_it = other._it;
				return *this;
			}

			const IteratorReturnType operator*() const override final {
				return IteratorReturnType(_index, *_it);
			}

			IVectorImpConstIterator& operator++() override final {
				++_index;
				++_it;
				return *this;
			}

			std::unique_ptr<IVectorImpConstIterator> clone() const override final {
				return std::unique_ptr<IVectorImpConstIterator>(new VectorVectorImpConstIterator(*this));
			}

			friend VectorVectorImp;

			bool operator==(typename const IVectorImp<T>::IVectorImpConstIterator& it2) const override final {
				const VectorVectorImpConstIterator *that = dynamic_cast<const VectorVectorImpConstIterator*>(&it2);
				if (that)
					return _index == that->_index && _it == that->_it;
				else
					return false;
			}

			bool operator!=(typename const IVectorImp<T>::IVectorImpConstIterator& it2) const override final {
				return !(it2 == *this);
			}

		protected:
			VectorVectorImpConstIterator(typename const std::vector<T>::const_iterator &it, int index):
				_index(index),
				_it(it){
			}
		private:
			typename std::vector<T>::const_iterator _it;
			int _index;
		};

		//iterator methods
		std::shared_ptr<IVectorImpConstIterator> begin() const override final {
			return std::shared_ptr<IVectorImpConstIterator>(new VectorVectorImpConstIterator(_elements.cbegin(), 0));
		}

		std::shared_ptr<IVectorImpConstIterator> end() const override final {
			return std::shared_ptr<IVectorImpConstIterator>(new VectorVectorImpConstIterator(_elements.cend(), _elements.size()));
		}
		
	private:
		std::vector<T> _elements;

		VectorVectorImp(const IVectorImp &other) = delete;
		VectorVectorImp& operator=(const IVectorImp &other) = delete;
		VectorVectorImp(const VectorVectorImp &other) = delete;
		VectorVectorImp& operator=(const VectorVectorImp &other) = delete;
	};
}

#endif // !_VECTORVECTORIMP_H_
