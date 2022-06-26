# 深入MySQL(Innodb)

<font color=red>**——2021.10.29 ThoughtWorks**</font>

==<font color=blue size=4>**什么是 逻辑表？物理表？**</font>==

1. 物理库 / 物理文件：存储在计算机中的。
2. 逻辑库 / 逻辑文件：给用户看的（即Database和Table就是我们常说的逻辑库的范畴）；
3. **逻辑表 可以理解为数据库中的视图。可以映射到一张或者多张物理表。这些表可以来自于不同的数据源。（主要出现在分库分表的情况下）**



https://www.cnblogs.com/rjzheng/p/9915754.html

**创建的索引，如复合索引、前缀索引、唯一索引，都属于非聚簇引（辅助索引）**；其数据结构为B+树。其中：<font color=red>**辅助索引的节点值保存的是 该索引字段对应的值 和 主键值。**</font>

<font size=4>**聚簇索引，在Mysql中是没有语句来另外生成的**</font>。在Innodb中，Mysql中的数据是按照主键的顺序来存放的。那么**<font color=blue size=4>聚簇索引就是按照每张表的主键来构造一颗B+树，叶子节点存放的就是整张表的行数据（也就是所有列值）。</font>**由于表里的数据只能按照一颗B+树排序，因此**<font color=blue>一张表只能有一个聚簇索引。</font>**(因为一张表只能存在一个主键key)。==聚簇索引默认就是主键索引==。

> 问：**如果我的表没建主键呢？**
>
> 答：**没有主键时，会用一个唯一且不为空的索引列做为主键**，成为此表的聚簇索引；
>
> ​       如果没有这样的索引，InnoDB 会隐式定义一个主键来作为聚簇索引

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210317211751261.png" alt="image-20210317211751261" style="zoom:67%;" />

<img src="image-20210317211822839.png" alt="image-20210317211822839" style="zoom:50%;" />

当我们执行：

```
select * from table where pId='11'
```

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210317212604963.png" alt="image-20210317212604963" style="zoom:55%;" />

接下来**引入非聚簇索引；**我们执行下面的语句

```
create index index_name on table(name);
```

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210317212656011.png" alt="image-20210317212656011" style="zoom:55%;" />

**InnoDB 根据索引字段生成一颗新的B+树**。因此， 我们**每加一个索引，就会增加表的体积， 占用磁盘存储空间**。然而，**注意看叶子节点，==非聚簇索引的叶子节点并不是真实数据==，它的叶子节点依然是索引节点**，**==存放的是该索引字段的值以及对应的主键索引==**(聚簇索引)。

```
select * from table where name='lisi'
```

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210317213113499.png" alt="image-20210317213113499" style="zoom:55%;" />

当我们查询 name 为 "lisi" 的用户信息时（学号，姓名，年龄），因为name上建了索引，先从name非聚集索引上，找到对应的主键id，然后根据主键id从聚集索引上找到对应的记录。

从**非聚集索引上找到对应的主键值然后到聚集索引上查找**对应记录的过程为**<font color=red>回表</font>**

**<font color=red size=4>什么情况不去聚簇索引树上查询呢？</font>**

```
select name from table where name='lisi'
```

此时在 非聚簇索引树上就可以查询到 name='lisi' 对应的name值，不需要回表。<font color=red>**覆盖索引（不用回表）**</font>

<font size=4>**当执行select col from table where col = ?，col上有索引的时候，效率比执行select \* from table where col = ? 速度快好几倍！**</font>



### 稠密索引 和 稀疏索引

**稠密索引**：可以加快搜索速度，但需要更多空间来存储索引记录。**数据库中的每个搜索键值都有一个索引记录。**

**稀疏索引：不会为每个关键字创建索引记录**。占用空间小，查询效率低。

在Innodb底层的B+树索引的两种类型中：

1. 聚集索引（主键索引）：即为稠密索引，因为主键 索引是所有值都不为空，每个主键都有对应的行记录。
2. 非聚集索引（辅助索引、复合索引）：稀疏索引，不会为表的每个值创建索引，而是为单个字段或多个字段创建，且行记录可以为null。当 where 条件不止单个条件的时候，我们首先通过索引查找出一批数据，然后通过顺序查找筛选。



## 基于主键索引和普通索引的查询有什么区别？

如上所示：执行下列语句

```
select * from table where name='lisi' 
```

需要先搜索 非聚簇索引树，得到主键值后再回表到 聚簇索引树进行查询。效率比主键索引慢得多。<font color=red>**普通索引有回表过程。**</font>



## 为什么不是红黑树

**<font color=red size=4>B树的优势：多路查找，一个节点可以放多个 Key</font>**，这是优于红黑树的具体原因。**B树每个结点有多个key，而红黑树每个结点有一个key**，那么随着数据的不断增多，**红黑树的高度不断增加，效率不断降低（最坏情况下为O(n)），**而**<font color=red>B树的高度一般都很低</font>**，为甚？我一个结点放上1024个key，满了才分裂一次。

==红黑树/ 平衡二叉树 都没有利用  磁盘预读功能；而 <font size=4>**B树可以充分利用磁盘预读**</font>。==

<font size=4>索引存在索引文件中，是存在磁盘中的</font>。因为**索引通常很大，无法一次将全部索引加载**到内存中，因此**每次只能从磁盘中读取一个磁盘页的数据到内存。**

**而 适合作为索引的结构应该是  尽可能少地执行磁盘I/O操作。**

<font size=5>**磁盘预读**</font>：因为 磁盘的存取速度很慢，为了提高效率，减少磁盘IO。磁盘往往不是按需读取，即使只需要一个字节，磁盘也会顺序向后读取 一定长度的数据放入内存中。**依据 局部性原理；**

