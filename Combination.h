/*
 * Combination.h
 *
 *  Created on: 30. 9. 2014
 *      Author: kuba
 */

#ifndef COMBINATION_H_
#define COMBINATION_H_
#include "Node.h"


class Combination {
public:
	Combination();
	~Combination();
	void initialize(int n, Node ** nodes, int minDeg);
	void initialize(int n, Node ** nodes, char * wrap);
	bool next();
	bool test();
	char split();
	void print();
	int getLevel();

private:
	bool * field;
	Node * nodes;
	int step;
	int level;
	int size;
	void initLevel();
	void moveLevel();


};

#endif /* COMBIN ATION_H_ */
