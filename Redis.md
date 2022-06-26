Redis 数据库里面的每个键值对，都是由对象组成的。其中：

- 数据库键   总是一个字符串对象；
- 数据库键的值   可以是 **字符串对象，列表对象、哈希对象、集合(set)，有序集合（zset）。**

SDS（simple dynamic string）简单动态字符串，用作 Redis 的默认字符串表示。

<img src="https://i.loli.net/2021/05/03/mDnhP8Cuvos2MSp.png" alt="image-20210331215642253" style="zoom:67%;" />

<font color=red>**Redis 的数据库就是使用字典来作为底层实现**</font>的，对数据库的增、删、改、查也是构建在对字典的操作之上。

#### 字典

**字典 使用 哈希表作为底层实现**。每个字典带两个哈希表，一个平时使用，另一个仅在 rehash 时使用。在对哈希表进行扩展或收缩时，==**采用 渐进式rehash 到 ht[1]**==中。

负载因子= 哈希表已存储节点数 / 哈希表大小   load_factor= ht[0].used / ht[0].size ;

字典的数据结构如下：

<img src="https://i.loli.net/2021/05/03/g53qknZFIs1SpHf.png" alt="image-20210405220855386" style="zoom:50%;" />

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210503171131447.png" alt="image-20210503171131447" style="zoom:80%;" />

哈希表 dictht 的数据结构：

<img src="https://i.loli.net/2021/05/03/miVJ1rIEq6xbNHX.png" alt="image-20210405221119523" style="zoom:70%;" />

#### 跳跃表

**Redis 使用跳跃表作为有序集合的底层实现之一**。

跳跃表是一种有序数据结构，通过在 每个节点中维持多个指向其他节点的指针，从而达到快速访问节点的目的。

Redis 只在两个地方用到了跳跃表。1、实现有序集合键；2、在集群节点中用作内部数据结构；

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210407165638555.png" alt="image-20210407165638555" style="zoom:67%;" />

**层的数量越多，访问其他节点的速度就越快。**

各个节点保存的成员对象必须是唯一的，但多个节点保存的分值可以相同。最后按照成员对象在字典序中的大小来排序。

#### 压缩列表

**压缩列表 ziplist** 也可以实现有序集合；每个集合元素有两个节点保存，第一个节点保存元素的成员（member），第二个节点保存元素的分值（score）。按分值进行排序

<img src="https://i.loli.net/2021/05/03/Bpmt8ya9il1sSb3.png" alt="image-20210503160313910" style="zoom:80%;" />

Redis 中的每个对象都由一个 redisObject 结构表示；

```
typedef struct redisObject{
	unsigned type:4;          // 类型
	unsigned encoding:4;      // 编码
	void  *ptr;               // 指向 底层实现数据结构的指针
    ...
} robj;
```

引用计数，用于实现内存回收，以及对象共享。

将数据库键的值指针，指向一个现有的值对象。被共享对象的引用计数+1；

<img src="https://i.loli.net/2021/05/03/4pht7g6ZdwYnoVb.png" alt="image-20210503162756179" style="zoom:67%;" />

Redis 是一个 key-value pair 的 数据库服务器，每个数据库都由 redisDb 结构表示，**redisDb结构的 dict 字典，保存了数据库中的所有键值对**。这个字典成为 key space.

#### 设置过期时间

EXPIRE <key><ttl>，将 键 key 的生存时间设置为 ttl 秒

PEXPIRE<key><ttl> 设置生存时间为 毫秒

这个命令的底层都是调用 PEXPIREAT 命令，设置 过期时间。

PERSIST 命令，移除一个键的过期时间（毫秒级别）。

TTL / PTTL 命令 返回 这个键的剩余生存时间。

伪代码的实现：

```
def PTTL(key):
	if key not in redisDb.dict:    # 键不存在于数据库
		return -2;
	expire_time_in_ms=redisDb.expires.get(key);   # 获取设置的过期时间
	if expire_time_in_ms is None:
		return -1;
	now_ms=get_current_timestamp();
	return(expire_time_in_ms — now_ms);
```

NOTE：键的过期时间都保存在——过期字典中；

#### 过期键的删除策略？

- 定时删除：设置键的过期时间的同时，设置定时器；（占用CPU，在内存不紧张，但CPU时间紧张的情况下，将CPU时间用在删除和当前任务无关的过期键上，会对服务器的响应时间和吞吐量造成影响）——在现阶段来说，不现实。
- 惰性删除：不执行过期键的操作。但每次从键空间获取键时，都要判断键是否过期。（对 CPU 友好，对内存不友好）对运行状态非常依赖于内存的Redis 服务器来说，不是好消息。比如 log，在某个时间点之后，对它们的访问会大大减少，甚至不再访问，如果这类过期数据大量积压在数据库中，会造成内存泄漏。——不推荐
- 定期删除：每隔一段时间，程序就对数据库进行检查，删除过期键。

<font color=red>Redis 使用，惰性删除  和  定期删除 相结合的策略。</font>

