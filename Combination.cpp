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
	for(int i = 0; i < n; i++){

	}
}

