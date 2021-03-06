#pragma once
#include <vector>
#include <tuple>
#include <memory>

///<summary>
/// Binary heap, type required to be tuple with compare value on position 1
///</summary>
template<typename T, bool ascending = true>
class PriorityQueue
{
	static T none;
	size_t sizeStep;
	size_t maxSize = sizeStep;
	size_t count = 0;
	T ** data = nullptr;
	
	void sortUp(size_t index)
	{
		if (!index)
			return;
		const size_t parent = (index - 1) / 2;
		if (ascending && std::get<1>(*data[parent]) > std::get<1>(*data[index]) ||
			!ascending && std::get<1>(*data[parent]) < std::get<1>(*data[index]))
		{
			std::swap(data[index], data[parent]);
			return sortUp(parent);
		}
	}

	void sortDown(size_t index)
	{
		size_t child = 2 * index + 2;
		if (child < count)
		{
			if (ascending && (std::get<1>(*data[index]) > std::get<1>(*data[child]) && std::get<1>(*data[child]) <= std::get<1>(*data[child - 1])) ||
				!ascending && (std::get<1>(*data[index]) < std::get<1>(*data[child]) && std::get<1>(*data[child]) >= std::get<1>(*data[child - 1])))
			{
				std::swap(data[index], data[child]);
				return sortDown(child);
			}
			else if (ascending && (std::get<1>(*data[index]) > std::get<1>(*data[child - 1]) && std::get<1>(*data[child - 1]) <= std::get<1>(*data[child])) ||
				!ascending && (std::get<1>(*data[index]) < std::get<1>(*data[child - 1]) && std::get<1>(*data[child - 1]) >= std::get<1>(*data[child])))
			{
				std::swap(data[index], data[--child]);
				return sortDown(child);
			}
		}
		else if (--child < count &&
			(ascending && std::get<1>(*data[child]) < std::get<1>(*data[index]) ||
				!ascending && std::get<1>(*data[child]) > std::get<1>(*data[index])))
			std::swap(data[index], data[child]);
	}

public:
	constexpr PriorityQueue(size_t size = 16) : sizeStep(size) {}
	~PriorityQueue()
	{
		if (data)
		{
			for (size_t i = 0; i < count; ++i)
				if (data[i])
					delete data[i];
			free(data);
		}
	}

	constexpr size_t size() const { return count; }
	void sort()
	{
		for (long long i = (count - 2) / 2; i >= 0; --i)
			sortDown(i);
	}
	
	void push(T && value)
	{
		if (!data)
		{
			data = static_cast<T**>(calloc(maxSize, sizeof(T*)));
			data[0] = new T(std::move(value));
			count = 1;
		}
		else
		{
			if (count == maxSize)
			{
				maxSize += sizeStep;
				data = static_cast<T**>(realloc(data, maxSize * sizeof(T*)));
			}
			data[count] = new T(std::move(value));
			sortUp(count++);
		}
	}

	T top()
	{
		if (count)
		{
			T value = std::move(*data[0]);
			delete data[0];
			data[0] = data[--count];
			if (count + sizeStep < maxSize)
			{
				maxSize -= sizeStep;
				data = static_cast<T**>(realloc(data, maxSize * sizeof(T*)));
			}
			sortDown(0);
			return value;
		}
		return none;
	}	

	T & operator[](size_t at)
	{
		if (at < count)
			return *data[at];
		return none;
	}
};
template<typename T, bool ascending>
T PriorityQueue<T, ascending>::none = T();
