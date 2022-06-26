#### LC. 154 寻找旋转排序数组中的最小值 II（略难）

给你一个可能存在 **重复** 元素值的数组 `nums` ，它原来是一个升序排列的数组，并按上述情形进行了多次旋转。请你找出并返回数组中的 **最小元素** 。

<font size=4 color=blue>题解：</font>

​	旋转排序数组 nums 可以被拆分为 2 个排序数组 nums1  , nums2；且 `nums1任一元素 >= nums2任一元素`；因此，考虑二分法寻找此两数组的分界点 nums[i]  (即第 2 个数组的首个元素)。

- 设置 left，right 指针在 nums 数组两端，mid 为每次二分的中点：
  - 当 nums[mid]  > nums[right]  时，mid 一定在第 1 个排序数组中，i 一定满足 mid < i <= right，因此执行 left = mid + 1；
  - 当 nums[mid] <  nums[right]  时，mid  一定在第 2 个排序数组中，i 一定满足 left < i <= mid，因此执行 right = mid；

  - 当 `nums[mid] == nums[right]` 时，是此题的难点（原因是此题中数组的元素**可重复**，难以判断分界点 i 指针区间）；

```
class Solution {
public:
    int findMin(vector<int>& nums) {
        int n=nums.size();
        int l=0,r=n-1;
        while(l<r){
            int mid=l+(r-l)/2;
            if(nums[mid]==nums[r]) r--;
            else if(nums[mid]>nums[r]){
                l=mid+1;
            }
            else{
                r=mid;
            }
        }
        return nums[l];
    }
};
```



#### LC 264. 丑数 II

给你一个整数 `n` ，请你找出并返回第 `n` 个 **丑数** 。**丑数** 就是只包含质因数 `2`、`3` 和 `5` 的正整数。

<font size=4 color=blue>题解：</font>

​	定义数组 dp，其中  dp[i]  表示第 i  个丑数，第 n 个丑数即为 dp[n]。

​	由于最小的丑数是 1，因此 dp[1]=1。如何得到其余的丑数呢？定义三个指针 p2，p3，p5 ，表示下一个丑数是当前指针指向的丑数乘以对应的质因数。初始时，三个指针的值都是 1。

```
class Solution {
public:
    int nthUglyNumber(int n) {
        vector<int> dp(n + 1);
        dp[1] = 1;
        int p2 = 1, p3 = 1, p5 = 1;
        for (int i = 2; i <= n; i++) {
            int num2 = dp[p2] * 2, num3 = dp[p3] * 3, num5 = dp[p5] * 5;
            dp[i] = min(min(num2, num3), num5);
            if (dp[i] == num2) {
                p2++;
            }
            if (dp[i] == num3) {
                p3++;
            }
            if (dp[i] == num5) {
                p5++;
            }
        }
        return dp[n];
    }
};
```



#### LC 220 存在重复元素 III

​	在整数数组 nums 中，是否存在两个下标 i 和 j，使得 nums [i] 和 nums [j] 的差的绝对值小于等于 t ，且满足 i 和 j 的差的绝对值也小于等于 ķ 。

<font size=4 color=blue>题解：</font> 桶排序，每个桶大小为 t，只要桶内有元素，就可以返回 true。否则看 相邻桶是否有元素，且 下标间距<=k

```
class Solution {
public:
    bool containsNearbyAlmostDuplicate(vector<int>& nums, int k, int t) {
        if(t<0) return false;
        long bucket_size=t+1L;
        
        unordered_map<long ,long > bucket;
        for(int i=0;i<nums.size();i++)
        {
            int index= nums[i]/ bucket_size;
            //可能nums[i]为负数，比如-4 / 5 以及 -4 / 5都等于0，所以负数要向下移动一位
            if(nums[i] < 0) index--;
         
            if(bucket.find(index)!=bucket.end()) return true;
      
            else if(bucket.find(index-1)!=bucket.end() && abs(nums[i] - buck[index-1]) <= t)
                return true;
            else if(bucket.find(index+1)!=bucket.end() && abs(nums[i] - bucket[index+1]) <= t)
                return true;
                
            bucket[index] = nums[i];
            if(i >= k)   // 桶相当于  滑动窗口，当 i>=k，删除最左边的桶，维持相邻桶元素间隔 k
            {
                bucket.erase(nums[i - k] / bucket_size);
            }
        }
        return false;
    }
};
```

#### LC 26. 删除有序数组中的重复项（双指针）

​	给你一个**有序数组 nums** ，请你 原地 删除重复出现的元素，使每个元素 只出现一次 ，返回删除后数组的新长度。不要使用额外的数组空间，你必须在 原地 修改输入数组 并在使用 O(1) 额外空间的条件下完成。

<font size=4 color=blue>题解：</font>首先数组是有序的，那么重复的元素一定会相邻。要求删除重复元素，实际上就是将不重复的元素移到数组的左侧。

双指针，指针p 和q。比较 p 和 q 位置的元素是否相等。如果相等，q 后移 1 位；如果不相等，将 q 位置的元素复制到 p+1 位置上，p 后移一位，q 后移 1 位；重复上述过程，直到 q 等于数组长度。

返回 p + 1，即为新数组长度。

```
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int n=nums.size();
        if(!n) return 0;
        int left=0,right=1;
        while(right<n){
            if(nums[left]!=nums[right]){
                nums[++left]=nums[right];
            }
            right++;
        }
        return left+1;
    }
};
```



#### 剑指Offer 47 礼物的最大价值



## 回溯算法

【 LC优质解答 】 https://leetcode-cn.com/problems/subsets/solution/c-zong-jie-liao-hui-su-wen-ti-lei-xing-dai-ni-gao-/

回溯算法与 DFS 的区别就是   有无状态重置；

