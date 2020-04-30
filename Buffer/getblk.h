#ifndef GETBLK_H
#define GETBLK_H
#include "dll.h"
#include "wait.h"
#include<iostream>
#include<unistd.h>
#include<thread>
#include<mutex>
using namespace std;

DLL HQ[4],FL;
node *temp=new node;
mutex mtx;
int r_hqnum;//to store hq num for scenario 5
int r;//scenario 5 block no
int ch5=0;//read or write for scenario5
void wait_for_specific();
void signal_for_specific();
void wait_for_any();
void signal_for_any();
void threadhandler(int);
void getBlock(int blockNum,int HQnum,int ch1);


void update_hashqueues(int blkno,int HQnum)
{ 
  temp=HQ[HQnum].getBuffer(blkno);
  temp->statusf_b=0;
} 

void brelse(int blkno,int HQnum)//frees a block from dll
{  
  sleep(3);
  temp=HQ[HQnum].getBuffer(blkno);	
  if(temp->status2==2)
  {
    if(!FL.isPresent(blkno))//old is deleted and new added 
	FL.insert_at_start(blkno,0,2);  
  }
  else
  {  
    if(!FL.isPresent(blkno)) 
	FL.insert_at_end(blkno,0,1);//free and vaild
  }
  
  update_hashqueues(blkno,HQnum);//if not call this fn then hash queue buffer status will remains busy always
}

void handle_write(int HQnum,int BlockNum)
{ 
  
  temp=HQ[HQnum].getBuffer(BlockNum);
  temp->status2=2;  //making status delayed write
  sleep(5); 
  brelse(BlockNum,HQnum);//free this now
}

void handle_async_write(int block_0)
{
 cout<<"\nAsynchronously writing buffer to disk for : "<<std::this_thread::get_id()<<endl;
 sleep(2);
 cout<<"\n\nAsynchronous Writing completed for : "<<std::this_thread::get_id()<<endl;

}


void threadhandler(int block_0)
{ 
  int blockNum,HQnum,ch1;
  mtx.lock();
  blockNum=(rand()%10)+1;
  cout<<"\nThread : "<<std::this_thread::get_id();  
  cout<<" Needs Block number :";
  //cin>>blockNum;
  cout<<blockNum<<endl;
  mtx.unlock();
  //sleep(2);
  mtx.lock();
  ch1=(rand()%2)+1;
  ch1==1?cout<<"Request is for Read":cout<<"Request is for Write";
  cout<<" by Thread: "<<std::this_thread::get_id()<<endl;    
  HQnum=blockNum%4;//Hash Function deciding which hq to refer
  //cout<<"hq number:"<<HQnum<<" referred by Thread: "<<std::this_thread::get_id()<<endl;     
  cout<<endl;
  mtx.unlock();    
  getBlock(blockNum,HQnum,ch1);
}

