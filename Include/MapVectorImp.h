#ifndef _MAPVECTORIMP_H_
#define _MAPVECTORIMP_H_

#include "../Include/IVectorImp.h"
#include <map>
#include <memory>

namespace MN {

	template<class T>
	class MapVectorImp :public IVectorImp<T> {
	public:
		MapVectorImp(size_t size) :
			IVectorImp<T>(size)
			/*,_elements is initialized by default*/{
		}
		virtual ~MapVectorImp() = default;

		//getters
		const T at(size_t index) const override final {
			assert(isInRange(index));
			auto it = _elements.find(index);
			if (it == _elements.end())
				return T();

			return it->second;
		}

		//setters
		void setValueAtIndex(const T &value, size_t index) override final {
			if (value == T()) {
				auto it = _elements.find(index);
				if (it != _elements.end()) {
					_elements.erase(it);
				}
			}
			else {
				_elements[index] = value;
			}
		}

		//other methosd
		std::unique_ptr<IVectorImp<T> > clone() const override final {
			IVectorImp<T> *copy = new MapVectorImp<T>(_size);
			size_t index = 0;
			for (auto it = _elements.begin(); it != _elements.end(); it++) {
				copy->setValueAtIndex(it->second, index++);
			}

			return std::unique_ptr<IVectorImp<T> >(copy);
		}

		size_t numberOfElements() const override final {
			return _elements.size();
		}

		//Iterator
		class MapVectorImpConstIterator :public IVectorImp::IVectorImpConstIterator {
		public:
			typedef std::pair<size_t, T> IteratorReturnType;
			MapVectorImpConstIterator(const MapVectorImpConstIterator &other) :
				_it(other._it) {
			}

			virtual ~MapVectorImpConstIterator() {}

			IVectorImpConstIterator& operator=(const MapVectorImpConstIterator &other) {
				_it = other._it;
				return *this;
			}

			const IteratorReturnType operator*() const override final {
				return IteratorReturnType(_it->first, _it->second);
			}

			IVectorImpConstIterator& operator++() override final {
				++_it;
				return *this;
			}

			std::unique_ptr<IVectorImpConstIterator> clone() const override final {
				return std::unique_ptr<IVectorImpConstIterator>(new MapVectorImpConstIterator(*this));
			}

			friend MapVectorImp;

			bool operator==(typename const IVectorImp<T>::IVectorImpConstIterator& it2) const override final {
				const MapVectorImpConstIterator *that = dynamic_cast<const MapVectorImpConstIterator*>(&it2);
				if (that)
					return _it == that->_it;
				else
					return false;
			}

			bool operator!=(typename const IVectorImp<T>::IVectorImpConstIterator& it2) const override final {
				return !(it2 == *this);
			}
		protected:
			MapVectorImpConstIterator(typename const std::map<size_t, T>::const_iterator &it) :
				_it(it) {
			}
		private:

			typename std::map<size_t, T>::const_iterator _it;
		};

		//iterator methods
		std::shared_ptr<IVectorImpConstIterator> begin() const override final {
			return std::shared_ptr<IVectorImpConstIterator>(new MapVectorImpConstIterator(_elements.cbegin()));
		}

		std::shared_ptr<IVectorImpConstIterator> end() const override final {
			return std::shared_ptr<IVectorImpConstIterator>(new MapVectorImpConstIterator(_elements.cend()));
		}

	private:
		std::map<size_t, T> _elements;

		MapVectorImp(const IVectorImp &other) = delete;
		MapVectorImp& operator=(const IVectorImp &other) = delete;
		MapVectorImp(const MapVectorImp &other) = delete;
		MapVectorImp& operator=(const MapVectorImp &other) = delete;
	};
}

#endif // !_MAPVECTORIMP_H_