**何时使用回溯算法**
		当问题需要 "回头"，以此来查找出所有的解的时候，使用回溯算法。**即满足结束条件或者发现不是正确路径的时候(走不通)，要撤销选择，回退到上一个状态**，继续尝试，直到找出所有解为止；



### （一）组合| 子集（与顺序无关）

#### LC 78. 子集

给你一个整数数组 `nums` ，数组中的元素 **互不相同** 。返回该数组所有可能的子集（幂集）。

<font size=4 color=blue>题解：</font> 

<img src="https://pic.leetcode-cn.com/d8e07f0c876d9175df9f679fcb92505d20a81f09b1cb559afc59a20044cc3e8c-%E5%AD%90%E9%9B%86%E9%97%AE%E9%A2%98%E9%80%92%E5%BD%92%E6%A0%91.png" alt="子集问题递归树.png" style="zoom:50%;" />

```
class Solution {
private:
    vector<vector<int>> ans;
    vector<int> temp;
public:
    void dfs(int start, vector<int>& nums){
        ans.push_back(temp);
        for(int i = start; i < nums.size(); i++){    // start,
            temp.push_back(nums[i]);
            dfs(i + 1, nums);
            temp.pop_back();
        }
    }

    vector<vector<int>> subsets(vector<int>& nums) {
        dfs(0, nums);
        return ans;
    }
};
```

#### LC 90. 子集 II

给你一个整数数组 `nums` ，其中可能**包含重复元素**，请你返回该数组所有可能的子集（幂集）。

```
class Solution {
private:
    vector<vector<int>> res;
    vector<int> path;
public:
    void backtrace(vector<int>& nums,int start){
        res.push_back(path);
        for(int i=start;i<nums.size();i++){    // start，状态变量
            if(i>start && nums[i]==nums[i-1])   // 去重，剪枝
                continue;
            path.push_back(nums[i]);
            backtrace(nums,i+1);
            path.pop_back();
        }
    }
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        res.clear();
        path.clear();
        sort(nums.begin(),nums.end());  // 对于重复元素，排序后更方便操作
        backtrace(nums,0);
        return res;
    }
};
```

#### LC 39.  组合总和

​		给定一个**无重复元素的数组 candidates 和一个目标数 target** ，找出 candidates 中所有可以使数字和为 target 的组合。candidates 中的**数字可以无限制重复被选取**。

​      示例：

```
输入：candidates = [2,3,6,7], target = 7,
所求解集为：
[
  [7],
  [2,2,3]
]
```

解答：

```
class Solution {
    vector<int> path;
    vector<vector<int>> res;
public:
    void backtrace(vector<int>& candidates,int start,int target){
        if(target==0){  // 终止搜索| 回溯条件
            res.push_back(path);
            return ;
        }
        for(int i=start;i<candidates.size()&& target-candidates[i]>=0;i++){  // target-candidates[i]>=0 剪枝条件
            path.push_back(candidates[i]);
            backtrace(candidates,i,target-candidates[i]);
            path.pop_back();
        }
    }
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        sort(candidates.begin(),candidates.end());  // 排序，方便剪枝
        backtrace(candidates,0,target);
        return res;
    }
};
```



### （二）排列

【 LC优质解答 】 https://leetcode-cn.com/problems/zi-fu-chuan-de-pai-lie-lcof/solution/c-zong-jie-liao-hui-su-wen-ti-lei-xing-dai-ni-ga-4/

#### LC 46. 全排列

给定一个 **没有重复 数字**的序列，返回其所有可能的全排列。

```
示例:
输入: [1,2,3]
输出:
[
  [1,2,3],
  [1,3,2],
  [2,1,3],
  [2,3,1],
  [3,1,2],
  [3,2,1]
]
```

<font size=4 color=blue>题解：</font> 

<img src="https://pic.leetcode-cn.com/60930c71aa60549ff5c78700a4fc211a7f4304d6548352b7738173eab8d6d7d8.png" alt="在这里插入图片描述" style="zoom:30%;" />

```
class Solution {
private:
    vector<int> path;
    vector<vector<int>> res;
public:
    vector<vector<int>> permute(vector<int>& nums) {
        vector<bool> used(nums.size(),false);   // 使用 used[]
        backtrace(nums,used);
        return res;
    }

    void backtrace(vector<int>& nums,vector<bool> used){
        if(path.size()==nums.size()){
            res.push_back(path);
            return ;
        }
        for(int i=0;i<nums.size();i++){    //
            if(used[i]==true) continue;
            used[i]=true;
            path.push_back(nums[i]);
            backtrace(nums,used);
            path.pop_back();
            used[i]=false;
        }
    }
};
```

<font size=4 color=red>**总结：**</font>

<font size=4>可以发现“排列”类型问题和“子集、组合”问题不同在于：**“排列”问题使用used数组来标识选择列表**，而**“子集、组合”问题则使用start参数**。</font>

#### LC  47.  全排列 II

​		给定一个可**包含重复数字**的序列 `nums` ，按任意顺序 返回所有不重复的全排列。

<font size=4 color=blue>题解：</font> 

参考 LC 90，**对于需要去重的，将 nums 数组排序，让重复的元素并列排在一起。**

```
void backtrack(vector<int>& nums,vector<bool>&used,vector<int>& path)//used初始化全为false
{
    if(path.size()==nums.size())
    {
        res.push_back(path);
        return;
    }
    for(int i=0;i<nums.size();i++)          //从给定的数中除去，用过的数，就是当前的选择列表
    {
        if(!used[i])
        {
            if(i>0&&nums[i]==nums[i-1]&&!used[i-1])    //剪枝，三个条件
                continue;
            path.push_back(nums[i]);                  //做选择
            used[i]=true;                            //设置当前数已用
            backtrack(nums,used,path);              //进入下一层
            used[i]=false;                          //撤销选择
            path.pop_back();                        //撤销选择
        }
    }
}
```

#### ==LC 31. 下一个排列==