而 红黑树，树的高度明显要深得多；由于 逻辑上很近的节点，物理上可能很远，无法利用局部性原理。

而 **<font color=red>B树 将节点大小设置为 磁盘页的大小，就是充分利用 磁盘预读的功能</font>，每次读取磁盘页，就会读取一整个节点。**



<font size=4>**先补充知识点：**</font>

​		【==<font color=red size=4>**磁盘 IO 一次读取一个磁盘页的数据，而数据库的节点大小就等于一个页**</font>==； <font size=4>内存分配是按页对齐的，这样就实现了**一个节点只需要一次 IO**</font>；因为 **B+树**的 中间结点，不存放data 值，所以**一个节点可以存放更多的 key**，所以**一次 IO操作可以读取更多的数据**，所以**可以减少 IO次数；IO次数取决于 B+数的高度**。】



## 深度为 3 的B+ Tree 索引 可以存储多少条记录

分析：==**InnoDB 存储引擎中的页大小为 16 KB**==，表的主键一般为 INT(4字节) 或 BIGINT（8字节），指针（4字节|8字节）；

所以，==<font color=red size=4>**一个页中大概存储 16KB /(8KB + 8KB) = 1 K个键值。**</font>==

深度为 3 的B+ Tree可以维护 10^3 * 10^3 * 10^3=10 亿条记录。



## B+的维护（增加 | 删除元素）

https://www.cnblogs.com/lianzhilei/p/11250589.html

<font size=4 color=blue>插入：</font>

针对 m 阶 高度 h 的B树，插入一个元素时，首先在B树中是否存在，如果不存在，即在叶子结点处结束，然后在叶子结点中插入该新的元素。

- 若该节点元素个数小于m-1，直接插入；
- 若该节点元素个数等于m-1，**引起节点分裂；以该节点中间元素为分界，取中间元素**（偶数个数，中间两个随机选取）**插入到父节点**中；
- 重复上面动作，直到所有节点符合B树的规则；**最坏的情况一直分裂到根节点，生成新的根节点，高度增加1**；

5阶B树关键点:

- 2<=根节点子节点个数<=5
- 3<=内节点子节点个数<=5
- 1<=根节点元素个数<=4
- 2<=非根节点元素个数<=4

<font size=4 color=blue>删除：</font>

- 某结点中元素数目小于（m/2）-1，(m/2)向上取整，则需要看其某相邻兄弟结点是否丰满；
- 如果丰满（结点中元素个数大于(m/2)-1），则向父节点借一个元素来满足条件；
- 如果其相邻兄弟都不丰满，即其结点数目等于(m/2)-1，则该结点与其相邻的某一兄弟结点进行“合并”成一个结点；

5阶B树为例，详细讲解删除的动作；

- 关键要领，元素个数小于 2（m/2 -1）就合并，大于4（m-1）就分裂；

  

**<font color=red size=4>一棵含有N个总关键字数的m阶的B树的最大高度是多少？</font>**

　**<font color=purple>　log（m/2）(N+1)/2 + 1 ，log以（m/2）为低，(N+1)/2的对数再加1</font>**



## B树和B+树的区别（重点）

<font size=5>==B+树的优势：==</font>

1. **相比于B树，每个非叶子节点可以存储更多的 key关键字，所以树的高度就越低，所以查询的 IO 次数更少（原因见 补充的知识点）**，这就使得它更适合做为数据库MySQL的底层数据结构了。（<font color=purple>**B+ 树的非叶子节点只存了 key，data 存储在叶子节点；而 B树的 非叶子节点存储 key+ data**</font>）。
2. <font size=4>**所有的查询都要查找到叶子节点**，**查询性能是稳定的**</font>，而B树，每个节点都可以查找到数据，所以不稳定。
3. **所有的叶子节点形成了一个有序链表**（节点之间有指针连接 ），更加便于<font size=4>==**范围查找。**==</font>
4. **<font color=red size=4>B树 必须用中序遍历的方法按序扫库</font>，而B+树的叶子节点使用指针顺序链接在一起，<font color=red>只要遍历叶子节点就可以实现整棵树的遍历（直接从叶子结点挨个扫一遍就完了）</font>**；换句话说，<font size=4>**B+树全节点遍历更快：B+树遍历整棵树只需要遍历所有的叶子节点即可**，而不需要像B树一样需要对每一层进行遍历，这**有利于数据库做全表扫描**。</font>
5. B+树支持range-query非常方便，而B树不支持。这是数据库选用B+树的最主要原因。

一棵B+ 树：

<img src="https://img-blog.csdn.net/20160202205105560" alt="索引" style="zoom:80%;" />

### **m阶B+树定义**

B+树是B树的一种变形形式，m阶B+树满足以下条件：

(1) <font size=4 color=purple>**每个结点至多有m个孩子。**</font>（所以，看 一棵 B+树是几阶的，最重要的是看，每个结点最多的孩子数是多少）

(2) 除根节点和叶结点外，每个结点至少有（m+1）/2个孩子。

(3) 如果根节点不为空，根结点至少有两个孩子。

(4) 所有叶子结点增加一个链指针，所有关键字都在叶子结点出现。

#### **结点的分裂**

- 将已满结点进行分裂，**将已满节点后M/2节点生成一个新节点，将新节点的第一个元素指向父节点。**
- 父节点出现已满，将父节点继续分裂。
- 一直分裂，如果根节点已满，则需要分类根节点，此时树的高度增加。

#### B+ 树的时间复杂度

假设一个含有`N`个值，阶为`n`的B+树

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210324145107849.png" alt="image-20210324145107849" style="zoom:67%;" />

## 最左匹配原则的底层原理

https://blog.csdn.net/zzti_erlie/article/details/83718947?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-16.control&dist_request_id=1328665.18778.16160298177745045&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-16.control

