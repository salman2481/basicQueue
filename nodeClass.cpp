/*
Self Referential Node Class decleration and definition that are going to serve as our STACK elements
*/
#include<iostream>
using namespace std;

class cNode {
	//Data carrier of cNode objetct and operator carrier
	int data;
public:
	//Reference to next cNode Object
	cNode* nextNode;

	//Using this pointer to refer to the same class object
	//Basic Class Constructors / Overlaoded constructors
	cNode() { this->data = -1; }
	cNode(int number) { this->data = number; }

	//Class Getter
	int getData() { return data; }


	//Class Setter
	void setData(int number) { this->data = number; }

	//Printing the value of data carrier
	void printData() { cout << data; }

};
#pragma once
#include <iostream.>
using namespace std;
#define max 10
int main()
{
	int number [max]={1,2,3,4,5,6,7,8,9,10};
	int index;
//	cout.set(ios::right);
	for(index=0;index<max;index++)
	{
		cout<<endl<<"the elements of array element are.."<<endl;
		cout.width(10);
		cout <<(index +1)<<"is";
		cout<<number[index];	
	}
		cout <<endl<<endl;
	return 0;
}