<font size=4 color=red> 题目：</font>

实现获取 下一个排列 的函数，算法需要将给定数字序列重新排列成字典序中下一个更大的排列。

如果不存在下一个更大的排列，则将数字重新排列成最小的排列（即升序排列）。

<font size=4 color=blue>题解：</font> 

1. 在**尽可能靠右的低位**进行交换，需要**从后向前**查找（画图）

下一个排序：尽可能将左边的【较小数】 和 右边的【较大数】 作交换；同时让 【较小数】尽量靠右，而【较大数】尽可能小

具体地：从后往前，**找到第一个非降序的数组**【ai，ai+1】。然后再从后往前找到第一个aj，使得 aj>ai；交换数据，并翻转【ai...an-1】;

```
class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        int n=nums.size();
        int i=n-2;
        // 寻找第一个非降序对
        while(i>=0 &&nums[i+1]<=nums[i]) i--;
        if(i>=0){
            int j=n-1;
            // 寻找 
            while(j>=0 && nums[j]<=nums[i]) j--;
            swap(nums[i],nums[j]);
        }
        reverse(nums.begin()+i+1,nums.end());
    }
};
时间复杂度：O(N)
```



## LC 554	砖墙（Middle）

题目（简单描述）：有一堵矩形的，由 n行砖块组成的砖墙。砖块的高度相同，宽度不同。画一条自顶向下的垂线，求穿过的最少砖块数。（不能沿着墙的两个垂直边缘之一画线）

思路：乍一看，以为 和 求 “穿过气球的最少箭数” 题目一样，先排序再求不 相交的区间数。

​           但不同的是，这个题目不需要排序，关键点在于，求解 “每行中的砖块 距离最左侧的距离”。距离相同的出现最多，即穿过的砖块数最少。（画图好理解，和 贪心无关）

```
class Solution {
public:
    int leastBricks(vector<vector<int>>& wall) {
        unordered_map<int,int> map;
        for(auto& walls:wall){
            int n=walls.size();
            int sum=0;
            for(int i=0;i<n-1;i++){
                sum+=walls[i];
                map[sum]++;
            }
        }
        int maxcnt=0;
        for(auto& [_,c]:map){    // 注意代码写法
            maxcnt=max(maxcnt,c);
        }
        return wall.size()-maxcnt;
    }
};
```

时间复杂度：O(nm)，其中 n是砖墙的高度，m 是每行砖墙的砖的平均数量。

空间复杂度：O(nm)

## LC 452. 用最少数量的箭引爆气球（Middle）

题目（概况一下）：提供 气球直径的开始和结束坐标，求 使得 所有气球全部被引爆，所需的弓箭的最小数量。

思路：Lc 554 是二维坐标，而 LC 452 其实是一维坐标。先根据 气球的结束坐标排序，然后其 区间数

```
class Solution {
public:
    int findMinArrowShots(vector<vector<int>>& points) {
        sort(points.begin(),points.end(),[](vector<int>a,vector<int>b){
            return a[1]<b[1];    // 按结束坐标排序
        });
        int r=points[0][1];
        int cnt=1;
        for(auto& poin:points){
            if(poin[0]>r){
                cnt+=1;
                r=poin[1];
            }
        }
        return cnt;
    }
};
```

时间复杂度：O(n log n)，n 是数组  points  的长度。排序的时间复杂度为 O(n log n)。

空间复杂度：O(log n)，即为排序需要使用的栈空间。

## LC 647. 回文子串

题目：计算字符串中有多少个回文子串。

思路：1、动态规划 dp [ i ] [ j ] ，表示以 i 为起始点，j 为中心点的 回文子串

```
class Solution {
public:
    int countSubstrings(string s) {
        vector<vector<bool>>dp(s.size(),vector<bool>(s.size()));
        int ans=0;
        for(int i=0;i<s.size();i++){   // 枚举可能的回文中心
            for(int j=0;j<=i;j++){
                if(s[i]==s[j]&& (i-j<2 || dp[j+1][i-1])){
                    dp[j][i]=true;
                    ans++;
                }
            }
        }
        return ans;
    }
};
时间复杂度为 O(N^2)；空间复杂度为 O(N^2)
```

2、中心扩散法，双指针

```
    int countSubstrings(string s) {
        int ans=0;
        // 枚举可能的中心位置
        for(int center=0;center<2*s.size()-1;center++){
            int left=center/2;
            int right=left+center%2;  // 可能是 奇数长度也可能是偶数长度
            while(left>=0 && right<s.size()  && s[left]==s[right]){
                 ans+=1;
                 left--;
                 right++;
            }
        }
        return ans;
    }
```

## LC 5. 最长回文子串（Middle）（3刷）

可以依据 647 返回的所有回文子串中，找出最长的。

```
class Solution {
public:
    string longestPalindrome(string s) {
        string ans="";
        int maxlen=0;
        for(int cen=0;cen<2*s.size()-1;cen++){
            int left=cen/2;
            int right=left+cen%2;
            string temp;
            while(left>=0 && right<s.size() && s[left]==s[right]){
                left--;
                right++;
            }
            if(right-left+1>maxlen){
                ans=s.substr(left+1,right-left-1);
                maxlen=right-left+1;
            }
        }
        return ans;
    }
};
```

## LC 3. 无重复的最长子串 Middle

题目：给定一个字符串，找出其中不含有重复字符的最长子串的长度。

思路：滑动窗口（双指针），使用 unordered_set，判断 滑动窗口内的子串是否重复。

```
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        if(s.size()==0) return 0;
        unordered_set<char> map;
        int l=0,ans=0                 // 双指针
        for(int r=0;r<s.size();r++){
            while(map.find(s[r])!=map.end()){
                map.erase(s[l]);
                l++;
            }
            map.insert(s[r]);
            ans=max(ans,r-l+1);
        }
        return ans;
    }
};
时间复杂度：O(n)
```

