#include "OptimRes.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple> 
#include <unordered_map>
#include <sstream>
using namespace std;
OptimRes::OptimRes(int taskC, int resC)
	:numb_tasks(taskC),numb_resources(resC)
{
	for(int i=0;i<numb_resources;i++) maxRes.push_back(0);
}

void OptimRes::run(vector<Tasks>& tasks){
	vector<int> avail_res(maxRes);
	unordered_map<int, vector<int> > allocated;
	vector<int> blocked;
	vector<int> running;
	int cycle = 0;
	cout<<"Cycle "<<cycle<<endl;
	///////since first step is always initialisation, do a generic single run on all tasks 
	for(int i=0;i<numb_tasks;i++){
		running.push_back(tasks[i].id);
		vector<int> buf(numb_resources,0);////initialise allocated hash map with zeros 
		allocated[tasks[i].id-1] = buf;
		cout<<"Task "<<tasks[i].id<<" initiated"<<endl;
		tasks[i].incIterator();///go to the next activity of each task
	}
	cycle++;
	cout<<endl<<"End of cycle "<<cycle<<endl<<endl;
	/////////creating a block list and a running list which are queues, where 'running' contains all that are not terminated, blocked, aborted
	////////and 'blocked' contains all that are blocked
	while(true){
		vector<int> temp;
		vector<int> temp_res(avail_res.size(),0);
		if(blocked.size()>0) cout<<"Blocked states: "<<endl;
		for(int i=0;i<blocked.size();i++){//loop over the entire 'blocked' queue
			tasks[blocked[i]-1].waitTime++;//increase the wait time


			if(tasks[blocked[i]-1].isResourceNotAvail(avail_res)){//if enough resources are not available remain blocked
				cout<<'\t'<<"Task "<<blocked[i]<<"'s request cannot be granted"<<endl;
				continue;
			}
			else {//oterhwise remove from the blocked and add it to a temp queue to be added at the top of running queue at the end this iteration
				string s;
				int n2,n3,n4;
				tie(ignore,ignore,n2,n3,n4) = tasks[blocked[i]-1].getNext();
				cout<<'\t'<<"Task "<<blocked[i]<<" requested "<<n3<<" for "<<n4<<" units"<<endl;
				allocated[blocked[i]-1][n3 - 1] += n4;
				avail_res[n3 - 1] -= n4; 
				tasks[blocked[i]-1].incIterator();
				tie(s,ignore,ignore,ignore,ignore) = tasks[blocked[i]-1].getNext();//to check for immediate termination after serving the task's request
				if(s == "terminate"){
					if(!tasks[blocked[i]-1].isSuspended()){//if delay time > 0 reduce it and add it to temp that is added to running in the end///notice the ! 'not' condition
						cout<<'\t'<<"Task "<<blocked[i]<<" terminated"<<endl;
						tasks[blocked[i]-1].finishTime = cycle+1;//update finish time
						for(int j=0;j<numb_resources;j++) avail_res[j] += allocated[blocked[i]-1][j];
					}
					else temp.push_back(blocked[i]);
				}
				else temp.push_back(blocked[i]);
				blocked[i] = 0;//done with it so make it to zero, to be erase in the end
			}
		}
		//Iterate over running queue
		for(int i=0;i<running.size();i++){
			tuple<string, int,int,int,int> current_act = tasks[running[i]-1].getNext();
			string s;
			int n2,n3,n4;
			tie(s,ignore,n2,n3,n4) = current_act;
			if(tasks[running[i]-1].isSuspended() ){//if delay time>0 reduce it by one and continue
				cout<<"Task "<<running[i]<<" delayed "<<tasks[running[i]-1].delay_time<<endl;
				if(s != "terminate") continue;
			}
			//initiate check
			if(s == "initiate"){
				cout<<"Task "<<running[i]<<" initiated"<<endl;
				tasks[running[i]-1].incIterator();
			}
			//request check
			else if(s == "request"){
				if(!tasks[running[i]-1].isResourceNotAvail(avail_res)){//if resources are not available, block it

					cout<<"Task "<<running[i]<<" requested "<<n3<<" for "<<n4<<" units"<<endl;
					allocated[running[i]-1][n3 - 1] += n4;
					avail_res[n3 - 1] -= n4; 
					tasks[running[i]-1].incIterator();
					tie(s,ignore,ignore,ignore,ignore) = tasks[running[i]-1].getNext();
				}
				else {//otherwise allocate resources
					cout<<"Task "<<running[i]<<"'s request cannot be granted (blocked)"<<endl;
					blocked.push_back(running[i]);
					running[i] = 0;
				}
			}
			//release alloacated resources into a temporary file to be added in the end
			else if(s == "release"){
				temp_res[n3 - 1] += n4; 
				allocated[running[i]-1][n3 - 1] -= n4;
				tasks[running[i]-1].incIterator();
				cout<<"Task "<<running[i]<<" released "<<n3<<" for "<<n4<<" units"<<endl;
				tie(s,ignore,ignore,ignore,ignore) = tasks[running[i]-1].getNext();
			}
			//terminate check, notice there is no else-if, so terminates in the same cycle if it is the immediate instruction for one of the above's
			if(s == "terminate"){
				if(!tasks[running[i]-1].notYetSuspended()){
					cout<<"Task "<<running[i]<<" terminated"<<endl;
					tasks[running[i]-1].finishTime = cycle+1;//update finish time
					for(int j=0;j<numb_resources;j++) avail_res[j] += allocated[running[i]-1][j];
					running[i] = 0;
				}
				
			}
		}
		for(int k=0;k<numb_resources;k++) avail_res[k]+=temp_res[k];// add the temporary allocated resources into the main resources
		vector<int>::iterator it = running.begin();
		running.insert(it,temp.begin(),temp.end());
		running.erase(remove_if(running.begin(),running.end(),[](int i){return i==0;}),running.end());//remove completed/aborted tasks from running list
		blocked.erase(remove_if(blocked.begin(),blocked.end(),[](int i){return i==0;}),blocked.end());//remove unblocked tasks from the block list
		while(running.size()==0 && blocked.size()!=0){//deadlock detection, check if all tasks are pending requests without delayed tasks
			bool abort = true;
			int minIdx=0;//task with the minimum id to aborted
			for(int i=0;i<blocked.size();i++){
				if(tasks[blocked[i]-1].isResourceNotAvail(avail_res)){
					minIdx = (blocked[i]<blocked[minIdx])?i:minIdx;
					continue;
				}
				else{
					abort = false;
					break;
				}
			}
			if(abort){//abort the task with minimum id
				for(int k=0;k<numb_resources;k++){
					avail_res[k]+= allocated[blocked[minIdx]-1][k];
					allocated[blocked[minIdx]-1][k] = 0;
				} 
				tasks[blocked[minIdx]-1].aborted = true;
				cout<<"Task "<<blocked[minIdx]<<" has been aborted"<<endl;
				blocked.erase(blocked.begin()+minIdx);
			} 
			else break;
		}
		cycle++;//counting cycles
		cout<<endl<<"End of Cycle "<<cycle<<endl<<endl;
		if(running.size()==0 && blocked.size()==0) break;

	}
	
}