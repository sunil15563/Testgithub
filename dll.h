#ifndef DLL_H
#define DLL_H
#include<iostream>
//sunil kumar
using namespace std;
struct node
{
  int statusf_b;   //0-free,1-busy
  int status2;//0-invalid,1-valid,2-delayed write
  int data;
  node *next;
  node *prev;
};
class DLL
{
  node *head,*tail;
  int count;
  public:
  DLL()
  {
    head=NULL;
    tail=NULL;
    count=0;        
  }
  void insert_at_end(int,int,int); 
  void insert_at_start(int,int,int);
  node* delete_from_begin();
  void delete_by_value(int);
  bool isPresent(int);
  bool isEmpty();
  node *getBuffer(int);
  void display();
  void change_status2(int,int);
};
//DLL HQ[4],FL;
void DLL::change_status2(int blockNum,int newstatus)
{
/*Input:blockNum,newstatus
  Output:none 
  Description:changes status of given block number to newstatus 
*/
  node *t=head;
  while(t!=NULL)//scan entire dll
  {
     if(t->data==blockNum)
     {
       t->status2=newstatus;
       return;
     }
     t=t->next;
  }
} 
void DLL::insert_at_start(int data,int status,int status2) 
{
/*Input:data,status,status2
  Output:none 
  Description:insert at beggining of dll 
*/
  node *t=new node;
  t->data=data;
  t->statusf_b=status;
  t->status2=status2;
  t->next=NULL;
  t->prev=NULL;
  if(head==NULL||count==0)
  {
    head=t;
    tail=head;
  }
  else
  {
    t->next=head;
    head->prev=t;
    head=t;
  }
  count++;
}
void DLL::insert_at_end(int data,int status,int status2)
{	
/*Input:data,status,status2
  Output:none 
  Description:insert at end of dll 
*/
  node *t=new node;
  t->data=data;
  t->statusf_b=status;
  t->next=NULL;
  t->prev=NULL;
  t->status2=status2;
  if(tail==NULL || count==0)
  {
    head=t;
    tail=head;
  }
  else
  {
    tail->next=t;
    t->prev=tail;
    tail=t;
  }
  count++;
}
node* DLL::delete_from_begin()
{ 
/*Input:none
  Output:pointer to buffer that is deleted from beginning 
  Description:A node is deleted from dll and pointer to it is returned 
*/
  if(count==0 ||head==NULL)
  {
    cout<<"\n empty list  ";
    return NULL;
  }
  node *temp=head;
  node *temp2;
  if(head==tail)
  {
    temp2=head;
   //delete head;
   tail=NULL;
   head=NULL;	
  }
  else
  { 
    temp2=head;
    node *t=head;
    head=t->next;
    head->prev=NULL;
    delete t;
  }
  count--;
  return temp2;
}
void DLL::delete_by_value(int val)
{/*Input:val of type integer
  Output:none
  Description:delete a node from dll by its value 
*/
  if(head==NULL || count==0)
    cout<<"\n empty list "<<count;
  /*if(head==tail)
  { 
  //  delete head;
    tail=NULL;	
    head=NULL;
  }*/
  if(head->data==val)
  {
    if(head->next==NULL)
    {
      delete head;
      return;
    }
    else
    {
      node *t=head;
      head=t->next;
      head->prev=NULL;
      delete t;
    }
  }
  else
  {
    node *pred,*temp,*temp2;
    pred=head;
    temp=head->next;
    for(pred,temp;temp->data!=val;pred=pred->next,temp=temp->next);
    if(temp->next==NULL)
    {
      pred->next=NULL;
      tail=pred;
      delete temp;
      return;
    }	
    else
    {
      pred->next=temp->next;
      temp->next->prev=pred;
      delete temp;
    }
  }
  display();
  count--; 
}
bool DLL::isPresent(int blockNum)
{/*Input:blockNum
  Output:true if blockNum is present otherwise false
  Description:block number is present in dll or not 
*/
  node *t=head;
  while(t!=NULL)
  {
    if(t->data==blockNum)
      return true;
    t=t->next;
  }
  return false;
}
node* DLL::getBuffer(int blockNum) 
{/*Input:blockNum
  Output:pointer to buffer containing given blockNum otherwise NULL
  Description: Searches for blockNum,if found then returns pointer to buffer
*/
  node *t=head;
  while(t!=NULL)
  {
    if(t->data==blockNum)
      return t;
    t=t->next;
  }
  return NULL;  //when buffer with given block number is not found
}
bool DLL::isEmpty()
{/*Input:none
  Output:true if blockNum is present otherwise false
  Description:block number is present in dll or not 
*/
  if(count==0 ||head==NULL)
    return true;
  else 
    return false;
}
void DLL::display()
{/*Input:none
  Output:display dll with data and status
  Description:display dll with status which indicates whether buffer is free/busy/valid/invalid/delayedwrite
*/ 
  node *t=head;
  if(t==NULL) 
    cout<<"empty list ";
  else
  {
    while(t->next!=NULL)
    {
      cout<<t->data<<"/"<<t->statusf_b<<"/"<<t->status2<<" <---> ";
      t=t->next;
    }
    cout<<t->data<<"/"<<t->statusf_b<<"/"<<t->status2;
  }
}
#endif
