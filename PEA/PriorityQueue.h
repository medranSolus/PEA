#pragma once
#include <vector>
#include <tuple>
#include <memory>

///<summary>
/// Binary heap, type required to be tuple with compare value on position 1
///</summary>
template<typename T, bool up = true>
class PriorityQueue
{
	size_t sizeStep;
	size_t maxSize = sizeStep;
	size_t count = 0;
	T ** data = nullptr;
	
	void sortUp(size_t index)
	{
		if (!index)
			return;
		const size_t parent = (index - 1) / 2;
		if (std::get<1>(*data[parent]) > std::get<1>(*data[index]))
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
			if (up && (std::get<1>(*data[index]) > std::get<1>(*data[child]) && std::get<1>(*data[child]) <= std::get<1>(*data[child - 1])) ||
				!up && (std::get<1>(*data[index]) < std::get<1>(*data[child]) && std::get<1>(*data[child]) >= std::get<1>(*data[child - 1])))
			{
				std::swap(data[index], data[child]);
				return sortDown(child);
			}
			else if (up && (std::get<1>(*data[index]) > std::get<1>(*data[child - 1]) && std::get<1>(*data[child - 1]) <= std::get<1>(*data[child])) ||
				!up && (std::get<1>(*data[index]) < std::get<1>(*data[child - 1]) && std::get<1>(*data[child - 1]) >= std::get<1>(*data[child])))
			{
				std::swap(data[index], data[--child]);
				return sortDown(child);
			}
		}
		else if (--child < count && (up && std::get<1>(*data[child]) < std::get<1>(*data[index]) ||!up && std::get<1>(*data[child]) > std::get<1>(*data[index])))
			std::swap(data[index], data[child]);
	}

public:
	PriorityQueue(size_t size = 16) : sizeStep(size) {}
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

	inline size_t size() const { return count; }
	
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
		return T();
	}
		
};
