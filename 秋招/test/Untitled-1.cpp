#include<iostream>
using namespace std;
class A{
public:
    virtual void func(int i){
        cout<<"func(int i)"<<endl;
    }
};

class B:public A{
public:
    virtual void func(int i,int j){
        cout<<"func(int i,int j)"<<endl;
    }
};

int main(){
    A a;
    B b;
    a.func(2);
    b.func(2,3);
    b.A::func(2);
    system("pause");
    return 0;
}