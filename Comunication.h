/* 
 * File:   Communication.h
 * Author: kuba
 *
 * Created on 25. listopad 2013, 16:34
 */

#ifndef COMMUNICATION_H
#define	COMMUNICATION_H

#include <mpi.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "globals.h"

using namespace std;

#define MSG_REQUEST_WORK 1000
#define MSG_WORK_REQUEST_DENIED 1001
#define MSG_WORK_REQUEST_ACCEPTED 1002
#define MSG_BEST_RESULT 2000
#define MSG_TERMINATE 3000
#define MSG_TOKEN 4000

int processId;
int processNumber;
int token = 0;
bool hasToken = false;
int workRequestOffset = 0;


double time() {
    return (MPI_Wtime());
}

void barier(){
#ifdef MPI_MSG_DEBUG
	cout << "Process:" << processId <<" is waiting for others."<<endl;
#endif
    MPI_Barrier(MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout << "Process:" <<processId <<": Waiting ended"<<endl;
#endif
}

void sendRefuse(int dest){ // odmitnuti poslani prace, protoze proces sam praci nema
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg to:"<<dest<<" tag:"<<MSG_WORK_REQUEST_DENIED<<endl;
#endif
    MPI_Send(NULL, 0, MPI_INT, dest, MSG_WORK_REQUEST_DENIED, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
}

void sendTerminate(){
    for(int i=0;i<processNumber;i++){
        if(i==processId)continue;
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" msg to:"<<i<<" tag:"<<MSG_TERMINATE<<endl;
#endif
        MPI_Send(NULL,0,MPI_INT, i, MSG_TERMINATE, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" msg send"<<endl; 
#endif
    }
}

void sendToken(){
#ifdef MPI_MSG_DEBUG
	cout<<"Process:"<<processId<<" msg to:"<<(processId+1)%processNumber<<" tag:"<<MSG_TOKEN<<endl;
#endif
	MPI_Send(&token,1,MPI_INT,(processId+1)%processNumber,MSG_TOKEN, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
	token = 0;
	hasToken = false;
}


void sendBest(){
    int * msg = new int[bestCount+1];
#ifdef MEM_DEBUG
    cout<<" Create: SB "<<msg<<" - "<<msg+bestCount<<endl;
#endif
    msg[0]=bestCount;
    combination->setLimit(bestCount);
#ifdef MEM_DEBUG
    cout<<" Copy: MI-SB "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<" to "<<msg+1<<" - "<<msg+bestCount<<endl;
#endif
    memcpy(msg+1,maxIndependence,bestCount*sizeof(int));
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg to:"<<(processId+1)%processNumber<<" tag:"<<MSG_BEST_RESULT<<endl;
#endif
    MPI_Send(msg, bestCount+1, MPI_INT, (processId+1)%processNumber, MSG_BEST_RESULT, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
#ifdef MEM_DEBUG
    cout<<" Delete: SB "<<msg<<" - "<<msg+bestCount<<endl;
#endif
    delete[] msg;
}

void sendWork(int dest){
    int * buf= combination->split();
    if(buf==NULL){
        sendRefuse(dest);
        return;
    }
    if((dest < processId) || (processId==0)){
    	token |= 1;
    }
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg to:"<<dest<<" tag:"<<MSG_WORK_REQUEST_ACCEPTED<<endl;
#endif
    MPI_Send(buf, buf[0]+2, MPI_INT, dest, MSG_WORK_REQUEST_ACCEPTED, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
#ifdef MEM_DEBUG
    cout<<" Delete: SW "<<buf<<" - "<<buf+buf[0]+1<<endl;
#endif
    delete[] buf;
}

void checkForWorkRequest(){
    int flag;
    MPI_Status status;
    int buf;
    while(true){
        MPI_Iprobe ( MPI_ANY_SOURCE, MSG_REQUEST_WORK, MPI_COMM_WORLD, &flag, &status );
        if(!flag)break;
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" Work msg from:"<<status.MPI_SOURCE<<" tag:"<<status.MPI_TAG<<endl;
#endif
        MPI_Recv(&buf,0,MPI_INT,status.MPI_SOURCE,status.MPI_TAG,MPI_COMM_WORLD,&status);
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" Work msg recieved"<<endl;
#endif
        sendWork(status.MPI_SOURCE);
    }
}

void checkForBestMsg(){
    int flag;
    int count;
    MPI_Status status;
    int * msg;
    bool improved=false;
    while(true){
        MPI_Iprobe ( MPI_ANY_SOURCE, MSG_BEST_RESULT, MPI_COMM_WORLD, &flag, &status );
        if(!flag)break;
        MPI_Get_count(&status, MPI_INT, &count);
        msg = new int[count];
#ifdef MEM_DEBUG
        cout<<" Create: GB "<<msg<<" - "<<msg+count-1<<endl;
#endif
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" Best msg from:"<<status.MPI_SOURCE<<" tag:"<<status.MPI_TAG<<endl;
#endif
        MPI_Recv(msg,count,MPI_INT,status.MPI_SOURCE,status.MPI_TAG,MPI_COMM_WORLD,&status);
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" Best msg recieved"<<endl;
#endif
        if(bestCount<msg[0]){
            bestCount=msg[0];
            if(maxIndependence==NULL){
#ifdef MEM_DEBUG
            	cout<<" Delete: MI "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<endl;
#endif
            	delete[] maxIndependence;
            }
            maxIndependence= new int[bestCount];
#ifdef MEM_DEBUG
            cout<<" Create: MI "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<endl;
            cout<<" Copy: MI-GB "<<msg+1<<" - "<<msg+bestCount<<" to "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<endl;
#endif
            memcpy(maxIndependence,msg+1,bestCount*sizeof(int));
            combination->setLimit(bestCount);
            improved=true;
        }
#ifdef MEM_DEBUG
        cout<<" Delete: GB "<<msg<<" - "<<msg+count-1<<endl;
#endif
        delete[] msg;
    }
    if(improved){
        sendBest();
    }
}

void checkForMsg(){
    checkForBestMsg();
    checkForWorkRequest();
}

int * getWork(){
	if(hasToken){
		sendToken();
	}
    MPI_Status status;
    int i=1;
    int * buf = new int[nodeCount];
#ifdef MEM_DEBUG
    cout<<" Create: GW "<<buf<<" - "<<buf+nodeCount-1<<endl;
#endif
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg to:"<<(processId+i)%processNumber<<" tag:"<<MSG_REQUEST_WORK<<endl;
#endif
    MPI_Send(NULL, 0, MPI_INT, (processId+i+workRequestOffset)%processNumber, MSG_REQUEST_WORK, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
    cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
    while(true){  // cekani jestli nekdo odpovi na zadost o praci
        MPI_Recv(buf, nodeCount+1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
#ifdef MPI_MSG_DEBUG
        cout<<"Process:"<<processId<<" msg from:"<<status.MPI_SOURCE<<" tag:"<<status.MPI_TAG<<endl<<"Process:"<<processId<<" msg recieved"<<endl;
#endif
        switch(status.MPI_TAG){
            case MSG_REQUEST_WORK:{     // nekdo me zada o praci -> zadnou nemam -> odmitnuti poslani prace
                sendRefuse(status.MPI_SOURCE);
            }break;
            case MSG_WORK_REQUEST_DENIED:{      // odmitli mi dat praci
                i++;
                if((i+workRequestOffset)%processNumber == 0){
                	i++;
                }
                // zadost nasledujicimu procesoru o praci
#ifdef MPI_MSG_DEBUG
                cout<<"Process:"<<processId<<" msg to:"<<(processId+i+workRequestOffset)%processNumber<<" tag:"<<MSG_REQUEST_WORK<<endl;
#endif
                MPI_Send(NULL, 0, MPI_INT, (processId+i+workRequestOffset)%processNumber, MSG_REQUEST_WORK, MPI_COMM_WORLD);
#ifdef MPI_MSG_DEBUG
                cout<<"Process:"<<processId<<" msg send"<<endl;
#endif
            }break;
            case MSG_WORK_REQUEST_ACCEPTED:{    // prisla prace
            	workRequestOffset += i;
                return (buf);
            }break;
            case MSG_TERMINATE:{        // dostal jsem prikaz k ukonceni -> koncim
#ifdef MEM_DEBUG
            	cout<<" Delete: GW "<<buf<<" - "<<buf+nodeCount-1<<endl;
#endif
                delete[] buf;
                return (NULL);
            }break;
            case MSG_TOKEN:{/*
            Dostal jsem token jsem:
            	process 0:
            		pokud je token èerný, pošlu ho znova dokola
            		pokud je bílý, posílám terminate a konèím
            	process i>0:
            		pokud jsem èerný, obarvuji token a zbavuji se èernoty
            		posílám token dál
            */	token |= buf[0];
            	if(processId == 0){
            		if(token == 0){
            			sendTerminate();
            	    	checkForBestMsg();
#ifdef MEM_DEBUG
            	    	cout<<" Delete: GW "<<buf<<" - "<<buf+nodeCount-1<<endl;
#endif
            	    	delete[] buf;
            	    	return (NULL);
            		}
            		token = 0;
            	}
                sendToken();
            }break;
            case MSG_BEST_RESULT:{      // prisel novy nejlepsi vysledek
                if(bestCount<buf[0]){
                    if(maxIndependence!=NULL){
#ifdef MEM_DEBUG
                    	cout<<" Delete: MI "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<endl;
#endif
                    	delete[] maxIndependence;
                    }
                    bestCount=buf[0];
                    maxIndependence = new int[bestCount];
#ifdef MEM_DEBUG
                    cout<<" Create: MI "<<maxIndependence<<" - "<<maxIndependence+(bestCount-1)*sizeof(int)<<endl;
                    cout<<" Copy: GW-MI "<<buf+1<<" - "<<buf+bestCount<<" to "<<maxIndependence<<" - "<<maxIndependence+bestCount-1<<endl;
#endif
                    memcpy(maxIndependence,buf+1,bestCount*sizeof(int));
                    combination->setLimit(bestCount);
                    sendBest();
                }
            }break;
                
        }
       
    }
}

void initialize(int argc,char **argv){
    MPI_Init(&argc, &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processNumber);
    if(processId == 0){
    	hasToken = true;
    }
}

void finalize(){
    MPI_Finalize();
}


#endif	/* COMMUNICATION_H */
