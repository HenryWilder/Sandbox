// Equivalent of std::vector

#pragma once
#include <malloc.h>
#include <stdio.h>
#include "common.h"
#include "mem.h"

namespace amy
{
	template<class Type>
	class DynArray 
	{
#define BYTES_FOR(count) (sizeof(Type) * (count))
#define START_SAFE printf("Testing safety...\n"); if (m_content != nullptr) { printf("Evaluated to be safe.\n");
#define END_SAFE } printf("Evaluated unsafe.\n");

		static Type* Request(int _count) { printf("Requesting %i elements.\n", _count); return (Type*)malloc(BYTES_FOR(_count)); }
		void Delete() { printf("Freeing memory...\n"); free((void*)m_content); printf("Memory freed. Nullifying...\n"); m_content = nullptr; printf("Sudccessfully destructed.\n"); }

	public:
		DynArray() : m_content(nullptr), m_size(0), m_capacity(0) {}; // Construct null

		template<int _sizeIn>
		DynArray(Type(*p)[_sizeIn]) : m_content(nullptr), m_size(0), m_capacity(0) { // Construct with stack array
			Reserve(_sizeIn);
			amy::MemCopy(m_content, p, BYTES_FOR(_sizeIn));
		};

		DynArray(const DynArray& p) { // Copy construct
			m_content = new Type[p.Size()];
			amy::MemCopy(m_content, p, BYTES_FOR(p.Size()));
		};

		~DynArray() { START_SAFE Free(); END_SAFE } // Destruct

		size_t Size() const { printf("Array is currently size %zu.\n", m_size); return m_size; }
		size_t Capacity() const { printf("Array is currently capacity %zu.\n", m_capacity); return m_capacity; }

		void Reserve(const size_t n) {
			printf("Reserving %zu elements. Current capacity is %zu; will be %zu.\n", n, Capacity(), n);
			Type* temp = new Type[m_capacity = n];
			START_SAFE
				amy::MemCopy(temp, m_content, BYTES_FOR(m_size = amy::Min(n, m_size)));
			Free();
			END_SAFE
			m_content = temp; // Give content temp's new pointer, not the value
			Size();
			Capacity();
		}

		Type& operator[](const size_t index) { return *(m_content + index); } // Returns by reference so the element can be modified

		void Push_Back(const Type& elem) {
			if (Size() == Capacity()) Reserve(Capacity() + 1); // Not enough space
			m_content[m_size++] = elem;
			printf("Element pushed back.\n");
		}
		void Push_Back(const Type&& elem) {
			if (Size() == Capacity()) Reserve(Capacity() + 1); // Not enough space
			m_content[m_size++] = elem;
			printf("Element pushed back.\n");
		}

		void Pop_Back() { m_size--; printf("Element popped back.\n"); }

		void shrink_to_fit() { Reserve(Size()); }

		void Insert(size_t _where, Type& element) { }

		void Free() { Size(); Capacity(); printf("Attempting to delete[] %p...\n", m_content); delete[] m_content; m_content = nullptr; printf("Memory freed with delete[].\n"); }

	private:
		Type* m_content;
		size_t m_size;
		size_t m_capacity;

#undef BYTES_FOR
#undef START_SAFE
#undef End_SAFE
	};
}