<img src="https://i.loli.net/2021/05/03/51TVjCD6yahNLpx.png" alt="image-20210503175537050" style="zoom: 67%;" />

### RDB持久化快照

将Redis 在内存中的<font color=red>数据库状态（数据库中的键值对）保持在磁盘中</font>，避免数据意外丢失。生成一个经过压缩的二进制RDB文件。

##### RDB文件的创建和载入

两个 Redis 命令，SAVE / BGSAVE。 **SAVE 命令会阻塞服务器**，直到 RDB文件创建完毕，阻塞期间，服务器不能处理任何命令请求； **BGSAVE 命令会派生 一个子进程**，由子进程负责创建 RDB文件，服务器继续处理命令请求。**最后**：SAVE 和 BGSAVE 只是以不同的方式调用 rdbSave 函数。

服务器启动时，自动载入 RDB文件。

<img src="https://i.loli.net/2021/05/03/JIWzhBE86tGP75y.png" alt="image-20210503180655257" style="zoom:67%;" />

##### 自动间隔性保存

用户通过设置 save 选项，让服务器每隔一段时间自动执行 BGSAVE命令。

服务器会根据 save选项所设置的保存条件，设置服务器状态 redisServer 结构的 saveparams。

```
struct saveparam{
	time_t seconds;  # 秒数
	int changes;     # 修改数
}
```

<img src="https://i.loli.net/2021/05/03/lns9LS3TNMfpdbP.png" alt="image-20210503202237218" style="zoom: 67%;" />

dirty 计数器 ：记录距离上一次成功执行 BSAVE命令后，服务器对数据库状态进行了多少次修改。

##### RDB 文件结构

<img src="https://i.loli.net/2021/05/03/NsorhMYtfRyq9DX.png" alt="image-20210503203554806" style="zoom: 67%;" />

database 部分：

<img src="https://i.loli.net/2021/05/03/D2wqEd1iOGzCYuj.png" alt="image-20210503203809761" style="zoom:67%;" />

key_value_pairs 部分，保存了一个及以上的键值对，如果键值对带有过期时间的话，过期时间也会被保存。

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210503205112564.png" alt="image-20210503205112564" style="zoom:67%;" />

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210503205133489.png" alt="image-20210503205133489" style="zoom:67%;" />

### AOF 持久化

**保存服务器所执行的写命令。**

##### 实现

AOF 持久化功能的实现，可以分为 命令追加（append）、文件写入、文件同步三个步骤。

redisServer 里有 sds aof_buf，作为 AOF 缓冲区；服务器进程就是一个事件循环，服务器在每次结束一个 loop之前，都会调用 flushAppendOnlyFile 函数，考虑是否把 aof_buf 缓存区中的内容写入和保存在 AOF文件中。

##### AOF 文件的载入与数据还原

因为AOF 文件里包含了重建数据库状态的所有写命令，所以服务器只要重新执行一遍AOF 里的命令，就可以还原数据库状态。

##### AOF 重写（重要）

<font color=red>为了解决 AOF文件体积膨胀 的问题，Redis 提供了 AOF文件重写功能</font>。新的AOF 文件不会包含任何浪费空间的冗余命令；

**重写的实现：**

不需要对现有的AOF 文件进行任何读取，直接读取服务器当前的数据库状态来实现。

首先从数据库中读取键现在的值，然后用一条命令去记录键值对，代替之前记录这个键值对的多条命令。（**AOF重写的原理**）

**Redis 将 AOF 重写程序放到子进程执行**（不用线程，可以避免使用锁）；当子进程在进行AOF重写时，服务器还需要继续处理命令请求，新的命令会导致数据库状态发生变换，为了**保持数据一致性，Redis 设置AOF重写缓存区**，在创建 子进程后开始使用。

在子进程执行AOF重写期间，服务器进程需要执行以下工作：

- 执行客户端发来的命令；
- 将执行后的命令追加到AOF缓存区
- 将执行后的命令追加到 AOF重写缓冲区

##### AOF 写策略

- Always，同步写回：每个写命令执行完，立马同步地将日志写回磁盘；
- Everysec，每秒写回：每个写命令执行完，只是先把日志写到 AOF 文件的内存缓冲区，每隔一秒把缓冲区中的内容写入磁盘；
- No，操作系统控制的写回：每个写命令执行完，只是先把日志写到 AOF 文件的内存缓冲区，由操作系统决定何时将缓冲区内容写回磁盘。

### Redis 事件

#### 文件事件

对 socket 的系列操作。采用 Reactor 事件处理器。用 I/O 多路复用同时监听多个套接字。

<img src="https://i.loli.net/2021/05/03/p6XPVWiItKbdFq9.png" alt="image-20210503213553747" style="zoom: 50%;" />

<img src="https://i.loli.net/2021/05/03/xYc1qhC5kHOAbJm.png" alt="image-20210503213711138" style="zoom:67%;" />

#### 时间事件

一个时间事件由 三个属性组成：id(唯一全局，新事件id比 旧事件大)， when（设定的触发时间），timeProc（响应函数）。

