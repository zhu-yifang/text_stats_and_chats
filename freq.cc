//
// freq.cc
//
// This implements the operations for the unordered dictionary data
// structure with the type `freq::dict*` for Project 1 of the Spring
// 2020 offering of CSCI 221.  Specifically, we define the functions
// for a bucket hash table that stores a collection of words and their
// integer counts.
//
// It is described more within the README for this project.
//
// The functions it defines include
//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
//
// The top four are implemented already, the other four need to be written.
//

#include <string>
#include <iostream>
#include <algorithm>
#include "freq.hh"


// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR CHOOSING HASH TABLE SIZE
//

// isPrime(n)
//
// Return whether or not the given integer `n` is prime.
//
bool isPrime(int n) {
  // Handle the obvious cases, including even ones.
  if ((n <= 2) || (n % 2 == 0)) {
    return (n == 2);
  }
  // Try several odd divisors.
  int d = 3;
  while (d*d <= n) {
    if (n % d == 0) {
      // It has a divisor. It's not prime.
      return false;
    }
    d += 2;
  }
  // No divisors. It's prime.
  return true;
}

// primeAtLeast(n)
//
// Return the smallest prime number >=n
//
int primeAtLeast(int n) {
  if (n <= 2) {
    return 2;
  }
  int p = 3;
  while (p < n || !isPrime(p)) {
    p += 2;
  }
  return p;
}

// * * * * * * * * * * * * * * * * * * * * * * *
//
// HELPER FUNCTIONS FOR COMPUTING THE HASH VALUE
//

// charToInt(c):
//
// Returns an integer between 0 and 31 for the given character. Pays
// attention only to letters, the contraction quote, "stopper" marks,
// and space.
//
//
int charToInt(char c) {
  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  } else if (c == '.') {
    return 27;
  } else if (c == '!') {
    return 28;
  } else if (c == '?') {
    return 29;
  } else if (c == '\'') {
    return 30;
  } else if (c == ' ') {
    return 31;
  } else {
    return 0;
  }
}

// hashValue(key,tableWidth):
//
// Returns an integer from 0 to tableWidth-1 for the given string
// `key`. This serves as a hash function for a table of size
// `tableWidth`.  
//
// This method treats the string as a base-32 encoding of the integer
// it computes, modulo `tableWidth`. It relies on `charToInt` defined
// just above.
//
int hashValue(std::string key, int modulus) {
  int hashValue = 0;
  for (char c: key) {
    // Horner's method for computing the value.
    hashValue = (32*hashValue + charToInt(c)) % modulus;
  }
  return hashValue;
}

// Help function for sort()
//
bool cmp(freq::entry a, freq::entry b){
	return a.count>b.count;
}

// * * * * * * * * * * * * * * * * * * * * * * *
//
// Operations on freq::dict, and other support functions.
//
namespace freq {

//    * `int hashValue(std::string,int)`: give the hash location for a key
//    * `freq::dict* freq::build(int,int)`: build a word count dictionary 
//    * `int freq::totalCount(freq::dict*)`: get the total word count
//    * `int freq::numKeys(freq::dict*)`: get number of words
//    * `void freq::increment(freq::dict*,std::string)`: bump a word's count 
//    * `int freq::getCount(freq::dict*,std::string)`: get the count for a word
//    * `freq::entry* freq::dumpAndDestroy(freq::dict*)`: get the word counts, sorted by frequency
//    * `void freq::rehash(freq::dict*)`: expand the hash table
  //destory(D)
  //destory the dictionary
  void destory(dict* D){
	//free the heap
	int bucketIndex=0;
	entry* nextEntry;
	bucket* bucketP;
	while(bucketIndex<D->numBuckets){
		bucketP=&(D->buckets[bucketIndex]);
		entry* current=bucketP->first;
		entry* nextEntry=current;
		while(nextEntry!=nullptr){
			nextEntry=current->next;
			delete current;
			current=nextEntry;
		}
	bucketIndex++;
	}
	D->numIncrements=0;
	D->numBuckets=0;
	D->numEntries=0;
	D->loadFactor=0;
	delete D->buckets;

  }
  // buildBuckets(howMany):
  //
  // Return an array of buckets of length `howMany`.
  //
  bucket* buildBuckets(int howMany) {
    bucket* bs = new bucket[howMany];
    for (int i=0; i<howMany; i++) {
      bs[i].first = nullptr;
    }
    return bs;
  }

  // build(initialSize,loadFactor):
  //
  // Build a word count dictionary that is roughly the given size, and
  // maintains the given load factor in its hash table.
  //
  dict* build(int initialSize, int loadFactor) {
    dict* newD = new dict;
    newD->numIncrements = 0;
    newD->numEntries    = 0;
    newD->loadFactor    = loadFactor; 
    newD->numBuckets    = primeAtLeast(initialSize);
    newD->buckets       = buildBuckets(newD->numBuckets);
    return newD;
  }