## LC 76. 最小覆盖子串 （Hard）滑动窗口

题目：给你一个字符串 s，一个字符串 t。返回 s 中覆盖 t 所有字符的最小子串。

思路：滑动窗口，先右移完全覆盖 t 子串，再左移缩小边界，然后不断进行这个过程，同时记录最小子串。

```
class Solution {
public:
    string minWindow(string s, string t) {
        vector<int> map(128);   
        int left = 0, right = 0, need = t.size(), minStart = 0, minLen = INT_MAX;
        for(char ch : t)    ++map[ch];      //统计t中字符出现次数      
        while(right < s.size())
        {
            if(map[s[right]] > 0) --need;   //窗口右移，每包含一个t中的字符，need-1
            --map[s[right]];
            ++right;
            while(need == 0)    //完全覆盖子串时
            {
                if(right - left < minLen)   //此时字符被包含在[left,right)中
                {
                    minStart = left;
                    minLen = right - left;
                }
                if(++map[s[left]] > 0) ++need;  //窗口左移
                ++left;
            }
        }
        if(minLen != INT_MAX)   return s.substr(minStart, minLen);
        return "";
    }
};
```

## LC 137. 只出现一次的数字 II（Middle）剑指Offer 56-II

题目：给一个整数数组 nums，除某个元素仅出现一次外，其余每个元素都出现了三次，请返回那个只出现一次的元素。

思路：位运算

```
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int res=0;
        for(int i=0;i<32;i++){
            int cnt=0;                     // 分别计算每个 bit上的1个数
            for(auto& num:nums){
                if(num &(1<<i)) cnt+=1    // & 运算，计算出各位置上1的个数，并和3 求余
            }
            if((cnt%3)==1)
                res^=(1<<i);             // 用异或 方法生成二进制中的每一位
        }
        return res;
    }
};
```



## LC 993. 二叉树的堂兄弟节点(BFS,队列)

```
// BFS
class Solution {
public:
    bool isCousins(TreeNode* root, int x, int y) {
        TreeNode* ans=nullptr;                  // 保存其中一个元素的父节点  
        queue<pair<TreeNode*,TreeNode*>> que;   // 队列中需要保存父节点信息
        que.emplace(nullptr,root);
        while(!que.empty()){
            int size=que.size();
            for(int i=0;i<size;i++){
                auto[p,q]=que.front();
                que.pop();
                if(q->val==x || q->val==y){
                    if(ans && ans!=p) return true;    // 堂兄弟，在当前层次，但父节点不同
                    ans=p;
                }
                if(q->left) que.emplace(q,q->left);
                if(q->right) que.emplace(q,q->right);
            }
            if(ans) return false;   // for 循环结束，表示当前层的所有结点遍历结束，而 ans!=nullptr，则说明，两个节点不在同一层，深度不同，所以直接返回 false
        }
        return false;
    }
};
```



```
// DFS
class Solution {
private:
    int x,y;
    int d1,d2; // 深度
    TreeNode* f1;
    TreeNode* f2;   // 父节点
    bool found_x,found_y;
public:
    void dfs(TreeNode* root,int dep,TreeNode* fa){
        if(!root) return;
        if(root->val==x){
            tie(d1,f1,found_x)=tuple{dep,fa,true};  //
        }
        else if(root->val==y){
            tie(d2,f2,found_y)=tuple{dep,fa,true};
        }
        if(found_x && found_y) return ;   // 可以提前结束遍历
        dfs(root->left,dep+1,root);
        if(found_x && found_y) return ;
        dfs(root->right,dep+1,root);
    }

    bool isCousins(TreeNode* root, int x, int y) {
        this->x=x;
        this->y=y;
        dfs(root,0,nullptr);
        return (d1==d2 && f1!=f2);
    }
};
```



## 剑指OFFER 16. 数值的整数次方（第三次，但没印象）

