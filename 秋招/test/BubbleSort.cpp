#include<iostream>
#include<vector>
using namespace std;

void bubbleSort(vector<int>& a){
    for(int j=a.size()-1;j>0;j--){
        for(int i=0;i<j;i++){
            if(a[i]>a[i+1]){
                swap(a[i],a[i+1]);
            }
        }
    }
}

int main(){
    vector<int>arr{8,1,14,3,21,5,7,10};
    bubbleSort(arr);
    for(int i=0;i<arr.size();i++){
        cout<<arr[i]<<endl;
    }
    return 0;
}