  // numKeys(D):
  //
  // Gives back the number of entries stored in the dictionary `D`.
  //
  int numKeys(dict* D) {
    return D->numEntries;
  }

  // totalCount(D):
  //
  // Gives back the total of the counts of all the entries in `D`.
  //
  int totalCount(dict* D) {
    return D->numIncrements;
  }


  // getCount(D,w):
  //
  // Gets the count associated with the word `w` in `D`.
  //
  int getCount(dict* D, std::string w) {
	int index=hashValue(w,D->numBuckets);
	bucket* bkt;
	entry* current;
	bkt=&(D->buckets[index]);
	current=bkt->first;
	while(current!=nullptr){
		if(current->word==w){
			return current->count;
		}
		else{
			current=current->next;
		}
	}
    return 0;
  }

  // rehash(D):
  //
  // Roughly doubles the hash table of `D` and places its entries into
  // that new structure.
  //
  void rehash(dict* D) {
    // create a roughly double sized array

	int newSize=primeAtLeast(2*D->numBuckets);

	bucket* newBuckets = buildBuckets(newSize);

	//move each entry
	int bucketIndex=0;
	int counter=0;
	entry* current;
	bucket* bucketP;
	while(bucketIndex<D->numBuckets){
		bucketP=&(D->buckets[bucketIndex]);
		entry* target=bucketP->first;
		entry* pre = target;
		while(target!=nullptr){
			//recalculate the hash value
			int hash=hashValue(target->word,newSize);
			//move the entry
			//if the bucket is empty
			if(newBuckets[hash].first==nullptr){
				newBuckets[hash].first=target;
				pre=target->next;
				target->next=nullptr;
			}
			//else find the end of the bucket
			else{
				entry* current2=newBuckets[hash].first;
				while(current2->next!=nullptr){
					current2=current2->next;
				}
				current2->next=target;
				pre=target->next;
				target->next=nullptr;
			}
			target=pre;
		}
	bucketIndex++;
	}

	//Delete the old array
	delete[] D->buckets;
	D->numBuckets=newSize;
	D->buckets=newBuckets;//replace 
  }

  // increment(D,w):
  //
  // Adds one to the count associated with word `w` in `D`, possibly
  // creating a new entry.
  //
  void increment(dict* D, std::string w) {
	D->numIncrements++;
	//check loadfactor
	if(D->numEntries/D->numBuckets>=D->loadFactor){
		rehash(D);
	}
	int index=hashValue(w,D->numBuckets);
	bool change= false;
	bucket* bkt;//the target bucket
	entry* current;//the entry
	entry* follower;
	bkt=&(D->buckets[index]);
	current=bkt->first;
	follower=current;

	

	//try to find the word
	while(current!=nullptr){
		if(current->word==w){
			current->count++;
			change=true;
			break;
		}
		else{
			follower=current;
			current=current->next;
		}
	}
	//if can't find the word, create it
	if(change==false){
		entry* newWord = new entry;
		newWord->word=w;
		newWord->count=1;
		newWord->next=nullptr;
		D->numEntries++;
		//if there is at least 1 entry
		if(follower!=nullptr){
			follower->next=newWord;
		}
		//if there is no entry
		else{
			bkt->first=newWord;
		}
	}

  }

  // dumpAndDestroy(D):
  //
  // Return an array of all the entries stored in `D`, sorted from
  // most to least frequent.
  //
  // Deletes all the heap-allocated components of `D`.
  //
  entry* dumpAndDestroy(dict* D) {
    // get out every entry
	entry* dumpArray = new entry[D->numEntries];
	int bucketIndex=0;
	int counter=0;
	entry* current;
	bucket* bucketP;
	while(bucketIndex<D->numBuckets){
		bucketP=&(D->buckets[bucketIndex]);
		current=bucketP->first;
		while(current!=nullptr){
			dumpArray[counter]=*current;
			current=current->next;
			counter++;
		}
	bucketIndex++;
	}
	// sort decresingly
	//std::sort(dumpArray,dumpArray+(D->numEntries),cmp);
	
	for(int i=0;i<D->numEntries;i++){
		int j=i;
		int max=j;
		while(j<D->numEntries){
			if(dumpArray[max].count<=dumpArray[j].count){
				max=j;
			}
			j++;
		}
		//swap
		entry temp;
		temp=dumpArray[i];
		dumpArray[i]=dumpArray[max];
		dumpArray[max]=temp;
	}
	
	// destory
	destory(D);
	return dumpArray;
  }


} // end namespace freq

