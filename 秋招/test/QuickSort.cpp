#include<iostream>
#include<vector>
using namespace std;

int partition(vector<int>& a,int l,int r){
    int pivot=a[l];
    while(l<r){
        while(l<r && a[r]>=pivot) r--;
        a[l]=a[r];
        while(l<r && a[l]<=pivot) l++;
        a[r]=a[l];
    }
    a[l]=pivot;
    return l;
}

void quicksort(vector<int>& a,int l,int r){
    if(l<r){
        int index=partition(a,l,r);
        partition(a,l,index-1);
        partition(a,index+1,r);
    }
}

//快速排序（从小到大）
/*void quickSort(int left, int right, vector<int>& arr)
{
	if(left >= right)
		return;
	int i, j, base, temp;
	i = left, j = right;
	base = arr[left];  //取最左边的数为基准数
	while (i < j)
	{
		while (arr[j] >= base && i < j)
			j--;
		while (arr[i] <= base && i < j)
			i++;
		if(i < j)
		{
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}
	//基准数归位
	arr[left] = arr[i];
	arr[i] = base;
	quickSort(left, i - 1, arr);//递归左边
	quickSort(i + 1, right, arr);//递归右边
}*/

int main(){
    vector<int>arr{8,1,14,3,21,5,7,10};
    quicksort(arr,0,arr.size()-1);
   // quickSort(0,arr.size()-1,arr);
    for(int i=0;i<arr.size();i++){
        cout<<arr[i]<<endl;
    }
    return 0;
}