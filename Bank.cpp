#include "Bank.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple> 
#include <unordered_map>
#include <sstream>
using namespace std;
Bank::Bank(int taskC, int resC)//constructor
	:numb_tasks(taskC),numb_resources(resC)
{
	for(int i=0;i<numb_resources;i++) maxRes.push_back(0);
}

bool Bank::isClaimable(vector<int>& avail_res,int idx){ ////////////checks to see max-additional resources can be satisified by the available resources
	for(int i=0;i<numb_resources;i++){
		if(claim[idx-1][i]-allocated[idx-1][i] <= avail_res[i]) continue;
		else return false;
	}
	return true;
}

void Bank::resClaim(vector<int>& avail_res, int idx){//////////////claim the previously allocated task resources as a way of restoring it
	for(int i=0;i<numb_resources;i++){
		avail_res[i]-= allocated[idx-1][i];
	}
}

void Bank::resReClaim(vector<int>& avail_res, int idx){////////////////////release the resources for the considered task for bankers algorithm
	for(int i=0;i<numb_resources;i++){
		avail_res[i]+= allocated[idx-1][i];
	}
}

bool Bank::Banker(vector<Tasks>& tasks, int idx, vector<int>& avail_res, vector<int> running){///////bankers algorithm//recursive dynamic program with back tracking
	bool buf = false;
	int temp_id;
	int n3,n4;
	/////for the first call, first consider the request and allocate resources
	if(idx>0){
		tie(ignore,ignore,ignore,n3,n4) = tasks[idx-1].getNext();
		allocated[idx-1][n3 - 1] += n4;
		avail_res[n3 - 1] -= n4;
		temp_id = idx;
		idx = -1;
	}
	running.erase(remove_if(running.begin(),running.end(),[](int i){return i==0;}),running.end());////remove the tasks that have been terminated in the parent call
	if(running.size()==0) return true;
	for(int i=0;i<running.size();i++){
		if(isClaimable(avail_res,running[i])){
			resReClaim(avail_res,running[i]);/////release resources
			int temp = running[i];
			running[i]=0;
			buf = Banker(tasks,0,avail_res,running);/////recursively call for the rest of the tasks
			running[i] = temp;
			resClaim(avail_res,running[i]);////claim back the resources
			if(buf) break;///if there exists even one such order break
		}
	}
	/////for the first call, deallocate the resources
	if(idx<0){
		idx = temp_id;
		allocated[idx-1][n3 - 1] -= n4;
		avail_res[n3 - 1] += n4;
	}
	return buf;
}