redis 将所有时间事件放在无序链表中（不按 when 排序），但 **Redis 中只使用 serverCron 一个时间事件**，在这种情况下，无序链表将退化成指针来使用。



Redis 服务器，典型的一对多服务器程序；

服务器为每个客户端都建立了 redisClient 结构，来保存客户端当前的状态信息（套接字、指向当前正在使用的数据库的指针，输入缓冲区（保存输入命令），输出缓冲区，身份验证标志等）

### serverCron 函数

Redis 服务器中的 serverCron 函数默认每隔 100 ms 执行一次，负责管理服务器的资源。

- 更新服务器时间缓存：避免每次获取系统当前时间都要执行一次系统调用，减少执行次数，用于对精确度要求不高的功能上，如：打印日志，更新LRU时钟... )，unixtime属性、mstime 属性，对当前时间进行缓存。
- 更新 LRU时钟
- 管理客户端资源

### 主从复制

用户可以通过执行 slaveof 选项，让一个服务器去复制另一个服务器。

旧版复制的 SYNC 命令是一个非常耗费资源的操作。

每次执行 SYNC 命令时，主从服务器都要执行以下动作：

- 主服务器需要执行 BGSAVE 命令生成 RDB文件。会耗费主服务器的大量CPU、内存和IO.
- 主服务器将生成的 RDB 发送给从服务器。
- 从服务器载入RDB

**新版复制，用 PSYNC命令**代替。对 断线后重复制进行了优化，只需要**部分重同步**。

- **复制偏移量**：通过对比主从服务器的复制偏移量，可以判断是否主从一致
- **复制积压缓冲区**：由 主服务器维护的一个固定长度 的FIFO队列（**环形缓冲区，全局共享**），默认1MB。当主服务器进行命令传播时，除了将写命令发送给所有从服务器，还将写命令入队到缓冲区中。<font color=blue>如果从服务器断线重连后的偏移量之后的数据仍然在缓存区中，就部分重同步，否则执行  完整重同步。</font>主服务器记录自己的 offest，每个从服务器记录自己的 offset。
- **服务器的运行ID**：初次复制时，从服务器会保存主服务器的运行ID，在断线重连的时候，向当前连接的主服务器发送之间保存的ID。

##### 复制的实现

- 设置主服务器的地址和端口

- 建立套接字连接

- 发送 PING命令

- 身份验证

  在复制操作刚开始时，从服务器会成为主服务器的客户端，向主服务器发送命令请求。**在复制操作的后期**，主服务器成为客户端，**向从服务器传播命令**，保持主从服务器一致。从服务器向主服务器发送命令进行心跳检测。

### Sentinel（哨岗、哨兵）

<img src="../AppData/Roaming/Typora/typora-user-images/image-20210504212754560.png" alt="image-20210504212754560" style="zoom: 50%;" />

**sentinel 用于监视 服务器状态**。需要和 主服务器建立两个连接；一个是 **命令连接**，专门用于向 主服务器发送命令，并接收命令回复。一个是**订阅连接**。

Sentinel 即会通过命令连接想服务器的 _sentinel _:hello 频道发送信息，又通过订阅连接从服务器的 _sentinel _:hello 频道接收信息。

### 集群

一个节点，就是一个运行在集群模式下的Redis 服务器。Redis 服务器在启动时会根据 cluster-enabled 配置选项是否为 yes 来决定是否开启 集群模式。

cluster meet 加入集群，节点通过握手来将其他节点添加到自己所处的集群当中。

##### 集群数据结构

 clusterNode，保存节点的当前状态，比如节点的创建时间、名字、当前的配置纪元、节点的IP地址和端口号等。

clusterState：记录当前节点的视角下，集群目前所处的状态。

##### 槽指派

<font color=red>Redis 集群**通过分片的方式**来保存数据库中的键值对</font>：集群的整个数据库被分为 16384个 槽，数据库中的每个键都属于这 16384个槽中的一个。

CLUSTER ADDSLOTS 命令，将一个或多个槽指派给节点负责。

**传播节点的槽指派信息：**一个节点除了将自己负责处理的槽记录在 clusterNode结构体里，还会将自己的 slots 数组通过消息发送给集群中的其他节点，以此告知其他节点自己目前负责处理哪些槽。**（每个节点都会记录哪些槽指派给了自己，哪些槽指派给了其他节点）**

因为每个节点都会广播自己的槽指派信息，所以 clusterState中的 slots 数组记录了 集群中所有的 16384个槽的指派信息。

<img src="https://i.loli.net/2021/05/05/dtHueRBlf9Wax6o.png" alt="image-20210505213009802" style="zoom:50%;" />

ASK错误和 MOVED错误

MOVED错误：表示槽的负责权已经从一个节点转移到另一个节点，MOVED错误后，客户端每次遇到关于 槽 i 的命令请求，都可以直接将请求发送到 MOVED 所指向的节点。

ASK错误：只是两个节点 在迁移槽的过程中使用的一种临时措施。ASK错误发送的节点转向，不用作用到下一次。

故障检测

集群中的每个节点都会定期地向集群中的其他节点发送 PING消息，检测对方是否在线。

### 发布与订阅

