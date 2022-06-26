#include<vector>
#include<iostream>
using namespace std;

void insertSort(vector<int>& a){
    if(a.size()==0 || a.size()<2){
        return ;
    }
    for(int i=1;i<a.size();i++){
        for(int j=i-1; j>=0 && a[j]>a[j+1];j--){
            swap(a[j+1],a[j]);
        }
    }
}

int main(){
    vector<int>arr{8,1,14,3,21,5,7,10};
    insertSort(arr);
    for(int i=0;i<arr.size();i++){
        cout<<arr[i]<<endl;
    }
    return 0;
}