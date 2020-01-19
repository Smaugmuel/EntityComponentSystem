#pragma once
#include <vector>

/*
	Storage for elements assigned to a certain index.
	Elements will be stored sequentially and densely.
	Indices will be stored sparsely.
	Complexity is O(1) for all actions, excluding the vector operations
*/

template<typename T>
class SparseSet final
{
public:
	using IndexType = int;
	using value_type = T;

	SparseSet() = default;
	SparseSet(const SparseSet& other) = delete;
	~SparseSet() = default;

	SparseSet& operator=(const SparseSet& other) = delete;

	template<typename... TArgs>
	bool add(IndexType index, const TArgs& ... args)
	{
		// Invalid index
		if (index < 0)
		{
			return false;
		}

		expandToFit(index);

		// Element on this index already exists
		if (m_indexToElem[index] != -1)
		{
			return true;
		}

		addAndLinkElement(index, args...);

		return true;
	}
	bool remove(IndexType index)
	{
		// Remove only if index is valid and element exists
		if (!has(index))
		{
			return false;
		}

		// Indices which will be linked after removal of element
		const IndexType movedLinkIndex = m_elemToIndex.back();
		const IndexType movedElemIndex = m_indexToElem[index];

		// Move element and redirect links
		m_elements[movedElemIndex] = m_elements.back();
		m_elemToIndex[movedElemIndex] = movedLinkIndex;
		m_indexToElem[movedLinkIndex] = movedElemIndex;

		// Remove last element and remove links
		m_elements.pop_back();
		m_elemToIndex.pop_back();
		m_indexToElem[index] = -1;

		return true;
	}
	bool has(IndexType index) const
	{
		return ((index >= 0 && index < static_cast<IndexType>(m_indexToElem.size())) && m_indexToElem[index] != -1);
	}
	T* get(IndexType index)
	{
		return (has(index) ? &m_elements[m_indexToElem[index]] : nullptr);
	}
	size_t size() const noexcept
	{
		return m_elements.size();
	}
	
	std::vector<value_type>& getElements() noexcept
	{
		return m_elements;
	}

	const std::vector<IndexType>& getIndexToElem() const noexcept
	{
		return m_indexToElem;
	}
	const std::vector<IndexType>& getElemToIndex() const noexcept
	{
		return m_elemToIndex;
	}

	size_t getByteSize() const
	{
		size_t size = 0;
		size += sizeof(*this);
		size += sizeof(T) * m_elements.capacity();
		size += sizeof(IndexType) * (m_indexToElem.capacity() + m_elemToIndex.capacity());
		return size;
	}

private:
	void expandToFit(IndexType index)
	{
		size_t index_ = static_cast<size_t>(index);
		if (index_ >= m_indexToElem.size())
		{
			m_indexToElem.resize(index_ + 1, -1);
		}
	}

	template<typename ... TArgs>
	void addAndLinkElement(IndexType index, const TArgs&... args)
	{
		m_elements.emplace_back(args...);

		// Index is assumed to be valid
		m_indexToElem[index] = static_cast<IndexType>(m_elements.size()) - 1;
		m_elemToIndex.emplace_back(index);
	}

private:
	std::vector<value_type> m_elements;		// size = nr of elements
	std::vector<IndexType> m_elemToIndex;		// size = nr of elements
	std::vector<IndexType> m_indexToElem;		// size = highest index used
};