题目：实现 [pow(*x*, *n*)](https://www.cplusplus.com/reference/valarray/pow/) ，即计算 x 的 n 次幂函数。不得使用库函数，同时不需要考虑大数问题。

> 提示：	-100.0 < x < 100.0
> 				-231 <= n <= 231-1
> 				-104 <= xn <= 104

```
class Solution {
public:
    double myPow(double x, int n) {
        long N=n;
        if(n<0){
            x=1/x;
            N=-N;
        }

        return fastpow(x,N);
    }
//递归  
    double fastpow(double x,int n){
        if(n==0) return 1;
        double temp=fastpow(x,n/2);
        if(n&1) return temp*temp*x;      // (n&1) 如果为奇数的话
        else return temp*temp;
    }
};
时间复杂度: O(log2 n)
```



## LC 692. 前K个高频单词

给一非空的单词列表，返回前 *k* 个出现次数最多的单词。

返回的答案应该按单词出现频率由高到低排序。如果不同的单词有相同出现频率，按字母顺序排序。

```
// 哈希表 + 优先队列
class Solution {
public:
    vector<string> topKFrequent(vector<string>& words, int k) {
        unordered_map<string,int>map;
        for(auto& word:words){
            map[word]++; 
        }
        // lambda 函数的写法
        auto compare=[](const pair<string,int>& a,const pair<string,int>& b){
            return a.second==b.second ? a.first<b.first : a.second>b.second;
        };
        priority_queue<pair<string,int>,vector<pair<string,int>>,decltype (compare)> q(compare);
        for(auto& m:map){
            q.emplace(m);
            if(q.size()>k){
                q.pop();
            }
        }
        vector<string> res(k);
        for(int i=k-1;i>=0;i--){
            res[i]=q.top().first;
            q.pop();
        }
        return res;
    }
};
```

## LC 347.  前 K 个高频元素

给你一个整数数组 `nums` 和一个整数 `k` ，请你返回其中出现频率前 `k` 高的元素。你可以按 **任意顺序** 返回答案。

```
// 桶排序
class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int,int>dict;
        for(auto& num:nums){
            dict[num]++;
        }
        vector<vector<int>>buckets(nums.size()+1);
        for(auto&[key,value]:dict){
            buckets[value].emplace_back(key);
        }
        vector<int>res;
        for(int i=buckets.size()-1;i>=0&&res.size()<k;i--){
            for(auto& bucket:buckets[i]){
                res.emplace_back(bucket);
                if(res.size()==k) break;
            }

        }
        return res;
    }
};

```

## LC 477 汉明距离的总和（位运算）

题目：

两个整数的 汉明距离 指的是这两个数字的二进制数对应位不同的数量。给你一个整数数组 nums，请你计算并返回 nums 中任意两个数之间汉明距离的总和。

> 示例 1：
>
> 输入：nums = [4,14,2]
> 输出：6
> 解释：在二进制表示中，4 表示为 0100 ，14 表示为 1110 ，2表示为 0010 。（这样表示是为了体现后四位之间关系）
> 所以答案为：
> HammingDistance(4, 14) + HammingDistance(4, 2) + HammingDistance(14, 2) = 2 + 2 + 2 = 6

<font size=4>方法1（自己写的暴力法，超时了，但记录一下）</font>

```
class Solution {
public:
    int distance(int a,int b){
        int n=a^b;
        int cnt=0;
        while(n){
            n&=(n-1);
            cnt++;
        }
        return cnt;
    }
    int totalHammingDistance(vector<int>& nums) {
        int n=nums.size();
        int res=0;
        for(int i=0;i<n;i++){
            for(int j=i+1;j<n;j++){
                res+=distance(nums[i],nums[j]);
            }
        }
        return res;
    }
};
```

方法2：因为方法1存在重复计算，可以考虑，计算数组中所有数在每个位上的0,1个数

```
class Solution {
public:
    int totalHammingDistance(vector<int>& nums) {
        int n=nums.size();
        int res=0;
        for(int i=0;i<30;i++){
            int s0=0,s1=0;     // 此处可以优化，只需要一个变量存储 1状态个数，0状态个数即为 n-s1;
            for(int& num:nums){
                if(((num>>i)&1)==1){
                    s0++;
                }
                else
                    s1++;
            }
            res+=s0*s1;
        }
        return res;  
    }
};
```

## 剑指Offer 68 . 二叉树的最近公共祖先（后序遍历）

```
class Solution {
// 后序遍历
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if(!root || root->val==p->val || root->val==q->val){
            return root;
        }
        TreeNode* leftroot=lowestCommonAncestor(root->left,p,q);
        TreeNode* rightroot=lowestCommonAncestor(root->right,p,q);
        if(!leftroot) return rightroot;
        if(!rightroot) return leftroot;
        return root;
    }
};
时间复杂度 O(n);
空间复杂度 O(n);
```



## 剑指Offer 56-I. 数组中数字出现的次数（位运算，3刷，还是没有做出来）

题目：

一个整型数组 `nums` 里除两个数字之外，其他数字都出现了两次。请写程序找出这两个只出现一次的数字。要求时间复杂度是O(n)，空间复杂度是O(1)。

```
class Solution {
public:
    vector<int> singleNumbers(vector<int>& nums) {
        int res=0;
        for(auto num:nums){
            res^=num;
        }
        int div=1;
        // while 循环找出 两个不同的数之间，最低位的1
        while((div & res)== 0){
            div<<=1;
        }
        int a=0,b=0;  // 还原出这两个不同的数
        for(int n:nums){
            if(div & n){
                a^=n;
            }
            else
              b^=n;
        }
        return vector<int>{a,b};
    }
};
```



## 剑指Offer 40. 最小的 K 个数（堆排/ 快排）

题目：

输入整数数组 `arr` ，找出其中最小的 `k` 个数。例如，输入4、5、1、6、2、7、3、8这8个数字，则最小的4个数字是1、2、3、4。

```
class Solution {
public:
    vector<int> getLeastNumbers(vector<int>& arr, int k) {
        if(k>arr.size()) return arr;
        if(k==0) return {};
        vector<int> res;
        priority_queue<int,vector<int>> max_heap;  // 最小 K个数，用 大顶堆实现
        sort(arr.begin(),arr.end());

        for(int i=0;i<k;i++) max_heap.push(arr[i]);
        for(int i=k;i<arr.size();i++){
            if(arr[i]<max_heap.top()){
                max_heap.pop();
                max_heap.push(arr[i]);
            }
        }

        for(int i=0;i<k;i++){
            res.emplace_back(max_heap.top());
            max_heap.pop();
        }
        return res;
    }
};
空间复杂度为 O(k)；时间复杂度 O(nlogk),其中，每个元素 入堆/出堆的时间复杂度为O(log k);
```

方法2：快排

```
class Solution {
public:
    void quicksort(vector<int>& arr,int l,int r,int tar){
        int index=partition(arr,l,r);
        if(index==tar) return;
        else{
             index>tar ? quicksort(arr,l,index-1,tar) : quicksort(arr,index+1,r,tar);
        }
    }

    int partition(vector<int>& arr,int l,int r){
        int temp=arr[l];
        while(l<r){
            while(l<r && arr[r]>=temp) --r;
            arr[l]=arr[r];
            while(l<r && arr[l]<=temp) ++l;
            arr[r]=arr[l];
        }
        arr[l]=temp;
        return l;
    }
    vector<int> getLeastNumbers(vector<int>& arr, int k) {
        if(k>=arr.size()) return arr;
        vector<int> res;
        quicksort(arr,0,arr.size()-1,k);
        res.assign(arr.begin(),arr.begin()+k);
        return res;
    }
};
```



## 剑指Offer 11. 旋转数组的最小数字（二刷，没做出）

题目：

输入一个递增排序的数组的一个旋转，输出旋转数组的最小元素。例如，数组 `[3,4,5,1,2]` 为 `[1,2,3,4,5]` 的一个旋转，该数组的最小值为1。 

```
class Solution {
    // 二分查找
public:
    int minArray(vector<int>& numbers) {
        int r=numbers.size()-1,l=0;
        
        while(l<r){
            int mid=l+(r-l)/2;
            if(numbers[mid]>numbers[r]){
                l=mid+1;
            }else if(numbers[mid]<numbers[r]){
                r=mid;
            }
            else r--;
        }
        return numbers[r];
    }
};
```

### 剑指Offer 04. 二维数组中的查找

<font size=4 color=red>题目：</font>

在一个 n * m 的二维数组中，每一行都按照从左到右递增的顺序排序，每一列都按照从上到下递增的顺序排序。请完成一个高效的函数，输入这样的一个二维数组和一个整数，判断数组中是否含有该整数。

<font size=4 color=blue>思路：</font> 画图，在二维矩阵中，数从左到右增长，从上到下增长；用双指针 l,r；l 指向 小值，r 指向 大值；根据 target 判断 双指针的移动；

```
class Solution {
public:
    bool findNumberIn2DArray(vector<vector<int>>& matrix, int target) {
        int n=matrix.size();
        if(!n) return false;
        int m=matrix[0].size();
        int l=0,r=m-1;
        while(l<n && r>=0){
            if(matrix[l][r]==target) return true;
            else if(matrix[l][r]>target) r--;
            else l++;
        }
        return false;
    }
};
时间复杂度：O(N.M); 空间复杂度：O(1);
```

### 剑指Offer 14-I. 剪绳子

<font size=4 color=red>题目：</font>

给你一根长度为 n 的绳子，请把绳子剪成整数长度的 m 段（m、n都是整数，n>1并且m>1），每段绳子的长度记为 k[0],k[1]...k[m-1] 。请问 k[0]k[1]...k[m-1] 可能的最大乘积是多少？例如，当绳子的长度是8时，我们把它剪成长度分别为2、3、3的三段，此时得到的最大乘积是18。

<font size=4 color=blue>思路：</font> 每个数，都可用 2，3 累计相加表示。

```
class Solution {
public:
    int cuttingRope(int n) {
        if(n==2) return 1;
        if(n==3) return 2;
        int res=1;
        while(n>4){
            n-=3;
            res*=3;
        }
        return (int)res*n;
    }
};
```

```
class Solution {
public:
    int cuttingRope(int n) {
        if(n<=3) return n-1;
        int a=n%3,b=n/3;
        if(a==0) return (int)pow(3,b);
        else if(a==1) return (int)pow(3,b-1)*4;
        return (int)pow(3,b)*2;
    }
};
```

### 剑指Offer 22.  链表中倒数第 k 个节点

```
class Solution {
// 快慢指针，快指针先走 k 步
public:
    ListNode* getKthFromEnd(ListNode* head, int k) {
        ListNode* fast=head;
        ListNode* slow=head;
        int cnt=0;
        while(k){
            fast=fast->next;
            k--;
        }
        while(fast){
            fast=fast->next;
            slow=slow->next;
        }
        return slow;
    }
};
```

<font size=4 color=blue>思考，为什么代码不能写成如下的顺序 ？</font>

```
class Solution {
// 快慢指针，快指针先走 k 步
public:
    ListNode* getKthFromEnd(ListNode* head, int k) {
        ListNode* fast=head;
        ListNode* slow=head;
        int cnt=0;
        while(fast){
			while(k){
            	fast=fast->next;
            	k--;
        	}// 如果 把 fast快走 k 步的逻辑放在 while(fast)循环里，会出现一个问题 ;有可能 fast 走k步后，fast 已经等于 null, 而 while(k) 退出循环后，没有作判断，而是直接 移动 快慢指针。所以 有特殊的测试用例无法通过。
            fast=fast->next;
            slow=slow->next;
        }
        return slow;
    }
};
```

### 剑指Offer 26. 树的子结构

<font color=red size=4>题目：</font>

输入两棵二叉树A和B，判断B是不是A的子结构。(约定空树不是任意一个树的子结构)

B是A的子结构， 即 A中有出现和B相同的结构和节点值。

```
class Solution {
public:
    bool isSubStructure(TreeNode* A, TreeNode* B) {
        if(B==NULL || A==NULL)
            return false;
        //B是A的子树，有三种情况，B和A完全一样,B和A的左子树相同，B和A的右子树相同
        return isMatch(A,B)||isSubStructure(A->left,B)||isSubStructure(A->right,B);
    }
    bool isMatch(TreeNode* subA,TreeNode* B){
        if(subA==NULL)
           return false;
        if(subA->val==B->val){
            bool tag=true;
            if(B->left!=NULL)
               tag=tag && isMatch(subA->left,B->left);
            if(B->right!=NULL)
               tag=tag && isMatch(subA->right,B->right);

            if(tag)
             return true;
        }
        return false;
    }
};
```

### 剑指Offer 38. 字符串的排列（再做一遍吧）

<font color=red size=4>题目：</font>

输入一个字符串，打印出该字符串中字符的所有排列。（不能有重复元素）

<font color=red>示例：</font>

```
输入：s = "abc"
输出：["abc","acb","bac","bca","cab","cba"]
```

```
class Solution {
// 回溯 
    vector<string> res;
public:
    vector<string> permutation(string s) {
        dfs(s,0);
        return res;
    }
    void dfs(string& s,int pos){
        if(pos==s.size()-1){ // dfs 结束的条件
            res.push_back(s);
            return;
        }
        unordered_set<int> set;
        for(int i=pos;i<s.size();i++){
            if(set.find(s[i])!=set.end()) continue;
            set.insert(s[i]);
            swap(s[i],s[pos]);
            dfs(s,pos+1);
            swap(s[i],s[pos]);
        }
    }
};
时间复杂度 O（N!N）；N为字符串 s 的长度；时间复杂度和字符串排列的方案数成线性关系，方案数为 N *(N-1) * (N-2)*2*1 ，即复杂度为 O(N!)；字符串拼接操作 O(N) ；因此总体时间复杂度为 O(N!N)
空间复杂度 O(N^2)；全排列的递归使用栈空间大小为 O(N)) ；递归中辅助 Set 累计存储的字符数量最多为 N+(N-1+... +2+1=(N+1)N/2 ，即占用 O(N^2)的额外空间。
```

### 剑指Offer 24.  反转链表(easy)

<font color=blue size=4>题目：</font>

定义一个函数，输入一个链表的头节点，反转该链表并输出反转后链表的头节点。

<font color=blue size=4>思路</font>

### 归并排序 模板

```
class Solution {
public:
// 归并排序
    void merge(vector<int>& arr,int l,int mid,int r){
        vector<int> temp;
        temp.resize(r-l+1);
        int i=0;
        int p1=l;
        int p2=mid+1;
        while(p1<=mid && p2<=r){
            temp[i++]= arr[p1]<arr[p2] ? arr[p1++] :arr[p2++];
        }
        while(p1<=mid){
            temp[i++]=arr[p1++];
        }
        while(p2<=r){
            temp[i++]=arr[p2++];
        }
        for(int i=0;i<temp.size();i++){
            arr[l+i]=temp[i];
        }
    }
    void mergesort(vector<int>& arr,int l,int r){
        if(l==r) return;
        int mid=l+(r-l)/2;
        mergesort(arr,l,mid);
        mergesort(arr, mid+1,r);
        merge(arr,l,mid,r);
    }
    vector<int> MySort(vector<int>& arr) {
        mergesort(arr,0,arr.size()-1);
        return arr;
    }
};
```

### 冒泡排序

```
void BubbleSort(vector<int>& arr,int size){
	for(int i=0;i<size-1;i++){
		for(int j=i+1;j<size;j++){
			if(arr[j]<arr[i])
				swap(arr[j],arr[i]);
		}
	}
}
vector<int> MySort(vector<int>& arr){
	BubbleSort(arr,arr.size());
	return arr;
}
```



# “前缀和”专题

<font color=purple size=4>【前缀和 + 哈希表】</font>：将每个前缀和作为 map.key，map.value 根据题目要求；

### LC 523. 连续的子数组和

题目：给你一个整数数组 nums 和一个整数 k ，编写一个函数来判断该数组是否含有同时满足下述条件的连续子数组：

- 子数组大小 至少为 2 ，且
- 子数组元素总和为 k 的倍数。
  如果存在，返回 true ；否则，返回 false 。

<font size=4 color=blue> 思路：</font> <font color=red>前缀和+ 哈希表</font>；设 以 i 结尾的前缀和为 Presum1，以 j 结尾的前缀和为 Presum2；如果（Presum2 - Presum1）% k==0，即为 k 的倍数，在种情况下，Presum1 % k == Presum2 % k ; 所以，设一个哈希表，存储出现过的 余数，查找余数是否相同。

```
class Solution {
// 前缀和+哈希表
public:
    bool checkSubarraySum(vector<int>& nums, int k) {
        int m = nums.size();
        if (m < 2) {
            return false;
        }
        unordered_map<int, int> mp;
        mp[0] = -1;
        int remainder = 0;   
        for (int i = 0; i < m; i++) {
            remainder = (remainder + nums[i]) % k;
            if (mp.count(remainder)) {
                int prevIndex = mp[remainder];
                if (i - prevIndex >= 2) {
                    return true;
                }
            } else {
                mp[remainder] = i;
            }
        }
        return false;
    }
};
时间复杂度： O(N),N 为数组的大小；空间复杂度：O(min(N,k))
```



### LC 525 连续数据

题目：给定一个二进制数组 `nums` , 找到含有相同数量的 `0` 和 `1` 的最长连续子数组，并返回该子数组的长度。

```
class Solution {
// 前缀和+哈希表 
// 相同数量的 0,1 将 0看做-1，则该子数组和=0;
public:
    int findMaxLength(vector<int>& nums) {
        int n=nums.size();
        unordered_map<int,int>map;
        map[0]=-1;  // 前缀和为0 的下标为 -1;
        int presum=0;
        int maxlen=0;
        for(int i=0;i<n;i++){
            int num=nums[i];
            if(num==0) presum--;
            else presum++;
            if(map.count(presum)){
                int prevIndex=map[presum];
                maxlen=max(maxlen,i-prevIndex);
            }
            else{
                map[presum]=i;
            }
        }
        return maxlen;
    }
};
时间复杂度：O(N)，空间复杂度：O(N)
```



### LC 560 和为K的子数组

给定一个整数数组和一个整数 **k，**你需要找到该数组中和为 **k** 的连续的子数组的个数。

<font size=4 color=blue>题解：</font>

<font color=red>前缀和+哈希</font>

```
class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        unordered_map<int,int> map;   // map[i][j] 表示 前缀和i 出现了 j 次
        map[0]=1;
        int presum=0,cnt=0;
        for(auto& num:nums){
            presum+=num;
            if(map.find(presum-k)!=map.end()) cnt+=map[presum-k];
            map[presum]++;
        }
        return cnt;
    }
};
```



# “动态规划” 专题

###  LC 474. 一 和零（要再做一次）

题目：

给你一个二进制字符串数组 strs 和两个整数 m 和 n 。请你找出并返回 strs 的最大子集的大小，该子集中 最多 有 m 个 0 和 n 个 1 。

<font size=4 color=blue>题解：</font>

<font color=red>动态规划，01背包问题</font>

```
class Solution {
// 01动态规划：dp[k][i][j] 代表 前 k个字符串中，有 i个0，j个1;
public:
    vector<int> get(string& s){
        vector<int>res(2);
        for(auto& c:s){
            res[c-'0']++;
        }
        return res;
    }
    int findMaxForm(vector<string>& strs, int m, int n) {
        int size=strs.size();
        vector<vector<vector<int>>> dp(size + 1, vector<vector<int>>(m + 1, vector<int>(n + 1)));
        for(int k=1;k<size+1;k++){
            vector<int>zeros=get(strs[k-1]);
            for(int i=0;i<=m;i++){
                for(int j=0;j<=n;j++){
                    dp[k][i][j]=dp[k-1][i][j];
                    if(zeros[0]<=i && zeros[1]<=j){
                        dp[k][i][j]=max(dp[k][i][j],dp[k-1][i-zeros[0]][j-zeros[1]]+1);
                    }
                }
            }
        }
        return dp[size][m][n];
    }
};
// 空间复杂度 O(lmn) ,l为数组长度 
```

==优化==：dp[k] [i] [j]= max( dp[k-1] [i] [j], dp[k-1] [i-zero] [j-ones] ); 只和上一个状态有关，用 一维滚动数组  代替  二维

```
class Solution {
// 01动态规划：dp[k][i][j] 代表 前 k个字符串中，有 i个0，j个1;
public:
    vector<int> get(string& s){
        vector<int>res(2);
        for(auto& c:s){
            res[c-'0']++;
        }
        return res;
    }
    int findMaxForm(vector<string>& strs, int m, int n) {
        int size=strs.size();
        vector<vector<int>> dp(m + 1, vector<int>(n + 1));
        for(int k=1;k<size+1;k++){
            vector<int>zeros=get(strs[k-1]);
            for(int i=m;i>=zeros[0];i--){
                for(int j=n;j>=zeros[1];j--){
                    dp[i][j]=max(dp[i][j],dp[i-zeros[0]][j-zeros[1]]+1);
                }
            }
        }
        return dp[m][n];
    }
};
```

### LC 1833. 雪糕的最大数量

<font color=blue size=4> 题目：</font> 

`n` 支雪糕，用长度为 `n` 的数组 `costs` 表示雪糕定价。其中，`costs [ i ]` 表示第 `i` 支雪糕的价格。给你价格数组 `costs` 和现金总额 `coins`，请你计算并返回 能买到的雪糕的最大数量。

 <font size=4 color=blue>思路：</font> 排序+贪心

```
class Solution {
// 贪心+排序
public:
    int maxIceCream(vector<int>& costs, int coins) {
        sort(costs.begin(),costs.end());
        int n=costs.size();
        if(coins<costs[0])
            return 0;
        int res=0;
        for(int i=0;i<n;i++){
            if(coins>=costs[i]){
                res++;
                coins-=costs[i];
            }
            else break;
        }
        return res;
    }
};
```



# “归并排序"

### 剑指Offer 51. 数组中的逆序对

<font color=blue size=4>题目：</font>

在数组中的两个数字，如果前面一个数字大于后面的数字，则这两个数字组成一个逆序对。输入一个数组，求出这个数组中的逆序对的总数。

```
class Solution {
    // 归并排序
private:
    int cnt=0;
    vector<int> temp;
public:
    int reversePairs(vector<int>& nums) {
        int size=nums.size();
        if(size<2) return 0;
        temp.resize(size);
        merge(nums,0,size-1);
        return cnt;
    }
    void merge(vector<int>& nums,int l,int r){
        if(l==r) return ;
        int mid=l+(r-l)/2;
        merge(nums,l,mid);        // 左边归并排序，使得左子序列有序
        merge(nums,mid+1,r);      // 右边归并排序，使得右子序列有序
        mergesort(nums,l,mid,r);  // 将两个有序子数组合并
    }
    void mergesort(vector<int>& nums,int l,int mid,int r){
        int i=0;  // 临时数组指针
        int p1=l;    //左序列指针 
        int p2=mid+1;   // 右序列指针
        while(p1<=mid && p2<=r){
            if(nums[p1]<=nums[p2]){
                temp[i++]=nums[p1++];
            }else{
                temp[i++]=nums[p2++];
                cnt+=(mid-p1+1);
            }
        }
        while(p1<=mid) temp[i++]=nums[p1++];
        while(p2<=r) temp[i++]=nums[p2++];
        for(int i=l;i<=r;i++){
            nums[i]=temp[i];
        }
    }
};
```

### LC 23. 合并K个升序链表（hard）

<font color=blue size=4>题目：</font>

给你一个链表数组，每个链表都已经按升序排列。请你将所有链表合并到一个升序链表中，返回合并后的链表。

**时间复杂度 O ( N logK ）**

K 条链表的总结点数是 N，**平均每条链表有 N/K 个节点**，因此合并两条链表的时间复杂度是 O(N/K)。从 K 条链表开始两两合并成 1条链表，因此**每条链表都会被合并 logK 次**，因此 **K 条链表会被合并 K * logK 次**，因此总共的时间复杂度是 K∗logK∗N/K 即 **O（NlogK）**

```
class Solution {
    // 二分法
public:
    ListNode* mergetwo(ListNode* l1,ListNode* l2){
        if(!l1) return l2;
        if(!l2) return l1;
        if(l1->val<l2->val){
            l1->next=mergetwo(l1->next,l2);
            return l1;
        }
        l2->next=mergetwo(l1,l2->next);
        return l2;
    }
    ListNode* merge(vector <ListNode*> &lists, int l, int r){
        if(l==r) return lists[l];
        if(l>r) return nullptr;
        int mid=(l+r)>>1;
        ListNode* l1=merge(lists,l,mid);
        ListNode* l2=merge(lists,mid+1,r);
        return mergetwo(l1,l2);
    }
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int n=lists.size();
        return merge(lists,0,n-1);
    }
};
```