void getBlock(int blockNum,int HQnum,int ch1)//locks a buffer for given blocknumber
{ 
  
  int Insert_at_start,PrevTemp; 
  signal_for_any();//to handle  any buffer case if existing
  signal_for_specific();//to handle  specific buffer case if existing
 	
  Insert_at_start=0;
  lb:
      switch(ch1)
      {
      //sleep(1);
       case 2:
	
     	   if(HQ[HQnum].isPresent(blockNum))//blockNum Present in hq 
     	   { 
     	      temp=HQ[HQnum].getBuffer(blockNum);
	      if(temp->statusf_b==1)  //scenario 5(status is 'busy')
              {       
	        cout<<"\n--SCENARIO 5 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		 sleep(2);	        
		r=blockNum;
	        ch5=1;	
		r_hqnum=HQnum;	
	        wait_for_specific();     	       
	      }
	      else //scenario 1(status is 'free')
	      {		
	        temp->statusf_b=1;//change status to 'busy' and locks it    
	        mtx.lock();	       
	        if(FL.isPresent(blockNum)) 
	 	  FL.delete_by_value(blockNum);//status is changed to bsy		             
	        cout<<"\n--SCENARIO 1 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;	
	        sleep(2);
		cout<<"\nbuffer is locked for : "<<std::this_thread::get_id()<<endl;
		mtx.unlock();	        
		handle_write(HQnum,blockNum);	     		
	     }	
	  }
          else
          {
 	    sleep(1);
               if(FL.isEmpty())//scenario 4(no buffers left in FL)
	       {   
                 cout<<"\n--SCENARIO 4 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		 sleep(2);	 	
		 wait_for_any();		
	       }
	       temp=FL.delete_from_begin();
               if(temp->status2==2)//scenario 3(status is 'delayed write')
	       {                    
	         cout<<"\n--SCENARIO 3 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
 		 sleep(2);
		 thread async_thread(handle_async_write,0);
		 async_thread.join();
               
		 Insert_at_start=1;
		 PrevTemp=temp->data;
	         goto lb;
	       } 
	       else//scenario 2(Pick a free buffer from hq and insert at end of required hq) 
	       { 
		  mtx.lock(); 
     	          cout<<"\n--SCENARIO 2 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		  if(HQ[HQnum].isPresent(blockNum)) goto l12;
		  	HQ[HQnum].insert_at_end(blockNum,1,2);
		  mtx.unlock();
 			 l12:
		  cout<<"\nBuffer is locked for : "<<std::this_thread::get_id()<<endl;		  
		  handle_write(HQnum,blockNum);
         	}
           }
           if(Insert_at_start==1)
	   {  
             mtx.lock();
             FL.insert_at_start(PrevTemp,0,1);
	     HQ[PrevTemp%4].change_status2(PrevTemp,1); 
	     mtx.unlock();	
	   }	
	   break;
	   case 1:   
	     if(HQ[HQnum].isPresent(blockNum))//blockNum Present in hq 
       	     {	sleep(1);
           	temp=HQ[HQnum].getBuffer(blockNum);
	     	if(temp->statusf_b==1)  //scenario 5(status is 'busy')
	     	{       
		  cout<<"\n--SCENARIO 5 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		  sleep(2);		
		  r=blockNum;
		  r_hqnum=HQnum;			
		  wait_for_specific();		        	  
 		}
	     	else //scenario 1(status is 'free')
	    	{		
		  temp->statusf_b=1;//change status to 'busy' and locks it    
		  mtx.lock();	 
		  if(FL.isPresent(blockNum)) 
		     FL.delete_by_value(blockNum);//status is changed to bsy			
		  mtx.unlock();		    
		  cout<<"\n--SCENARIO 1 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		  sleep(2);		  
		  cout<<"\nbuffer is locked for : "<<std::this_thread::get_id()<<endl;
		  temp->status2=1;//valid status
		  brelse(blockNum,HQnum);     		
		}	
	      }
              else
              {	 sleep(1);
                 if(FL.isEmpty())//scenario 4(no buffers left in FL)
		 {	   
		   cout<<"\n--SCENARIO 4 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		   sleep(2);		
		   wait_for_any();		
		 }
		 temp=FL.delete_from_begin();
        	 if(temp->status2==2)//scenario 3(status is 'delayed write')
		 {                    
		   cout<<"\n--SCENARIO 3 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		   sleep(2);
		thread async_thread(handle_async_write,0);
		 async_thread.join();
		   Insert_at_start=1;
	           PrevTemp=temp->data;
	           //   cout<<"temp->data="<<temp->data;
                   goto lb;
		 } 
		 else//scenario 2(Pick a free buffer from hq and insert at end of required hq) 
		 {  
     	           cout<<"\n--SCENARIO 2 IS APPLYING-- for : "<<std::this_thread::get_id()<<endl;
		   sleep(2);
		   mtx.lock();        	
		   if(HQ[HQnum].isPresent(blockNum)) goto l11;
		   	HQ[HQnum].insert_at_end(blockNum,1,1);
		  mtx.unlock(); 
		  l11:
		  cout<<"\nBuffer is locked for: "<<std::this_thread::get_id()<<endl;
		  temp->status2=1;//valid
		  brelse(blockNum,HQnum); 	 	
		 }		   
              }
	      if(Insert_at_start==1)
	      {
		FL.insert_at_start(PrevTemp,0,1);
		HQ[PrevTemp%4].change_status2(PrevTemp,1);
	      }	
	      break;
 	    }
	    //sleep(2);

            mtx.lock();
	    cout<<"\n****************Hash Queues now :****************\n";
            for(int i=0;i<4;i++)
            {  
	      sleep(0.5); 
	      cout<<"hq "<<i<<" :  ";
	      HQ[i].display();
	      cout<<"\n";
  	    }
            cout<<"\n*****************Free List now  :****************\n";
            FL.display();
            cout<<"\n*************************************************\n";
            mtx.unlock();
            sleep(3);	
           		  
	    cout<<"\n\nThread :"<<std::this_thread::get_id()<<" needs another buffer \n";
	   // cout<<"\nthread handle called again \n";
	    threadhandler(0);    
}

#endif