重点：最左匹配原则生效的原因是:

<font color=blue>**数据库会根据 最左字段构建索引树**</font>

首先：**最左匹配原则 是针对联合索引**来说的。

- 概念：**mysql会一直向右匹配直到遇到范围查询**；（>、<、between、like）就停止匹配。

  > ==举个例子：==
  > 	建立联合索引（a,b,c,d），如果**查询  a=3 and b=4 and c>5 and d=6，d是用不到做索引的**
  > 	但是如果建立（a,b,d,c），则都可以用到，**abd的顺序可以任意调整**

  联合索引也是一颗B+树，只不过==<font color=red size=4>**联合索引的key不是一个，而是多个**</font>==。**==构建一颗B+树只能根据一个key来构建，<font color=red size=4>因此数据库依据联合索引最左的字段来构建B+树。</font>==**

假设 teacher表定义如下，在 **name 和age列上建立联合索引**

```
CREATE TABLE `teacher` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '教师编号',
  `name` varchar(10) NOT NULL COMMENT '教师姓名',
  `age` int(11) NOT NULL COMMENT '教师年龄',
  `ismale` tinyint(3) NOT NULL COMMENT '是否男性',
  PRIMARY KEY (`id`),      //  主键索引（聚簇索引）
  KEY `idx_name_age` (`name`, `age`)    //  创建(name,age)的联合索引
 ) ENGINE=InnoDB;
```

插入如下 SQL

```
insert into teacher (`name`, `age`, `ismale`) value('aa', 10, 1);
insert into teacher (`name`, `age`, `ismale`) value('dd', 12, 0);
insert into teacher (`name`, `age`, `ismale`) value('cb', 9, 1);
insert into teacher (`name`, `age`, `ismale`) value('cb', 15, 1);
insert into teacher (`name`, `age`, `ismale`) value('bc', 17, 0);
insert into teacher (`name`, `age`, `ismale`) value('bb', 15, 1);
insert into teacher (`name`, `age`, `ismale`) value('dd', 15, 1);
insert into teacher (`name`, `age`, `ismale`) value('dd', 12, 0);
```

<img src="https://img-blog.csdnimg.cn/20201209223326922.png?" alt="在这里插入图片描述" style="zoom:67%;" />

目录页由  name列，age列，页号 这三部分组成。目录**会先按照name列进行排序，当name列相同的时候才对age列进行排序。**

数据页由  name列，age列，主键值 这三部分组成。同样的，数据页会先按照name列进行排序，当name列相同的时候才对age列进行排序。

由 name,age 创建的联合索引的B+树结构大概如下所示：

<img src="https://img-blog.csdnimg.cn/20201209224211722.png?" alt="在这里插入图片描述" style="zoom:85%;" />

1. 从上图的 联合索引树中可以看出，**name 的值是有序的，而 age 的值是无序的**。所以当查找条件 age==12时，无法用到索引。因为此时创建的联合索引是按照 name 排序的。
2. 同时我们还可以发现在 name 相等的情况下，age 值又是按顺序排列的，但是这种顺序是相对的。所以**最左匹配原则遇上范围查询就会停止，剩下的字段都无法使用索引**。例如<font color=red>**a = 1 and b = 2；a，b字段 都可以使用索引，因为在a值确定的情况下，b是相对有序的；而a>1and b=2，a字段可以匹配上索引，但b值不可以，因为a的值是一个范围，在这个范围中b是无序的。**</font>（==重点理解）==

当执行如下语句的时候，会回表

```
select * from student where name = 'aa';
```

当执行如下语句的时候，没有回表的过程

```
select name, age from student where name = 'aa';
```

**为什么不需要回表？**

因为  idx_name_age 索引的叶子节点存的值为主键值，name值和 age 值，所以**从idx_name_age索引上就能获取到所需要的列值**，不需要回表，即**<font color=red>索引覆盖</font>**。



## 联合索引的好处

- <font color=blue size=4>**索引覆盖**</font>，减少了很多回表的操作，提高了查询的效率。
- <font color=blue size=4>**索引下推**</font>，索引列越多，通过索引筛选出的数据越少。(在 where 语句中做判断)



### 覆盖索引、索引下推

#### ==覆盖索引：==

​	覆盖索引就是**从索引中直接获取查询结果**。覆盖索引不需要回表。因为辅助索引（二级索引）保存了 索引值和主键值。如果能直接在 二级索引上命中，就不必对主键进行二次查询（回表）,提高查询效率。

要使用覆盖索引需要注意：

1. <font color=blue>select**查询列中包含在索引列**中</font>；
2. **where条件包含索引列或者复合索引的前导列；**

**覆盖索引可以减少树的搜索次数，显著提升查询性能**

#### ==索引下推：==

可以<font color=orange>在遍历索引的过程中，**对索引中包含的字段做判断，直接过滤掉不满足条件的数据，减少回表次数**</font>，如下图:

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210318101305337.png" alt="image-20210318101305337" style="zoom:70%;" />



## 索引为什么会失效？

1. **存在 函数操作或者 隐式类型转换**
2. **前导模糊查询不会命中索引**，例如 **like %李**
3. **负向条件索引不会命中索引，建议用 in**。负向条件有：!=、<>、not in、not exists、not like 等。
4. 避免**在  where 子句中，对字段进行 null 值判断**。否则将导致全文索引。

索引是按照一定规则排好序的，如果**对索引列使用函数，或者 like % 李，具体的值都不知道，它怎么在B+树上加速查询？**

针对 负向条件索引无法命中索引，建议使用 in 语句。举例子：

```
EXPLAIN SELECT * FROM user WHERE status!=1 AND status!=2
```

可以优化为 in 查询

