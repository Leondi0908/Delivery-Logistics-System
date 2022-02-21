// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

#ifndef ExpandableHashMap_INCLUDED
#define ExpandableHashMap_INCLUDED
#include <iostream>

const int NUM_BUCKETS = 8;

using namespace std;

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Node
	{
		Node(const KeyType& key, const ValueType& value)
			: m_key(key), m_val(value) {}
		KeyType m_key;
		ValueType m_val;
		Node* next;
	};
	unsigned int getBucketNumber(const KeyType& key) const
	{
		unsigned int hasher(const KeyType & k); // prototype
		unsigned int h = hasher(key);
		return  (h % size_Buckets);
	}
	void clean_Up()
	{
		for (int i = 0; i < size_Buckets; i++)
		{
			Node* temp = m_Bucket[i];
			while (temp != nullptr)
			{
				Node* killMe = temp;
				temp = temp->next;
				delete killMe;
			}
		}
		delete[] m_Bucket;
	}
	Node** m_Bucket;
	double m_maximumLoadFactor;
	int m_size;
	int size_Buckets;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	if (maximumLoadFactor <= 0)
	{
		m_maximumLoadFactor = 0.5;
	}
	else
		m_maximumLoadFactor = maximumLoadFactor;
	size_Buckets = NUM_BUCKETS;
	m_Bucket = new Node * [size_Buckets];
	for (int i = 0; i < size_Buckets; i++)
		m_Bucket[i] = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	clean_Up();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	clean_Up();
	size_Buckets = NUM_BUCKETS;
	m_Bucket = new Node * [size_Buckets];
	for (int i = 0; i < size_Buckets; i++)
		m_Bucket[i] = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_size;  // Delete this line and implement this function correctly
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	// Check whether the key is in the map or not
	// If it exists, we update the value	
	if (find(key) != nullptr)
	{
		*find(key) = value;
		return;
	}

	double curLoadFactor = size() / (size_Buckets * 1.0);
	if (curLoadFactor >= m_maximumLoadFactor)
	{
		// Copy the previous data
		int prev_Buckets = size_Buckets;
		Node** prev_mBucket = m_Bucket;

		// Expand the bucket
		size_Buckets *= 2;
		m_Bucket = new Node * [size_Buckets];
		for (int i = 0; i < size_Buckets; i++)
			m_Bucket[i] = nullptr;

		// Copy one by one and directly delete the old map
		for (int i = 0; i < prev_Buckets; i++)
		{
			if (prev_mBucket[i] != nullptr)
			{
				Node* killMe;
				Node* temp = prev_mBucket[i];
				while (temp != nullptr)
				{
					// Adding the prev node in old hashmap to the new hashmap
					Node* newNode = new Node(temp->m_key, temp->m_val);
					newNode->next = m_Bucket[getBucketNumber(temp->m_key)];
					m_Bucket[getBucketNumber(temp->m_key)] = newNode;
					m_size++;

					// Deleting the prev node to prevent the memory leak;
					killMe = temp;
					temp = temp->next;
					delete killMe;
				}
			}

		}
		//delete the bucket pointer array
		delete[] prev_mBucket;
	}

	// Insert new node into bucket
	Node* newNode = new Node(key, value);
	newNode->next = m_Bucket[getBucketNumber(key)];
	m_Bucket[getBucketNumber(key)] = newNode;
	m_size++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	// Finding the suitable index using the hashmap formula 
	// and search the fit key through that bucket to return the value address
	// Otherwise return nullprt
	Node* temp = m_Bucket[getBucketNumber(key)];
	while (temp != nullptr) {
		if (key == temp->m_key) {
			ValueType* val = &(temp->m_val);
			return val;
		}
		temp = temp->next;
	}
	return nullptr;
}


#endif