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
	void initialize(int n, Node ** nodes, int * wrap);
	//paralel direct init - set work acording to number of processes (n) and number of this process (0..n-1)
	void initialize(int n, Node ** nodes, int minDeg, int parSize, int parNumber);
	bool next();
	bool test();
	int * split();
	int msgLength();
	void print();
	int getLevel();
	int * getVals();
	void setLimit(int lim);

private:
	int * field;
	Node ** nodes;
	int step;
	int level;
	int size;
	int limit;
	void initLevel();
	void recMove(int i);


};

#endif /* COMBIN ATION_H_ */