```
EXPLAIN SELECT * FROM user WHERE status IN(0,3,4);
```

==<font color=red size=4>**补充：这是我混淆的点：**</font>==

**范围条件查询可以命中索引：**

```
EXPLAIN SELECT * FROM user WHERE status>5 ;
```

但是对于以下的情况：

```
select * from table where a>1 and b=2;  // a可以用到索引，但b用不到
```



## 如何判断索引失效（explain 查询语句）看 type ,key,extra

<font color=blue>**用 explain 查看执行时用到的 key。**</font> **explain关键字可以模拟优化器执行SQL查询语句。**

```
explain select * from people order by name
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/2018110715593476.PNG)

<font color=blue>**Extra列只有Using index，即根据索引顺序进行扫描**</font>（不会回表），type=index

```
explain select * from people order by address
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/20181107162452889.PNG)

Extra列 有 <font color=blue>**Using filesort（索引失效）**</font>。

1. 因为索引的结构是B+树，索引中的数据是按照一定顺序进行排列的，EXPLAIN分析查询时，Extra显示为Using index。
2. 所有不是通过索引直接返回排序结果的操作都是Filesort排序（文件排序），也就是说进行了额外的排序操作

## 索引越多越好？

答案是否定的。

- 数据量小的表不需要建立索引，建立会增加额外的索引开销

- 数据变更  需要 维护索引，更多的索引意味着更多的维护成本

- **更多的索引 需要更多的存储空间。**（因为索引会占用磁盘空间）

  

## 慢查询

概念：MySQL的慢查询日志是MySQL提供的一种日志记录，它**用来记录在MySQL中响应时间超过阀值的语句**，具体指运行时间超过**long_query_time值的SQL**，则会被记录到慢查询日志中。

#### 导致慢查询的原因：

1. 没有命中索引，导致全表扫描；（用 explain 语句查询是否 命中索引）
2. 查询出的数据量过大（采用多次查询，降低数据量） 

#### 解决方法：

1. 优化查询语句，或 **创建联合索引**
2. 分解关联查询（  **将一个大的查询分解为多个小查询**是很有必要的。对关联查询进行分解，就是可以对每一个表进行一次单表查询，然后将查询结果在应用程序中进行关联 ）

#### 查询的优化

- 尽量避免在 where 子句中 使用 != 等负向条件 的索引；

- **避免在  where 子句中，对字段进行 null 值判断**。否则将导致全文索引。

  

## 如何创建索引

用  <font size=4>**alter table** /  **create index**。</font>

```
alter table table_name add primary key(column_list) #添加一个主键索引
alter table table_name add index (column_list)      #添加一个普通索引
alter table table_name add unique (column_list)     #添加一个唯一索引
```

```
create index index_name on table_name (column_list)   #创建一个普通索引
create unique index_name on table_name (column_list)  #创建一个唯一索引
```

<font size=4 color=purple>索引可以索引整个列的数据，也可以只索引一个列开头部分的数据</font>

```
alter table t add key idx_b(b(100));  // 只给 b列的前100个字段创建索引
```

对于索引的添加   或  删除操作，MySQL先创建一张新的临时表，然后把数据导入临时表，删除原表，再把临时表



#### 什么时候应该创建索引呢？

<font size=4>如果<font color=red>某个字段的取值范围很广</font>，几乎没有重复，即高选择性，<font color=red>且仅取出表中少部分数据的时候。</font></font>

# 一条SQL查询语句是如何执行的？

https://blog.csdn.net/weixin_38990431/article/details/89050101

### 1.1 SQL 语句的查询过程

<img src="https://img-blog.csdnimg.cn/20190405160716135.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl8zODk5MDQzMQ==,size_16,color_FFFFFF,t_70" alt="在这里插入图片描述" style="zoom:80%;" />

1. <font color=red>**Server 层包括**</font>**连接器、查询缓存、分析器、优化器、执行器**等，以及所有的内置函数（如日期、时间、数学和加密函数等），所有跨存储引擎的功能，比如存储过程、触发器、视图等。
2. **存储引擎层负责数据的存储和提取。**

### 1.2 SERVER 层

#### 连接器

​	负责跟客户端建立连接、获取权限、维持和管理连接。

```
mysql -hlocalhost -uroot -p
```

​	如果用户名密码认证通过，连接器会到权限表里面查出你拥有的权限。之后，这个连接里面的权限判断逻辑，都将依赖于此时读到的权限。对这个用户的权限做了修改，也不会影响已经存在连接的权限。修改完成后，只有再新建的连接才会使用新的权限设置。

> 在使用中要尽量减少建立连接的动作，多使用长连接



#### 查询缓存

MySQL 拿到一个查询请求后，会先到查询缓存看看，之前是不是执行过这条语句。<font color=blue>**之前执行过的语句及其结果可能会以 key-value 对的形式，被直接缓存在内存中（MYSQL 8.0 后不支持查询缓存）**</font>。查询缓存的失效非常频繁，**只要对表有更新，这个表上所有的查询缓存都会被清空**。

#### 分析器

1. **词法分析**：从"SELECT" 识别是查询语句，把字符串“T”识别成“表名T”，把字符串“ID”识别成“列ID”
2. **语法分析**：根据与语法规则判断SQL语句是否满足MySQL语法

#### 优化器

优化器在表里**有多个索引**的时候，**决定使用哪个索引**；或者在一个语句有多表关联（join）的时候，**决定各个表的连接顺序。**

#### 执行器

1. 在执行之前，判断时候有执行的权限；
2. 命中查询缓存，在返回结果时做权限验证；
3. 查询会在优化器之前调用precheck验证权限；

### 举例子：

```
SELECT * FROM T WHERE ID=10
```

**==执行器流程：==**

