#include <string>
#include <iostream>
#include "gram.hh"
#include <ctime>
#include <cstdlib>

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

int hashValue(std::string key, int modulus) {
  int hashValue = 0;
  for (char c: key) {
    // Horner's method for computing the value.
    hashValue = (32*hashValue + charToInt(c)) % modulus;
  }
  return hashValue;
}

namespace gram {

  dict* build(void) {
    srand(time(0));
    dict* newD = new dict;
	newD->buckets= new bucket[32];
	newD->numBuckets=32;
    return newD;
  }

  std::string get(dict* D, std::string ws) {
    // try to find ws
	int index=hashValue(ws,D->numBuckets);//get the hash value of ws
	bucket* bucketP = &(D->buckets[index]);
	gram* current=bucketP->first;
	while(current->words!=ws && current->next!=nullptr){
		current=current->next;
	}
	//if we can't find ws
	if(current->words!=ws){
		return ".";
	}
	else{
		//count how many followers are there
		long count=1;
		follower* fol=current->followers;
		while(fol->next!=nullptr){
			fol=fol->next;
			count++;
		}
		//get a random number between 1 and count
		long index=(rand()%(count))+1;
		//return the follower
		fol=current->followers;
		for(long i = index;i>1;i--){
			fol=fol->next;
		}
		return fol->word;
	}

  }

  std::string get(dict* D, std::string w1, std::string w2) {
    return get(D,w1+" "+w2);
  }
             
  void add(dict* D, std::string ws, std::string fw) {
  	//create the follower
  	follower* newF = new follower; 
	newF->word=fw;
	newF->next=nullptr;
    // try to find ws
	int index=hashValue(ws,D->numBuckets);//get the hash value of ws
	bucket* bucketP = &(D->buckets[index]);
	gram* current=bucketP->first;
	//if there is nothing in the bucket
	if(current==nullptr){
		//std::cout << "nothing in the bucket[" << index << "]\n";
		gram* newG = new gram;
		newG->words=ws;
		newG->number=1;
		newG->followers=newF;
		newG->next=nullptr;
		bucketP->first=newG;
		return;
	}
	//find the word or find the last thing
	while(current->words!=ws && current->next!=nullptr){
		current=current->next;
	}
	// if find it, then add fw to the end
	if(current->words==ws){
		//std::cout << "find ws\n";
		//find the last follower
		follower* fol = current->followers;
		while(fol->next!=nullptr){
			fol=fol->next;
		}
		fol->next=newF;
	}
	// if can't find ws, create a new gram and add fw
  	else{
		//if(current->next==nullptr){
		//	std::cout << "current->next is null\n";
		//}
		//std::cout << "can't find ws\n";
		gram* newG = new gram;
		newG->words=ws;
		newG->number=1;
		newG->next=nullptr;
		newG->followers=newF;
		current->next=newG;
	}
  }
  
  void add(dict* D, std::string w1, std::string w2, std::string fw) {
     add(D,w1+" "+w2,fw);
  }
     
  void destroy(dict *D) {
    delete D;
  }

void print(dict* D){
	for(int i=0;i<D->numBuckets;i++){
		std::cout << "bucket[" << i << "]: \n";
		gram* currentG=D->buckets[i].first;
		//print the each gram and its followers
		while(currentG!=nullptr){
			std::cout << currentG->words << ": " ;
			follower* currentF= currentG->followers;
			//print every follower
			while(currentF!=nullptr){
				std::cout << currentF->word << ", ";
				currentF=currentF->next;
			}
			currentG=currentG->next;
			std::cout << std::endl;
		}
	}
}

} 

