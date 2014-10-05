/*
 * Combination.cpp
 *
 *  Created on: 30. 9. 2014
 *      Author: kuba
 */

#include "Combination.h"
#include "cstdlib"

using namespace std;

Combination::Combination() {
	step = 0;
	field = NULL;
	nodes = NULL;
}

Combination::~Combination() {
	if(field != NULL){
		delete(field);
		field = NULL;
	}

}

void Combination::initialize(int n, Node * nodes){
	this->nodes = nodes;
	this->field = new bool[n];
	for(int i = 0; i < n/2; i++){
		field[i] = false;
	}
	for(int i = n/2; i < n; i++){
		field[i] = true;
	}
}

void Combination::initialize(int n, Node * nodes, char * wrap){
	//not yet supported
	/*
	this->nodes = nodes;
	this->field = new bool[n];
	this->size = n;
	this->level = n - n/2; //nutnost ovìøit extrémní situace, viz. graf hvìzda
	this->initLevel();
	*/
}

bool Combination::next(){
	bool clf = true;
	for(int i = 0; i < level && clf; i++){
		clf &= field[i];
	}
	if(clf){
		level --;
		if(level == 1)return false;
		initLevel();
	}else{
		moveLevel();
	}
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
	int reset = i;
    for(i; !field[i]; i++);
    int resStart = --i;
    field[i] = false;
    for(i; i>resStart-(reset+1); i--){
    	field[i] = true;
    }
}