- 调用 InnoDB 引擎接口取这个表的第一行，判断 ID 值是不是 10，如果不是则跳过，如果是则将这行存在结果集中；
- 调用引擎接口取“下一行”，重复相同的判断逻辑，**直到取到这个表的最后一行**。
- **执行器将上述遍历过程中所有满足条件的行组成的记录集作为结果集返回给客户端**。
- 对有索引的表，第一次调用的是“取满足条件的第一行”这个接口，之后循环取“满足条件的下一行”这个接口，这些接口都是引擎中已经定义好的。



# 日志模块

### redo log

**<font color=blue>  循环写，有 crash-safe 能力</font>**

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210318153412815.png" alt="image-20210318153412815" style="zoom:50%;" />

#### 为什么redo log 是crash safe？

​	redo log 是循环写，**redo log 只会记录未刷盘的日志**，已经刷入磁盘的数据都会从 redo log 这个有限大小的日志文件里删除。

当数据库 crash 后，想要恢复**未刷盘但已经写入 redo log 和 binlog 的数据**到内存时，binlog 是无法恢复的。<font color=blue>虽然 binlog 拥有全量的日志，但没有一个标志让 innoDB 判断哪些数据已经刷盘，哪些数据还没有。</font>

###### 如何恢复未刷盘的数据？

根据 redo log 的两阶段提交的状态。



#### redolog 两阶段提交的原因（如果不用，则产生数据不一致）

**redolog   有 prepare 和 commit两种状态**（==保证 binlog 和 redolog 的一致性==），<font color=purple>**事务操作完成并且 binlog 写入完成时，redolog会从 prepare 状态转化为 commit 状态**</font>。

> 由于 redo log 和 binlog 是两个独立的逻辑，如果不用两阶段提交，**要么就是先写完 redo log 再写binlog ，或者采用反过来的顺序，会产生日志与数据不一致的问题，**例如：
>
> **先写 redo log 后写 binlog**，假设在 redo log 写完， **binlog 还没有写完的时候，MySQL进程异常重启**，redo log写完后，仍能够把数据恢复回来。但是**binlog就没有记录这条语句，如果需要用这个 binlog 来恢复临时库的话**，由于这个语句的 binlog 丢失，会导致数据不一致
> **先写 binlog 后写 redo log** 。如果在 binlog 写完之后 crash ，由于 redo log 还没写，崩溃恢复以后这个事务无效，单用 binlog 来恢复时就多了一个事务出来

<img src="https://img-blog.csdnimg.cn/20190418193314931.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2hhbnBlaXl1MTk5NQ==,size_16,color_FFFFFF,t_70" alt="img" style="zoom:40%;" />

<font size=4>分析这张图：</font>

1. **时刻 A 的地方**，也就是写入 redo log  处于 prepare 阶段之后、写 binlog 之前，**发生了崩溃**（ crash ），由于此时 binlog 还没写， redo log 也还没提交，所以崩溃恢复的时候，这个**事务会回滚**。这时候， binlog 还没写，所以也不会传到备库。
2. **在时刻 B** ，也就是 binlog 写完， redo log **还没 commit 前发生crash** ，崩溃恢复过程中事务还是会被提交。（先从 binlog 恢复 redo log，再从 redo log 恢复 change buffer。）

### bin log（二进制日志）

<font size=4 color=red>学习链接：</font>   https://www.modb.pro/db/68471  

**<font color=blue>binlog是Server层的。binlog是记录所有数据库表结构变更（例如CREATE、ALTER TABLE…）以及表数据修改（INSERT、UPDATE、DELETE…）的二进制日志。</font> <font size=4 color=red>即：，binlog记录了所有DML（ SELECT、UPDATE、INSERT、DELETE）和DDL（CREATE、ALTER、DROP）操作，其中DML是详细的具体行，DDL只记录语句。</font>**

binlog 的作用：

1. 在**主从复制中**，从库利用主库上的 binlog进行重播，实现主从同步。
2. 用于数据库的基于时间点的**数据恢复**。

binlog 有三种模式：**Statement（基于 SQL 语句的复制）、Row（基于行的复制） 以及 Mixed（混合模式）。**

<font size=4>**不同之处：**</font>

- **==redo log 是InnoDB 引擎特有的；binlog 是Server 层实现的，所有引擎都可以使用==。**
- **redo log 是物理日志，记录的是”在某个数据页上做了什么修改“；binlog 是逻辑日志，记录的是 原始的SQL语句。**
- redo log 是**<font color=blue>循环写</font>**，空间固定会用完；而 binlog 是可以**<font color=blue>追加写的</font>**。binlog 文件写到一定大小会切换到下一个，并不会覆盖以前的日志。（redo log 只会记录未刷盘的日志，已经刷入磁盘的数据都会从 redo log 这个有限大小的日志文件里删除。binlog 是追加日志，保存的是全量的日志。）
- ==<font color=red>**redo log**</font> 在 <font color=red>**事务执行过程中** </font>会**不断的写入**，而 <font color=red>**binlog**</font> 是在<font color=red> **事务最终提交前** </font>写入的==。

**事务执行过程中，binlog 首先会被写到 binlog cache 中。事务提交的时候（commit），再将binlog cache 写到 binlog 文件中。一个事务的 binlog 是原子的，无论多大都需要保证完整性。**

<font color=blue size=4>**binlog**</font> 有三种日志格式：<font color=red size=4>**statement、row、mixed**</font>

<font size=4 color=red>statement：</font>**只记录了 响应变更的语句，而不记录具体的行**（优点：只记录执行语句，减少了binlog 日志量 和 IO次数；缺点：只记录了执行语句，主从同步时，还会额外需要执行语句对应的上下文环境，麻烦**影响主从复制同步**）

