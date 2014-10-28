/*
 * main.cpp
 *
 *  Created on: 6. 10. 2014
 *      Author: kuba
 */

#include "cstdlib"
#include "iostream"
#include <fstream>
#include <cstring>
#include <climits>

#include "Combination.h"
#include "Comunication.h"
#include "File.h"
#include "globals.h"

using namespace std;

void getParameters(int argc, char** argv) {
    if (argc != 2) {
        throw "Zadejte nazev souboru reprezentujiciho graf.";
    }
    if (!File::fileExist(argv[1])) {
        throw "Soubor neexistuje.";
    }
    File::openFile(argv[1], inputFile);
}

int readCountOfNodes(ifstream& file) {
    char znak;
    int n = 0;
    file.get(znak);
    // nacteni poctu uzlu grafu
    while (file.good() && (znak != '\n')) {
    	if(znak == 13){
    		file.get(znak);
    		break;
    	}
        n = n*10 + (short)znak - 48;     // odecteni nuly '0' = 48
        //cout << (short)znak << " " << n << endl;
        file.get(znak);
    }
    return (n);
}

void readNodesFromFile(ifstream& file, Node** nodes, unsigned int nodeCount) {
    // nacteni a zpracovani matice sousednosti ze souboru
    char znak;
    for (unsigned int i=0; i < nodeCount; i++) {
        for (unsigned int j=0; j < nodeCount; j++) { // nacteni jednoho radku
            file.get(znak);
            if (file.fail()) {
                throw "Chyba cteni vstupniho souboru.";
            }
            if (znak != '0' && znak != '1') {
                throw "Neplatny obsah vstupniho souboru. 1";
            }
            if (znak == '1') {      // pridani souseda
                nodes[i]->addNeighbour(nodes[j]);   // pridani souseda primo uzlu
            }
        }
        file.get(znak);
        if (znak == 13) file.get(znak);
        if (znak != '\n') { // kontrola jestli jsem na konci radku
            throw "Neplatny obsah vstupniho souboru. 2";
        }
    }

    // Pokud je nastaven failbit a nen� konec souboru, do�lo k chyb� souboru.
    if (inputFile.fail() && !inputFile.eof()) {
        throw "Chyba cteni vstupniho souboru";
    }
}

void printNodes() {
    //if (processId == 0) {
        cout << ": Uzly:" << endl;
        for (unsigned int i=0; i < nodeCount; i++) {
            cout << nodes[i]->getId() << " -> ";
            for (unsigned int j = 0; j < nodes[i]->getCountOfNeighbours(); j++) {
                cout << nodes[i]->getNeighbour(j)->getId() << ", ";
            }
            cout << endl;
        }
        cout << endl;
//    }
//    else {
//        cout << processId << ": Uzly: Nacteny." << endl;
//    }
}

/**
 * Uvolneni veskere alokovane pameti.
 */
void cleanUp() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
    if (nodes != NULL) {
        for (unsigned int i=0; i<nodeCount; i++) {
            delete nodes[i];
        }
    }
    if (combination != NULL){
    	delete combination;
    }
    delete [] nodes;
//    finalize();
}

/**
 * Nacteni grafu ze souboru a priprava datovych struktur.
 */
void loadData() {
    // zjisteni poctu uzlu grafu - prvni radek vstupniho souboru
    nodeCount = readCountOfNodes(inputFile);
    cout  << ": Pocet uzlu grafu: " << nodeCount << endl;

    // alokace a priprava pole uzlu
    nodes = new Node*[nodeCount];
    for (unsigned int i=0; i < nodeCount; i++) {
    	nodes[i] = new Node(i);
    }

    readNodesFromFile(inputFile, nodes, nodeCount);
    printNodes();

}

void executeParalel(int argc, char ** argv){
	double startTime, stopTime;
	initialize(argc, argv);
	barier();
	startTime = time();
	getParameters(argc, argv);
	int * buf;
	combination = new Combination();
	loadData();
	nodeDegree = nodes[0]->getCountOfNeighbours();
	combination->initialize(nodeCount, nodes, nodeDegree,processNumber, processId);
	int i = 1;
	while(!finished){
		do{
			if(combination->test()){
				cout<<combination->getLevel()<<" - ";
				combination->print();
				bestCount = combination->getLevel();
				delete[] maxIndependence;
				maxIndependence = combination->getVals();
				sendBest();
				break;
			}
			if(i%100 == 0){//every 100 iteration check for messages
				checkForMsg();
			}
			i++;
		}while(combination->next());
		buf = getWork();
		if(buf != NULL){
			combination->initialize(nodeCount, nodes, buf);
		}else{
			break;
		}
	}
	barier();
	stopTime = time();
	finalize();
	if(processId == 0){
		cout << "Runtime: "<< stopTime-startTime << endl;
	}
}

void executeStandalone(int argc, char ** argv){
	getParameters(argc, argv);
	loadData();
	cout << endl;
	combination = new Combination();
	nodeDegree = nodes[0]->getCountOfNeighbours();
	combination->initialize(nodeCount, nodes, nodeDegree);
	do{
		if(combination->test()){
			cout<<combination->getLevel()<<" - ";
			combination->print();
			break;
		}
	}while(combination->next());
}

int main(int argc, char ** argv){
	try {
		//executeStandalone(argc, argv);
		executeParalel(argc,argv);
	}catch (const char * e) {
		cout <<"Chyba: " << e << endl;
	    cleanUp(); // uklid
	    return (1);
	}

    cleanUp(); // uklid
	return (0);

}

