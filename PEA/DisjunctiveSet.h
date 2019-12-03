#pragma once

class DisjunctiveSet
{
	class Node
	{
		long long up = 0;
		long long rank = 0;
	public:
		Node() {}
		Node(long long upNode, long long nodeRank);

		inline long long getUpNode() const { return up; }
		inline long long getRank() const { return rank; }

		inline void setUpNode(long long upNode) { up = upNode; }
		inline void setRank(long long nodeRank) { rank = nodeRank; }
		inline void incrementRank() { ++rank; }
	};
	Node * set = nullptr;
public:
	DisjunctiveSet(long long size) { set = new Node[size]; }

	long long findSet(long long vertexv);

	void makeSet(long long vertex);
	void unionSets(unsigned int start, unsigned int end);

	~DisjunctiveSet() { if (set) delete[] set; }
};