<font size=4 color=red>row：</font>该格式下**只记录涉及的行**，不记录具体的sql。这样做的优点， 是不记录执行的sql语句的上下文相关的信息，**仅需要记录那一条记录被修改成什么了**。

<font size=4 color=red>mixed：</font> 两种方式混合使用。**一般的语句修改使用statment格式保存binlog**，如**一些函数**，statement无法完成主从复制的操作，**则采用row格式保存binlog**，MySQL会根据执行的每一条具体的sql语句来区分对待记录的日志形式，也就是在Statement和Row之间选择一种。

### 怎么查看binlog

MySQl提供了命令`mysqlbinlog`，进行查看。针对ROW格式，加上`-vv`



### MySQL 怎么知道 binlog 是完整的 ?(binlog-checksum)

一个事务的 ==binlog 是有完整格式的：==

**在事务提交时**，不管是STATEMENT还是ROW格式的binlog，都会**在末尾添加一个XID_EVENT事件代表事务的结束**。



### redo log  和 bin log 是怎么关联起来的 ?

它们有一个<font color=blue>**共同的数据字段，叫 XID 。崩溃恢复的时候，会按顺序扫描 redo log**</font>：

- 如果碰到既有 prepare 、又有 commit 的 redo log ，就直接提交；
- 如果碰到只有 parepare 、而没有 commit 的 redo log ，**就拿着 XID 去 binlog 找对应的事务。判断binlog是否完整**

### binlog的写入机制

事务执行过程中，**先把日志写到 binlog cache** ，事务提交的时候，**再把 binlog cache 写到 binlog 文件中**。

> **系统给 binlog cache 分配了一片内存，每个线程一个，**参数 binlog_cache_size 用于控制单个线程内 binlog cache 所占内存的大小
>
> 如果超过了这个参数规定的大小，就要暂存到磁盘，事务提交的时候，把 binlog cache 里的完整事务写入到 binlog 中，并清空 binlog cache

<font size=4>**每个线程有自己 binlog cache （ binlog cache 是每个线程自己维护的），但是共用同一份 binlog 文件，保证了一个事务的binlog不能被拆开。**</font>

<img src="https://img-blog.csdnimg.cn/20190424162231963.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2hhbnBlaXl1MTk5NQ==,size_16,color_FFFFFF,t_70" alt="img" style="zoom:50%;" />

### 什么是change buffer

当需要更新一个记录，就要更新一个数据页：

- 如果数据页在内存中（buffer pool中时）就直接更新。
- 如果这个数据页还没有在内存中（没有在buffer pool中）。InooDB 会<font color=blue>**将这些更新操作语句缓存在 change buffer 中**</font>。**在下次查询**需要访问这个数据页时，**将数据页读入内存，然后执行 change buffer** 中与这个页有关的操作。

将 change buffer 中的操作应用到原数据页，得到最新结果的过程称为 **merge**，**避免大量的磁盘随机访问I/O**。

而**==唯一索引的更新就不能使用 change buffer==**：对于唯一索引，**所有的更新操作都要先判断这个操作是否违反唯一性约束。**那么必须将数据页读入内存才能判断。比如，要插入(4,400) 这个记录，就要先判断现在表中是否已经存在 k=4 的记录。

**==只有普通索引才能使用change buffer。==**



# join 和 left join 的区别

- <font size=4>join，是返回两个表中都有的符合条件的行</font>
- **<font size=4> left join，是返回左表中所有的行和右表中符合条件的行</font>**

<font size=4>left join 比 join 返回的行数多。</font>



# 数据库的三大范式

- **第一范式**：<font size=4 color=red>要求表中字段具有原子性，不可再分割</font>；
- **第二范式**：<font size=4 color=red>要求记录有唯一标识，即实体的唯一性，即不存在部分依赖</font>；（即：<font size=4>要有主键，要求其他字段都依赖于主键</font>；因为主键具有 唯一性，其他字段 依赖主键，所以其他字段也具有唯一性）就是说其他字段必须和它们的主键相关。因为不相关的东西不应该放在一行记录里。
- **第三范式**：<font size=4 color=red>消除传递依赖（消除冗余）</font>即，**<font size=4>各种信息只在一个地方存储，不出现在多张表中。</font>**



# 主从复制原理

https://zhuanlan.zhihu.com/p/96212530

### 0、为什么需要 主从复制？

<font size=4>使用主从复制，<font color=red>让主库负责写，从库负责读</font>，这样，**即使主库出现了锁表的情景，通过读从库也可以保证业务的正常**运作。</font>

### 1、什么是主从复制？

​		MySQL 主从复制是指<font color=red size=4>数据可以从一个MySQL数据库服务器主节点（Master）复制到一个或多个从节点（Slave）</font>。MySQL 默认采用**异步复制方式**，这样从节点不用一直访问主服务器来更新自己的数据，**数据的更新可以在远程连接上进行**，从节点可以复制主数据库中的所有数据库或者特定的数据库，或者特定的表。

### 2、复制的原理

（1）<font size=4>当master上的数据发生改变时，则**将数据更新写入二进制日志 binlog中**；</font>

（2）<font size=4>**slave服务器会在一定时间间隔内对master的 binlog进行探测其是否发生改变**，如果发生改变，则开始一个I/OThread请求master二进制事件</font>

（3）<font size=4>同时主节点为每个I/O线程启动一个dump线程，用于向其发送二进制事件，并保存至从节点本地的中继日志中，从节点将启动SQL线程从中继日志中读取二进制日志，在本地重放，使得其数据和主节点的保持一致，最后I/OThread  和  SQL Thread  将进入睡眠状态，等待下一次被唤醒。</font>

