// 手写堆排序
#include<iostream>
#include<vector>
using namespace std;

// 递归的方式构建堆
void adjust(vector<int>& arr,int len,int index)
{
    int left=2*index+1;
    int right=2*index+2;

    int maxidx=index;
    if(left<len && arr[left]>arr[maxidx]) maxidx=left;
    if(right<len && arr[right]>arr[maxidx]) maxidx=right;

    if(maxidx!=index){
        swap(arr[maxidx],arr[index]);
        adjust(arr,len,maxidx);
    }
}

void heapSort(vector<int>& arr,int size){
    // 构建大根堆（从最后一个非叶子节点向上）
    for(int i=size/2-1;i>=0;i--){
        adjust(arr,size,i);
    }

// 调整大根堆
    for(int i=size-1;i>=1;i--){
        swap(arr[0],arr[i]);            //将当前最大的放置到数组末尾
        adjust(arr,i,0);                // 将 未完成排序的部分继续进行堆排序
    }
}

int main(){
    vector<int>arr{8,1,14,3,21,5,7,10};
    heapSort(arr,arr.size());
    for(int i=0;i<arr.size();i++){
        cout<<arr[i]<<endl;
    }
    return 0;
}