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

//#include "MemManagment.h"
using namespace std;

Combination::Combination() {
	step = 1;
	level = 0;
	size = 0;
	limit = 0;
	field = NULL;
	nodes = NULL;
}

Combination::~Combination() {
	if(field != NULL){
#ifdef MEM_DEBUG
		cout<<" Delete: F "<<field<<endl;
#endif
		delete[] field;
		field = NULL;
	}

}

void Combination::initialize(int n, Node ** nodes, int minDeg, int parSize, int parNum){
	initialize(n, nodes, minDeg);
	step = parNum;
	next();
	step = parSize;
}

void Combination::initialize(int n, Node ** nodes, int minDeg){
	this->nodes = nodes;
	this->size = n;
	if(field!=NULL){
#ifdef MEM_DEBUG
		cout<<" Delete: F "<<field<<endl;
#endif
		delete[] field;
	}
	this->level = minDeg>n/2?n-minDeg:n/2;
	this->field = new int[this->level];
#ifdef MEM_DEBUG
	cout<<" Create: F "<<field<<" - "<<field+level-1<<endl;
#endif
	initLevel();
}

void Combination::initialize(int n, Node ** nod, int * wrap){
	if(field != NULL){
#ifdef MEM_DEBUG
		cout<<" Delete: F "<<field<<endl;
#endif
		delete field;
	}
	level = wrap[0];
	step = wrap[1];
	field = new int[level];
#ifdef MEM_DEBUG
	cout<<" Create: F "<<field<<" - "<<field+level-1<<endl;
#endif
	this->nodes = nodes;
	size = n;
#ifdef MEM_DEBUG
	cout<<" Copy: GW-F "<<wrap+2<<" - "<<wrap+level+1<<" to "<<field<<" - "<<field+level+1<<endl;
#endif
	memcpy(field, wrap+2, level*sizeof(int));
#ifdef MEM_DEBUG
	cout<<" Delete: GW "<<wrap<<" - "<<wrap+level+1<<endl;
#endif
	delete[] wrap;
}

bool Combination::next(){
	if(level<=limit)return (false);
	for(int i=0;i<step;i++){
		if(field[0]==(size-level)){
			level--;
			if(level<=limit)return (false);
			initLevel();
			continue;
		}
		recMove(level-1);
	}
	return (true);
}

void Combination::initLevel(){
	cout<<" Initializing level: "<<level<<" with step: "<<step<<endl;
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
		for(unsigned int j = 0; j < nodes[field[i]]->getCountOfNeighbours();j++){
			for(int k = 0;k < level; k++){
				if(field[k] == (int)nodes[field[i]]->getNeighbour(j)->getId()){
					return (false);
				}
			}
		}
	}
	if(limit<level){
		limit = level;
	}
	return (true);
}

int Combination::getLevel(){
	return (level);
}

int * Combination::split(){
	if(level == (limit+1)){
		return (NULL);
	}
	int * out = new int[(level+2)];
#ifdef MEM_DEBUG
	cout<<" Create: SW "<<out<<" - "<<out+level+1<<" "<<endl;
#endif
	out[0]=level;
	out[1]=step*2;
#ifdef MEM_DEBUG
	cout<<" Copy: F-SW "<<field+2<<" - "<<field+level+1<<" to "<<out<<" - "<<out+level+1<<endl;
#endif
	memcpy(out+2,field,level*sizeof(int));
	next();
	step *= 2;
	return (out);
}

int Combination::msgLength(){
	return (level+2);
}

int * Combination::getVals(){
	int * out = new int[level];
#ifdef MEM_DEBUG
	cout<<" Create: MI "<<out<<" - "<<out+level-1<<endl;
	cout<<" Copy: F-MI "<<field<<" - "<<field+level-1<<" to "<<out<<" - "<<out+level-1<<endl;
#endif
	memcpy(out,field,level*sizeof(int));
	return (out);
}

void Combination::setLimit(int lim){
	limit = lim;
}

int Combination::getStep(){
	return step;
}
