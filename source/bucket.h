/* ========================================================================
$File: bucket.cpp
$Date: June 4th 2021 4:29 pm
$Creator: Husam Dababneh
$Description: Single Header Implementation for a bucket 
========================================================================*/
#ifndef BUCKET_H
#define BUCKET_H

template<typename T, U64 BucketCap>
struct Bucket {
	Bucket* next = nullptr;
	Bucket* prev = nullptr;
	U64 count = 0;
	U64 cap = BucketCap;
	T data[BucketCap];
	
	T& operator[](U64 index) {
		return data[index];
	}
	
	const T& operator[](U64 index) const {
		return data[index];
	}
	
};


template<typename T, U64 size>   
struct ItemLocation {
	Bucket<T, size>* bucket;
	U64 index;
	bool found;
	
};

template<typename T, U64 BucketCap>
struct List {
	using BucketType = Bucket<T, BucketCap>;
	using ItemLocationType = ItemLocation<T, BucketCap>;
	
	BucketType  _data;
	BucketType* currentBucket;
	
	List(): currentBucket(&_data) {}
	
	
	ItemLocationType  find(const T& data) const  {
		BucketType& currentLookUpBucket = _data;
		bool found = false;
		U64 index = 0;
		
		while (true) {
			for (U64 it = 0; it < currentLookUpBucket.count; it++) {
				if (currentLookUpBucket[it] == data) {
					index = it;
					found = true;
					goto finish;
				}
			}
			
			if (currentLookUpBucket.next)
				currentLookUpBucket = *currentLookUpBucket.next;
			else
				break;
		}
		finish:
		return {&currentLookUpBucket, index, found};
		
	}
	
	void insert(T data) {
		if (currentBucket->count > (currentBucket->cap - 1)){
			generate_bucket();
		}
		
		(*currentBucket)[currentBucket->count] = data;
		
	}
	
	void generate_bucket() {
		BucketType* new_bucket = new BucketType;
		new_bucket->prev = currentBucket;
		currentBucket->next = new_bucket;
		currentBucket = new_bucket;
	}
	
};



#endif //BUCKET_H