void Bank::run(vector<Tasks>& tasks){
	vector<int> avail_res(maxRes);
	vector<int> blocked;
	vector<int> running;
	int cycle = 0;
	cout<<"Cycle "<<cycle<<endl;
	///////since first step is always initialisation, do a generic single run on all tasks 
	for(int i=0;i<numb_tasks;i++){
		string s;
		int n2,n3,n4;	
		tie(ignore,ignore,ignore,n3,n4) = tasks[i].getNext();
		if(avail_res[n3-1]<n4){////verify the claim with resources 
			tasks[i].aborted = true;
			cout<<"Task "<<tasks[i].id<<"'has been aborted as it claimed more than is available"<<endl;

			continue;
		}
		running.push_back(tasks[i].id);
		allocated[tasks[i].id-1] = vector<int>(numb_resources,0);////initialise allocated hash map with zeros 
		claim[tasks[i].id-1] = vector<int>(numb_resources,0);
		claim[tasks[i].id-1][n3-1] = n4;////initialise claim hash map with the respective claims
		cout<<"Task "<<i+1<<" initiated"<<endl;
		tasks[i].incIterator();///go to the next activity of each task
	}
	cycle++;
	cout<<endl<<"End of cycle "<<cycle<<endl<<endl;
	/////////creating a block list and a running list which are queues, where 'running' contains all that are not terminated, blocked, aborted
	////////and 'blocked' contains all that are blocked
	while(true){
		vector<int> temp;
		vector<int> temp_res(avail_res.size(),0);
		if(blocked.size()>0) cout<<"Blocked states:"<<endl;
		for(int i=0;i<blocked.size();i++){//loop over the entire 'blocked' queue
			vector<int> buffer;//a temporary buffer created to add the blocked task into running before checking for bankers algorithm safe state
			buffer.push_back(blocked[i]);
			vector<int>::iterator itt = buffer.end();
			buffer.insert(itt,running.begin(),running.end());//concatenate running at the end of the buffer
			
			tasks[blocked[i]-1].waitTime++;//increase the wait time
			string s;
			int n2,n3,n4;
			tie(ignore,ignore,n2,n3,n4) = tasks[blocked[i]-1].getNext();//extract the next activity of the corresponding task

			if(tasks[blocked[i]-1].isResourceNotAvail(avail_res)){//if enough resources are not available remain blocked
				cout<<'\t'<<"Task "<<blocked[i]<<"'s request cannot be granted as resource is not available"<<endl;
			}
			
			else if(!Banker(tasks,blocked[i],avail_res,buffer) ){//if Banker safe state check fails remain blocked
				cout<<'\t'<<"Task "<<blocked[i]<<"'s request cannot be granted as it is not a safe state"<<endl;
			}
			else {//oterhwise remove from the blocked and add it to a temp queue to be added at the top of running queue at the end this iteration
				cout<<'\t'<<"Task "<<blocked[i]<<" requested "<<n3<<" resource for "<<n4<<" units"<<endl;
				allocated[blocked[i]-1][n3 - 1] += n4;
				avail_res[n3 - 1] -= n4; 
				tasks[blocked[i]-1].incIterator();
				tie(s,ignore,ignore,ignore,ignore) = tasks[blocked[i]-1].getNext();//to check for immediate termination after serving the task's request
				if(s == "terminate"){
					if(!tasks[blocked[i]-1].isSuspended()){//if delay time > 0 reduce it and add it to temp that is added to running in the end///notice the ! 'not' condition
						allocated[blocked[i]-1] = vector<int>(numb_resources,0);
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
				claim[tasks[i].id-1][n3-1] = n4; 
				tasks[running[i]-1].incIterator();
			}
			//request check
			else if(s == "request"){
				
				if(tasks[running[i]-1].isResourceNotAvail(avail_res)){//if resources are not available, block it
					cout<<"Task "<<running[i]<<"'s request cannot be granted as resource is not available "<<endl;
					blocked.push_back(running[i]);
					running[i] = 0;
				}
				else if(claim[running[i]-1][n3-1]<n4+allocated[running[i]-1][n3-1]){//if requests more than claim, abort the task
					cout<<"Task "<<running[i]<<"'has been aborted as it requested more than initial claim"<<endl;
					for(int k=0;k<numb_resources;k++){
						avail_res[k]+= allocated[running[i]-1][k];
						allocated[running[i]-1][k] = 0;
					} 
					tasks[running[i]-1].aborted = true;
					running[i] = 0;
				}
				else if(!Banker(tasks,running[i],avail_res,running)){//if banker's test fails, block it
					cout<<"Task "<<running[i]<<"'s request cannot be granted as it is not a safe state (blocked in running)"<<endl;
					blocked.push_back(running[i]);
					running[i] = 0;
				}
				else{//otherwise allocate resources
					cout<<"Task "<<running[i]<<" requested "<<n3<<" resource for "<<n4<<" units"<<endl;
					allocated[running[i]-1][n3 - 1] += n4;
					avail_res[n3 - 1] -= n4; 
					tasks[running[i]-1].incIterator();
					tie(s,ignore,ignore,ignore,ignore) = tasks[running[i]-1].getNext();
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
					allocated[running[i]-1] = vector<int>(numb_resources,0);
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
		cycle++;//counting cycles
		cout<<endl<<"End of Cycle "<<cycle<<endl<<endl;
		if(running.size()==0 && blocked.size()==0) break;

	}
	
}