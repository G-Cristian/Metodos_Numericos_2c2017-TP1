#ifndef _IVECTORIMP_H_
#define _IVECTORIMP_H_

#include <assert.h>
#include <memory>
#include <utility>

namespace MN {

	template<class T>
	class IVectorImp {
	public:
		virtual ~IVectorImp() = default;

		//getters
		virtual const T at(size_t index) const = 0;
		size_t size() const { return _size; }
		virtual size_t numberOfElements() const = 0;

		//setters
		virtual void setValueAtIndex(const T &value, size_t index) = 0;

		//helpers
		bool isInRange(size_t n) const {
			return 0 <= n && n < _size;
		}

		//other methods
		virtual std::unique_ptr<IVectorImp> clone() const = 0;

		//Iterator
		class IVectorImpConstIterator {
		public:
			typedef std::pair<size_t, T> IteratorReturnType;

			virtual ~IVectorImpConstIterator() {}

			virtual const IteratorReturnType operator*() const = 0;
			virtual IVectorImpConstIterator& operator++() = 0;

			virtual std::unique_ptr<IVectorImpConstIterator> clone() const = 0;

			virtual bool operator==(typename const IVectorImp<T>::IVectorImpConstIterator&) const = 0;
			virtual bool operator!=(typename const IVectorImp<T>::IVectorImpConstIterator&) const = 0;
		protected:
			IVectorImpConstIterator() = default;
			IVectorImpConstIterator(const IVectorImpConstIterator &other) = default;
			IVectorImpConstIterator(IVectorImpConstIterator &&other) = delete;

			IVectorImpConstIterator& operator=(const IVectorImpConstIterator &other) = default;
			IVectorImpConstIterator& operator=(IVectorImpConstIterator &&other) = delete;
		};

		//iterator methods
		virtual std::shared_ptr<IVectorImpConstIterator> begin() const = 0;
		virtual std::shared_ptr<IVectorImpConstIterator> end() const = 0;

	protected:
		IVectorImp(size_t size) :
			_size(size) {
		}

		//members
		size_t _size;
	private:
		IVectorImp(const IVectorImp &other) = delete;
		IVectorImp& operator=(const IVectorImp &other) = delete;
	};
}

#endif // !_IVECTORIMP_H_
