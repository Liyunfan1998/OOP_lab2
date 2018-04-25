#include "iostream"
#include "demo.h"
using namespace std;

#define FALSE 0
#define TRUE 1
#define NOTFALSE TRUE
#define PRODUCT (a*b)

int main(){
#ifdef LAB2_H
	a = 3;
	b = 5;
	int s = sum(a, b); 
	int p = PRODUCT;
	int flag = 1;
	cout<<"#include pass !"<<endl;
#endif

#define PART2

#ifdef PART2
	flag ++;
	cout<<"#define check1 pass !"<<endl;
	cout<<"#ifdef pass !"<<endl;
#undef PART2
#endif

#ifdef PART2
	cout<<"#undef fail !"<<endl;
#else
	flag ++;
	cout<<"#undef pass ! "<<endl;
	cout<<"#else pass !" <<endl;
#endif

#ifndef PART2
	flag ++;
	cout<<"#ifndef pass !"<<endl;
#endif

#if 0
	cout<<"#if fail !"<<endl;
#else
#if 1
	flag ++;
	cout<<"#if pass !"<<endl;
#else
	cout<<"#if fail !"<<endl;
#endif
#endif

#if TRUE
	flag ++;
	cout<<"#define check2 pass !"<<endl;
#else
	cout<<"#define check2 fail !"<<endl;
#endif

#if NOTFALSE
	flag ++;
	cout<<"#define check3 pass !"<<endl;
#endif

#ifdef TRUE
	flag++;
	cout<<"#define check4 pass !"<<endl;
#endif

	if(s==8&&p==15) {
		flag ++;
		cout<<"#define check5 pass !"<<endl;
	} else {
		cout<<"#define check5 fail !"<<endl;
	}

	if(flag==9) {
		cout<<"PART 2 pass !!!!!"<<endl;
	} else {
		cout<<"PART 2 fail !!!!!"<<endl;
	}


#ifdef PART3
#define DOUBLE(arg) (2*arg)
	int d = DOUBLE(a);
	if(d==2*a) {
		cout<<"PART 3 pass !!!!!"<<endl;
	} else {
		cout<<"PART 3 fail !!!!!"<<endl;
	}
#else
	cout<<"Please try to make the test of part 3 work, e.g. add one line in this file !!"<<endl;
#endif


#ifdef PLUSES
#define Cong(arg) "PLUSES "#arg
	cout<<Cong(pass)<<" !!!!!"<<endl;
#else
	cout<<"Please try to make the test of pluses work, e.g. add one line in this file !!"<<endl;
#endif


	return 0;
}