- <font size=4 color=purple>**从库（Slave）会生成两个线程,一个I/O线程，一个SQL线程；主库（Master）生成一个线程 给 从库  传binlog **</font>
- <font size=4>**I/O线程会去请求主库的binlog**，并将得到的binlog**写到本地的relay-log(中继日志)**文件中；</font>
- <font size=4>**主库会生成一个log dump线程**，用来给从库I/O线程传  binlog；</font>
- <font size=4>**SQL线程**，负责从relay log日志里读出binlog内容，并更新到slave的数据库里；</font>

<img src="https://pianshen.com/images/646/8229a3fa279b77dd889b6d16d17b7ba6.png" alt="img" style="zoom:67%;" />

1. 从库通过手工执行**change master to 语句连接主库**，提供了连接的用户一切条件（user 、password、port、ip），并且让从库知道，**二进制日志的起点位置（file名 position 号）； start slave**
2. 从库的IO线程和主库的dump线程建立连接。

### **mysql  主从同步延时分析**

mysql的主从复制都是<font size=4>**单线程的操作**</font>，主库**对所有  DDL  和   DML   产生的日志写进binlog**，由于binlog是顺序写，所以效率很高，**Slave的SQL线程  将 主库的DDL和DML 操作事件在Slave中重放**。DML和DDL的**IO操作是随机的，不是顺序，所以成本要高很多；**

另一方面，由于**SQL 线程也是单线程的，当主库的并发较高时，产生的 DML 数量超过slave的SQL 线程所能处理的速度**，或者当slave中有大型query语句**产生了锁等待，那么延时就产生了**。

#### 解决方案

1. 业务的持久化层的实现采用**分库**架构，mysql服务可平行扩展，分散压力。
2. **单个库读写分离，一主多从，主写从读，分散压力**。这样从库压力比主库高，保护主库。
3. 服务的基础架构**在   业务和mysql之间       加入memcache  或   redis的cache层。降低mysql的读压力**。
4. 不同业务的mysql物理上放在不同机器，分散压力。



# InnoDB 全表 COUNT( * )

<img src="https://user-gold-cdn.xitu.io/2019/4/6/169f3114a5adb1e9?w=964&h=285&f=png&s=9961" alt="img" style="zoom:80%;" />



<font size=4>**关于 MyISAM 的 count 函数**</font>

​		MyISAM的COUNT()函数总是非常快，不过有前提条件的，即<font color=red>**只有没有任何where条件的COUNT()才非常快**，因为此时无需实际地去计算表的行数。MySQL可以利用存储引擎的特性直接获得这个值（**MyISAM可以把表的总行数单独记录下来**）</font>。如果MySQL知道某列col不可能为NULL值，那么MySQL内部会将COUNT(col)表达式优化为COUNT()。**当统计带WHERE子句的结果集行数，MySQL的COUNT()和其它存储引擎没有任何不同。**



<font size=4 color=red>**Innodb  count慢的原因:**</font>

​		Innodb为聚簇索引同时支持事物，其**在count指令实现上采用实时统计方式**。**在无可用的二级索引情况下，执行  count  会使MySQL扫描全表数据**，当数据中存在大字段或字段较多时候，其效率非常低下（每个页只能包含较少的数据条数，需要访问的物理页较多）。

​		 **二级索引存储的数据为索引值与主键值**。当我们**通过二级索引统计数据的时候，无需扫描数据文件**；而**通过主键索引统计数据时**，由于**主键索引与数据文件存放在一起，所以每次都会扫描数据文件**，故<font color=red>大多数情况下，通过二级索引统计数据效率 >= 基于主键统计效率 </font>

<font size=4>**可优化点：**</font>

1. **主键需要**采用**占用空间尽量**
2. **小的类型且数据具有连续性**（推荐自增整形id），这样有利于**减少页分裂、页内数据移动**，可加快插入速度同时有利于增加二级索引密度（一个数据页上可以存储更多的数据）。
3. 在表包含大字段或字段较多情况下，若存在count统计需求，**可建一个较小字段的二级索引**（例 char(1) , tinyint )来进行count统计加速。

**NOTE:**

​		在 InnoDB 中 COUNT(*)  和  COUNT(1)  实现上没有区别，而且效率一样，但是 COUNT(字段)  需要进行字段的非NULL判断，所以效率会低一些。

**COUNT（1）中的 1 代表 查询的表里的第一个字段**。这里用1，也是为了方便，当然如果数据量较大的话，也可以提高速度，因为写**COUNT（*）的话会所有列扫描**。但在 InnoDB中，两者效率没有区别。



# <font color=purple>mysql 自增主键原理（AUTO_INC锁）</font>

在 **MySql 5.1版本之前**，用 AUTO_INCREMENT 修饰的数据列确实是严格连续自增的。MySql 的实现是会针对每个插入语句加一个**全表锁**，这个锁**可以保证每次只有一条插入语句在执行**，每插入一行数据，就会生成一个主键ID。

 举个例子：

```
mysql> CREATE TABLE t1 (

-> c1 INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,

-> c2 CHAR(1)

-> ) ENGINE=InnoDB AUTO_INCREMENT=100;
```

```
mysql> INSERT INTO t1 (c1,c2) VALUES (NULL,'a'), (NULL,'b'), (NULL,'c'), (NULL,'d');
```

> 针对这条MySql执行的流程为：<font color=red>全表加 AUTO-INC锁</font>
>
> 1.1 生成主键ID：101
>
> 1.2 将行(101, 'a')插入表中
>
> 1.3 生成主键ID: 102
>
> 1.4 将行(102, 'b')插入表中
>
> ...
>
> <font color=red>释放 AUTO-INC锁</font>

使用 AUTO_INC 全表锁进行 主键自增 ，并发性是最差的。

