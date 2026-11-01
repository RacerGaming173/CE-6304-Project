/* 
We can use stl container list as a double
ended queue to store the cache keys, with
the descending time of reference from front
to back and a set container to check presence
of a key. But to fetch the address of the key
in the list using find(), it takes O(N) time.
This can be optimized by storing a reference
(iterator) to each key in a hash map. 
*/

//#include <bits/stdc++.h> 
#include <list>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <ctime>
#include "lfu.h"
#include <string.h>
using namespace std; 

LFUCache::LFUCache(int n) {
	csize = n;
	hits = 0;	// "hits" records the number of cache hit
	total_hits = 0;
	calls = 0;	// "calls" records the the number of total calls 
	total_calls = 0;
	migration = 0;	// "migration" records the number of data that is not cached in the optane while calling and need to be migrated into Optane then
	total_migration = 0;

	readHits = 0; 
	writeHits = 0; 
	evictedDirtyPage = 0; 
	
	std::cout << "LFU Algorithm is used" << std::endl;
	std::cout << "Cache size is: " << csize <<  std::endl;

}

LFUCache::~LFUCache() {
	csize = 0;
	hits = 0;	// "hits" records the number of cache hit
	total_hits = 0;
	calls = 0;	// "calls" records the the number of total calls 
	total_calls = 0;
	migration = 0;	// "migration" records the number of data that is not cached in the optane while calling and need to be migrated into Optane then
	total_migration = 0;

	readHits = 0; 
	writeHits = 0; 
	evictedDirtyPage = 0; 
	accessType.clear(); 

	dq.clear();
	dq_freq.clear();
	ma.clear();
}

void LFUCache::refer(long long int x, string rwtype) {
	calls++;
	
	//accessType[x] = rwtype; 
	//total_calls++;
	// if reference is not cached 
	
	if (ma.find(x) == ma.end()) {
		// if cache is full
		if (dq.size() == csize) {
			long long int min_key = dq.back();
			long long int min_freq = dq_freq[min_key];
			// scan from back to front, so ties go to the least recently used key
			for (std::list<long long int>::reverse_iterator xi = dq.rbegin(); xi != dq.rend(); xi++) {
				if (dq_freq[*xi] < min_freq) {
					min_freq = dq_freq[*xi];
					min_key = *xi;
				}
			}
			// evict the least used key, "last" is the key that is least used
			long long int last = min_key;
			// evict the least used key from std::list<int> dp by its corresponding iterator
			dq.erase(ma[last]);
			// evict the least used key and its iterator from unordered_map<int, std::list<int>::iteratr> ma by key
			ma.erase(last);

			if(accessType[last] == "Write"){
				evictedDirtyPage++;				
			}
			
		}
		// if reference is not cached, then it must be migrated into Optane cache
		//migration++;
		//total_migration++;
		accessType[x] = rwtype; 
	}
	// if reference is cached 
	else {
		hits++;
		// evict the reference from dp by its corresponding iterator
		dq.erase(ma[x]);
		if(rwtype == "Read"){
			readHits++;
			//total_hits++;
		} else {
			writeHits++;
			accessType[x] = "Write";
		}
	}
    
	// update frequency count
	dq_freq[x]++;
	
	// update the cache table by inserting the new reference into the front of dp
	dq.push_front(x);
	ma[x] = dq.begin();

}

void LFUCache::display() {
	// print the cached key after program terminate 
	for (std::list<long long int>::iterator xi = dq.begin(); xi != dq.end(); xi++) {
		std::cout << *xi << " ";
	}
	std::cout << std::endl;
}

void LFUCache::cachehits() {
	// print the number of total cache calls, hits, and data migration size
	//std::cout << "LFU Algorithm Summary " << std::endl;
	//std::cout << "the number of cache hits is: " << hits << std::endl;
	//std::cout << "the number of total calls is " << calls << std::endl;
    	//std::cout<<hits<<","<<calls<<","<<float(hits)/calls<< std::endl;
	//std::cout << "the data migration size into the optane is: " << ((double)migration) * 16 / 1024/ 1024 << "GB" << std::endl;
	std::cout<< "calls: " << calls << ", hits: " << hits << ", readHits: " << readHits << ", writeHits: " <<  writeHits << ", evictedDirtyPage: " << evictedDirtyPage << std::endl;


	std::ofstream result("ExperimentalResult.txt", std::ios_base::app);
	if (result.is_open()) { 
		result <<  "LFU " << "CacheSize " << csize << " calls " << calls << " hits " << hits << " hitRatio " << float(hits)/calls << " readHits " << readHits << " readHitRatio " << float(readHits)/calls << " writeHits " << writeHits << " writeHitRatio " << float(writeHits)/calls << " evictedDirtyPage " << evictedDirtyPage << "\n" ;			
	}
	result.close();
}

void LFUCache::refresh(){
	//when a new query is start, reset the "calls", "hits", and "migration" to zero
	calls = 0;
	hits = 0;
	migration = 0;
}

void LFUCache::summary() {
	// print the number of total cache calls, hits, and data migration size
	std::cout << "the total number of cache hits is: " << total_hits << std::endl;
	std::cout << "the total number of total refered calls is " << total_calls << std::endl;
	std::cout << "the total data migration size into the optane is: " << ((double)total_migration) * 16 / 1024/ 1024 << "GB" << std::endl;

}

/*
complie the code in Ubuntu
g++ -std=c++11 lfu.cpp
*/
