
/***************************************************

	Srinidhi Goud Myadaboyina
	N12449860	
	Lab 3


***************************************************/
///g++ -std=c++11 -c main.cpp Tasks.cpp Bank.cpp OptimRes.cpp
///g++ main.o Tasks.o Bank.o OptimRes.o
///./a.out input1.txt


#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map> 
#include <unordered_map>
#include <deque>
#include <utility> 
#include <iterator>
#include <sstream>
#include <algorithm>
#include <tuple>
using namespace std;
#include "Bank.h"
#include "OptimRes.h"
#include "Tasks.h"


int main(int argc, char* argv[]){
	////////////////////Reading the file////////////////////////////////
	ifstream inFile;
	inFile.open(argv[1]);
	if (!inFile) {
    	cerr << "Unable to open file datafile.txt";
    	exit(1);   // call system to stop
	}
	
	int numb_tasks = 0;
	int numb_resources = 0;
	int numb_units = 0;
	inFile>>numb_tasks;
	inFile>>numb_resources;
	Bank *bA = new Bank(numb_tasks,numb_resources);///object for bankers algorithm 
	OptimRes *oM = new OptimRes(numb_tasks,numb_resources);//object for OptimRes/fifo algorithm
	for(int i=0;i<numb_resources;i++){
		int x;
		inFile>>x;
		bA->maxRes[i] = x;
		oM->maxRes[i] = x;
	}
	vector<Tasks> tasks;
	for(int i=0;i<numb_tasks;i++) tasks.push_back(Tasks(i));//new objects for each individual task
	string line;
	while(getline(inFile,line)){
		if (line.empty()) continue;
		string activity;
		int Task_id;
		int delay;
		int resource_type;
		int number;
		istringstream iss(line);
		
		iss>>activity>>Task_id>>delay>>resource_type>>number;
		tuple<string,int,int,int,int> buf = make_tuple(activity,Task_id,delay, resource_type, number);
		tasks[Task_id-1].Act.push_back(buf);//read into tuples
	}
	inFile.close();
	cout<<"---------------------------------Bankers Algorithm------------------------------------"<<endl;
	for(int i=0;i<numb_tasks;i++) tasks[i].reset(i);//resetting the tasks (similar to initialising)
	bA->run(tasks);
	int total_time = 0;
	int wait_time = 0;
	float wait_per = 0; 
	////printing the performance details////////////
	for(int i=0;i<numb_tasks;i++){
		if(tasks[i].aborted){
			cout<<"Task"<<'\t'<<i+1<<'\t'<<"aborted"<<endl;
		}
		else {
			total_time+=tasks[i].finishTime;
			wait_time+=tasks[i].waitTime;
			cout<<"Task"<<'\t'<<i+1<<'\t'<<tasks[i].finishTime<<'\t'<<tasks[i].waitTime<<'\t'<<((float)tasks[i].waitTime/tasks[i].finishTime)*100<<" %"<<endl;
		}
	}
	cout<<"Total"<<'\t'<<" "<<'\t'<<total_time<<'\t'<<wait_time<<'\t'<<((float)wait_time/total_time)*100<<" %"<<endl;
	cout<<"---------------------------------Optimisitic Resource Manager------------------------------------"<<endl;
	for(int i=0;i<numb_tasks;i++) tasks[i].reset(i);//resetting the tasks (similar to initialising)
	oM->run(tasks);
	total_time = 0;
	wait_time = 0;
	wait_per = 0; 
	////printing the performance details////////////
	for(int i=0;i<numb_tasks;i++){
		if(tasks[i].aborted){
			cout<<"Task"<<'\t'<<i+1<<'\t'<<"aborted"<<endl;
		}
		else {
			total_time+=tasks[i].finishTime;
			wait_time+=tasks[i].waitTime;
			cout<<"Task"<<'\t'<<i+1<<'\t'<<tasks[i].finishTime<<'\t'<<tasks[i].waitTime<<'\t'<<((float)tasks[i].waitTime/tasks[i].finishTime)*100<<" %"<<endl;
		}
	}
	cout<<"Total"<<'\t'<<" "<<'\t'<<total_time<<'\t'<<wait_time<<'\t'<<((float)wait_time/total_time)*100<<" %"<<endl;
	return 0;
}