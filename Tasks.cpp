#include "Tasks.h"
#include <iostream>
#include <vector>
#include <string>
#include <tuple> 
using namespace std;
Tasks::Tasks(int i)//construtor
{
	iterator = 0;
	aborted = false;
	id = i+1;
	delay_time = 0;
	waitTime = 0;
}

void Tasks::reset(int i){//reset
	iterator = 0;
	id = i+1;
	finishTime = 0;
	aborted = false;
	delay_time = 0;
	waitTime = 0;
}

bool Tasks::hasNextActivity(){//if there exists a next activity
	if(iterator == Act.size()) return false;
	else return true;
}
tuple<string, int,int,int,int> Tasks::getNext(){
	return Act[iterator];
}

bool Tasks::isSuspended(){//check if the task is delayed and then reduce the delay time by 1
	if(delay_time>0){
		delay_time--;
		return true;
	}
	else return false;
}

bool Tasks::notYetSuspended(){//check if the task is delayed without reduction in the delay time
	if(delay_time>0){
		return true;
	}
	else return false;
}

bool Tasks::isResourceNotAvail(vector<int> res){//check for availability of resources
	int res_n;
	int res_type;
	tie(ignore,ignore,ignore,res_type,res_n) = Act[iterator];
	//cout<<"task "<<res[res_type-1]<<res_n<<endl;
	if(res[res_type-1] < res_n) return true;
	else return false;
}

void Tasks::incIterator(){//increase the inherent iterator to the next activity of the task
	iterator++;
	int d;
	tie(ignore,ignore,d,ignore,ignore) = Act[iterator];
	delay_time = d;
}