5.1 之后的版本 ，**对于简单语句的插入**（指的是，插入的的数据行数是可以提前确定的），不再加全表的AUTO-INC锁，只会在产生自增列数据的时候**加一个轻量级的互斥锁，等自增数据分配好，锁就释放了**，因此像上面的例子，在MySql5.1之后的执行流程如下加轻量级互斥锁。

> 1.1 分配自增数据
>
> 释放锁
>
> 将行(101, 'a')插入表中
>
> 将行(102, 'b')插入表中

在语句真正执行前，先加锁，分配好自增数据，再释放锁，再执行SQL语句。采用此方法，并发情况下的临界区变小了，且不再持有全表的锁，提升了并发性能。但对于复杂的插入，**INSERT ... SELECT ，无法提前知道插入的数据行，还是需要全表锁**。

在 MYSQL 8.1之后，默认的主从复制策略变成了基于数据行实现。对于 INSERT ... SELECT 语句，也采用 轻量级的锁。



# Read Committed 和 Repeatable Read 隔离级别

https://blog.csdn.net/Baisitao_/article/details/104723795?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-8.control&spm=1001.2101.3001.4242

### 在 RR 隔离级别是怎么解决幻读的呢？（间隙锁）

默认情况下，InnoDB工作在Repeatable Read的隔离级别下，并且**以Next-Key Lock的方式对索引行进行加锁**。当查询的索引具有唯一性（主键、唯一索引）时，Innodb存储引擎会对Next-Key Lock进行优化，将其降为行锁，仅仅锁住索引本身，而不是范围（除非锁定不存在的值）。若是普通索引，则会使用Next-Key Lock将记录和间隙一起锁定。

#### 1、共享锁 

```
SELECT ... LOCK IN SHARE MODE;
```

#### 2、排它锁

```
SELECT ... FOR UPDATE;
```

### MVCC

<font color=red>**MVCC 只在  Read Committed 和 Repeatable Read 两个隔离级别下工作。**</font>

<font size=4>==MVCC 的实现，依赖 隐藏字段、Read View、Undo log。==</font>

#### 1、隐藏字段

<img src="https://i.loli.net/2021/07/08/Iv8bQepF1wMx3XY.png" alt="image-20210708231047327" style="zoom: 67%;" />

- **DB_TRX_ID** (6字节)：表示最近一次对本记录行作修改（insert | update）的事务ID。**InnoDB会把 delete 操作认作 update，不过会更新一个另外的删除位，将行表示为deleted。并非真正删除。**
- **DB_ROLL_PTR** (7字节)：回滚指针，指向当前记录行的 undo log 信息
- **DB_ROW_ID** (6字节)：随着新行插入而单调递增的行ID

<font size=4>由于`undo log`中记录了各个版本的数据，并且通过`DB_ROLL_PTR`可以找到各个历史版本，并且由`DB_TRX_ID`决定使用哪个版本（快照）。所以相当于`undo log`实现了MVCC，MVCC实现了快照读。</font>



#### 3、Undo log

Undo log 中存储的是老版本数据，当一个事务需要读取记录行时，如果当前(最新)记录行不可见，可以顺着**undo log链**找到满足其可见性条件的记录行版本。

在 InnoDB 里，undo log 分为如下两类：

- **insert undo log** : 事务对 insert 新记录时产生的 undo log。只在事务回滚时需要，并且在事务提交后就可以立即丢弃。

- **update undo log** : 事务对记录进行 delete 和 update 产生的 undo log。在 事务回滚 和 快照中需要。只有当 数据库所使用的快照中不涉及该日志记录，对应的回滚日志才会被 purge 线程删除。

  > Purge 线程：为了实现 InnoDB 的 MVCC机制，updata 或 delete 都只是设置一个旧记录的 deleted_bit，并不真正将 旧记录 删除。
  >
  > 为了节省磁盘空间，InnoDB有专门的purge线程来清理deleted_bit为true的记录。purge线程自己也维护了一个read view，如果某个记录的deleted_bit为true，并且DB_TRX_ID相对于purge线程的read view可见，那么这条记录一定是可以被安全清除的。

<font size=4 color=purple>在同一个事务中，select  只能看到快照创建前已经提交的修改和该事务本身做的修改。</font>

#### 快照读

我们常说的 “读”，普通读 就是 快照读。

快照读 不加锁，依赖 MVCC 进行事务隔离，可保证读已提交、可重复读和部分幻读，读到的可能是历史数据。

```
select ... from ... where ...
```

#### 当前读

当前读，**返回最新的数据**。依赖  Next-Key 锁；

Next-Key锁 (行级锁) = S锁/X锁 (record lock) + 间隙锁（gap lock）

事实上，**“当前读”除了会上S/X锁，还会上一把间隙锁**。**共享锁和排他锁保证了“当前读”的读已提交和可重复读**；**间隙锁解决“当前读”的幻读问题。**（复习一下：**所谓幻读就是两次(当前)读之间被插入了一条数据**）



<font size=5>`undo log`和`redo log`并不是直接写到磁盘上的，而是先写入`log buffer`。再等待合适的时机同步到`OS buffer`，再由操作系统决定何时刷到磁盘。</font>

<img src="https://i.loli.net/2021/08/17/ItyiL8wfqpQJCgS.png" alt="在这里插入图片描述" style="zoom:50%;" />

<font size=4>**既然undo log 和 redo log都是从 log buffer 到 OS buffer，再到磁盘。所以中途还是有可能因为断电/硬件故障等原因导致日志丢失。为此MySQL提供了三种持久化方式**</font>，之前的博客：MySQL优化：参数 中提到过一个参数innodb_flush_log_at_trx_commit，这个参数主要控制InnoDB将log buffer中的数据写入OS buffer，并刷到磁盘的时间点，取值分别为0，1，2，默认是1。

