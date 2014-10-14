/*
 * Combination.cpp
 *
 *  Created on: 30. 9. 2014
 *      Author: kuba
 */

#include "Combination.h"
#include "cstdlib"
#include "iostream"

using namespace std;

Combination::Combination() {
	step = 1;
	level = 0;
	size = 0;
	field = NULL;
	nodes = NULL;
}

Combination::~Combination() {
	if(field != NULL){
		delete [] field;
		field = NULL;
	}

}

void Combination::initialize(int n, Node ** nod, int minDeg){
	this->nodes = nod;
	this->field = new bool[n];
	this->size = n;
	this->level = {minDeg>n/2?n-minDeg:n/2}; //minimaln� stup� n�m ��k� pouze minimum uzl�, kter� mus�me zahodit
	initLevel();
}

void Combination::initialize(int n, Node ** nod, char * wrap){
	//not yet supported
	/*
	this->nodes = nodes;
	this->field = new bool[n];
	this->size = n;
	this->level = n - n/2; //nutnost ov��it extr�mn� situace, viz. graf hv�zda
	this->initLevel();
	*/
}

bool Combination::next(){
	int i = 0;
	while(field[i])i++;
	if(i == level){
		level --;
		if(level == 1)
			return (false);
		initLevel();
	}else{
		moveLevel();
	}
	return (true);
}

void Combination::initLevel(){
	for(int i = 0; i < size; i++){
		field[i] = i > size-level-1;
	}
}

void Combination::moveLevel(){
	int i;
	for(i = 0; field[i]; i++){
		field[i] = false;
	}
	int reset = i+1;
	while(!field[i])i++;
    field[i] = false;
    i--;
    for(int j = i; j  > (i-reset); j--){
    	field[j] = true;
    }
}

void Combination::print(){
	for(int i = 0; i < size; i++){
		cout<<field[i];
	}
	cout<<endl;
}

bool Combination::test(){
	for(int i = 0; i < this->size; i++){
		if(field[i]){
			for(int j=0;j < ((int)nodes[i]->getCountOfNeighbours()); j++){
				if(field[nodes[i]->getNeighbour(j)->getId()]){
					return (false);
				}
			}
		}
	}
	return (true);
}

int Combination::getLevel(){
	return level;
}
