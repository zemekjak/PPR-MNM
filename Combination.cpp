/*
 * Combination.cpp
 *
 *  Created on: 30. 9. 2014
 *      Author: kuba
 */

#include "Combination.h"
#include "cstdlib"
#include "iostream"
#include "cstring"

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

void Combination::initialize(int n, Node ** nodes, int minDeg, int parSize, int parNum){
	initialize(n, nodes, minDeg);
	step = parNum;
	next();
	step = size;
}

void Combination::initialize(int n, Node ** nodes, int minDeg){
	this->nodes = nodes;
	this->size = n;
	this->level = {minDeg>n/2?n-minDeg:n/2};
	if(field!=NULL){
		delete field;
	}
	this->field = new int[this->level];
	initLevel();
}

void Combination::initialize(int n, Node ** nod, int * wrap){
	level = wrap[0];
	step = wrap[1];
	if(field != NULL){
		delete field;
	}
	field = new int[level];
	this->nodes = nodes;
	size = n;
	memcpy(field, wrap+2*sizeof(int), level*sizeof(int));
}

bool Combination::next(){
	for(int i=0;i<step;i++){
		if(field[0]==(size-level)){
			level--;
			initLevel();
			if(level<2)return (false);
			return (true);
		}
		recMove(level-1);
	}
	return (true);
}

void Combination::initLevel(){

	for(int i = 0; i < level; i++){
		field[i] = i;
	}
}

void Combination::recMove(int i){
	if(field[i]==(size+i-level)){
		recMove(i-1);
		field[i] = field[i-1]+1;
		return;
	}
	field[i]++;
}

void Combination::print(){
	for(int i = 0; i < level; i++){
		cout<<field[i]<<" ";
	}
	cout<<endl;
}

bool Combination::test(){
	for(int i = 0; i < level; i++){
		for(int j = 0; j < nodes[field[i]]->getCountOfNeighbours();j++){
			for(int k = 0;k < level; k++){
				if(field[k] == nodes[field[i]]->getNeighbour(j)->getId()){
					return (false);
				}
			}
		}
	}
	return (true);
}

int Combination::getLevel(){
	return (level);
}

int * Combination::split(){
	int * out = new int[(level+2)];
	out[0]=level;
	out[1]=step*2;
	memcpy(out+2*sizeof(int),field,level*sizeof(int));
	next();
	step *= 2;
	return (out);
}
