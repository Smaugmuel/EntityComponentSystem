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
	bool add(IndexType index, const TArgs& ... args);
	bool remove(IndexType index);
	bool has(IndexType index) const;
	T* get(IndexType index);
	size_t size() const noexcept;
	
	std::vector<value_type>& getElements() noexcept;
	const std::vector<IndexType>& getIdxToElem() const noexcept;
	const std::vector<IndexType>& getElemToIdx() const noexcept;

	size_t getByteSize() const;

private:
	void expandToFit(IndexType index);

	template<typename ... TArgs>
	void addAndLinkElement(IndexType index, const TArgs&... args);

private:
	std::vector<value_type> m_elements;		// size = nr of elements
	std::vector<IndexType> m_elemToIdx;		// size = nr of elements
	std::vector<IndexType> m_idxToElem;		// size = highest index used
};

template<typename T>
template<typename... TArgs>
inline bool SparseSet<T>::add(IndexType index, const TArgs& ... args)
{
	// Invalid index
	if (index < 0)
	{
		return false;
	}

	expandToFit(index);

	// Element on this index already exists
	if (m_idxToElem[index] != -1)
	{
		return true;
	}

	addAndLinkElement(index, args...);

	return true;
}

template<typename T>
inline bool SparseSet<T>::remove(IndexType index)
{
	// Remove only if index is valid and element exists
	if (!has(index))
	{
		return false;
	}

	// Indices which will be linked after removal of element
	const IndexType movedLinkIdx = m_elemToIdx.back();
	const IndexType movedElemIdx = m_idxToElem[index];

	// Move element and redirect links
	m_elements[movedElemIdx] = m_elements.back();
	m_elemToIdx[movedElemIdx] = movedLinkIdx;
	m_idxToElem[movedLinkIdx] = movedElemIdx;

	// Remove last element and remove links
	m_elements.pop_back();
	m_elemToIdx.pop_back();
	m_idxToElem[index] = -1;

	return true;
}

template<typename T>
inline bool SparseSet<T>::has(IndexType index) const
{
	return ((index >= 0 && index < static_cast<IndexType>(m_idxToElem.size())) && m_idxToElem[index] != -1);
}

template<typename T>
inline T* SparseSet<T>::get(IndexType index)
{
	return (has(index) ? &m_elements[m_idxToElem[index]] : nullptr);
}

template<typename T>
inline size_t SparseSet<T>::size() const noexcept
{
	return m_elements.size();
}

template<typename T>
inline std::vector<T>& SparseSet<T>::getElements() noexcept
{
	return m_elements;
}

template<typename T>
inline const std::vector<typename SparseSet<T>::IndexType>& SparseSet<T>::getIdxToElem() const noexcept
{
	return m_idxToElem;
}

template<typename T>
inline const std::vector<typename SparseSet<T>::IndexType>& SparseSet<T>::getElemToIdx() const noexcept
{
	return m_elemToIdx;
}

template<typename T>
inline void SparseSet<T>::expandToFit(IndexType index)
{
	size_t idx = static_cast<size_t>(index);
	if (idx >= m_idxToElem.size())
	{
		m_idxToElem.resize(idx + 1, -1);
	}
}

template<typename T>
template<typename ...TArgs>
inline void SparseSet<T>::addAndLinkElement(IndexType index, const TArgs& ... args)
{
	m_elements.emplace_back(args...);

	// Index is assumed to be valid
	m_idxToElem[index] = static_cast<IndexType>(m_elements.size()) - 1;
	m_elemToIdx.emplace_back(index);
}

template<typename T>
inline size_t SparseSet<T>::getByteSize() const
{
	size_t size = 0;
	size += sizeof(*this);
	size += sizeof(T) * m_elements.capacity();
	size += sizeof(IndexType) * (m_idxToElem.capacity() + m_elemToIdx.capacity());
	return size;
}
