#include "wait.h"
#include "dll.h"
#include "getblk.h"
#include<iostream>
#include<unistd.h>//sleep
#include<thread>
#include<mutex>
#include<signal.h>
#include<stdlib.h>//rand()
using namespace std;
int main()
{      
  srand(time(NULL));
  
  for(int i=0;i<4;i++)
  {
    FL.insert_at_start(0,0,0);
  }	
  cout<<"\n--------------------Initially Hash Queues :--------------\n";
  for(int i=0;i<4;i++)
  {       
    cout<<"hq "<<i<<" :  ";
    HQ[i].display();
    cout<<"\n";
  }
  cout<<"\n--------------------Initially Free List :-----------------\n";
  FL.display();
  cout<<"\n----------------------------------------------------------\n";
  thread t1(threadhandler,0);
  sleep(2);
  thread t2(threadhandler,0);  
  //sleep(2);
  //thread t3(threadhandle,0);
  t1.join();  
  t2.join();		
  // t3.join();
  return 0;
}
