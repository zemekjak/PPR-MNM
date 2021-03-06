/* 
 * File:   globals.h
 * Author: kuba
 *
 * Created on 25. listopad 2013, 16:32
 */

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <climits>
#include "Node.h"
#include "Combination.h"
  
unsigned int            nodeCount = 0;
unsigned int			nodeDegree = 1;
Node**                  nodes = NULL;
ifstream                inputFile;
int                     bestCount = 1;
int*                    maxIndependence = NULL;
bool                    finished = false;
Combination * 			combination = NULL;

#endif	/* GLOBALS_H */

