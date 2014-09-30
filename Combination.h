/*
 * Combination.h
 *
 *  Created on: 30. 9. 2014
 *      Author: kuba
 */

#ifndef COMBINATION_H_
#define COMBINATION_H_

class Combination {
public:
	Combination();
	~Combination();
	void initialize(int n, Node * nodes);
	void initialize(int n, Node * nodes, char wrap);
	bool next();
	bool test();
	char split();

private:
	bool * field;
	Node * nodes;
	int step;


};

#endif /* COMBIN ATION_H_ */
