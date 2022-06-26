### 三次握手四次挥手，相关问题

<font size=5 color=purple>好文章：https://leetcode-cn.com/circle/discuss/b4PW9S/ </font>

TCP 的连接状态查看，在 Linux 可以通过 `netstat -napt` 命令查看。

- **初始序列号ISN是什么？**
  - 答：ISN  <font color=blue>**随时间而变化**</font>，因此每个连接都将具有不同的ISN。<font color=blue>**ISN可以看作是一个32比特的计数器，每4ms +1 **</font>。这样选择序号的目的在于防止在网络中被延迟的分组在以后又被传送，而导致某个连接的一方对它做错误的解释。

<font size=4 color=red>ISN = M + F (localhost, localport, remotehost, remoteport)</font>

**M 是一个计时器，这个计时器每隔 4 毫秒加 1。**
**F 是一个 Hash 算法，根据源 IP、目的 IP、源端口、目的端口生成一个随机数值**。要保证 Hash 算法不能被外部轻易推算得出，**用 MD5 算法**是一个比较好的选择。



- <font  size=4>**第三次握手中，如果客户端的ACK  未送达服务器，会怎样？**</font>
  
  - 答：对于**<font color=red> Server</font>** 而言，由于Server没有收到ACK确认，因此**<font color=red>会重发之前的SYN+ACK（==默认重发五次，之后自动关闭连接进入CLOSED状态==）</font>**，Client  收到后会重新传ACK 给Server。
  - **当Server自动关闭这个连接后**，但是**Client认为这个连接已经建立**，如果Client端向Server写数据，**Server端将以RST包响应**，方能感知到Server的错误。
  - **在Server进入CLOSED状态之后，如果Client向服务器发送数据，服务器会以RST包应答。**
  
- <font color=red>**SYN攻击  ？**</font>
  
  - ==**服务器端的资源分配是在二次握手时分配的，而客户端的资源是在三次握手完成后分配的**==。(该知识点，具体展开来说。对端在第二次握手后，分配了什么资源？)
  
    - 当开放了一个TCP端口后，该端口就处于Listening状态，不停地监视发到该端口的 SYN 报文，<font color=#FF44FF>一旦接收到Client发来的 SYN报文，就需要**==为该请求分配一个TCB==（**Transmission Control Block），通常一个TCB至少需要280个字节，在某些操作系统中TCB甚至需要1300个字节，==并返回一个SYN ACK命令，立即转为SYN-RECEIVED即半开连接状态==</font>，而某些操作系统在SOCK的实现上最多可开启512个半开连接（如Linux2.4.20内核）。这种过程如下图所示：
  
    <img src="../AppData/Roaming/Typora/typora-user-images/image-20210324093329672.png" alt="image-20210324093329672" style="zoom:80%;" />
  
    <font size=4>从以上过程可以看到，如果恶意的向某个服务器端口发送大量的SYN包，则可以使服务器打开大量的半开连接，分配TCB，从而消耗大量的服务器资源，同时也使得正常的连接请求无法被相应。而攻击发起方的资源消耗相比较可忽略不计。</font>
  
  - 所以服务器容易受到SYN洪泛攻击。<font size=4>**SYN攻击就是Client在短时间内伪造大量不存在的IP地址，并向Server不断地发送SYN包**</font>，Server则回复确认包，并等待Client确认，由于源地址不存在，因此Server需要不断重发直至超时，<font size=4>这些==**伪造的SYN包将长时间占用半连接队列，导致正常的SYN请求因为队列满而被丢弃，从而引起网络拥塞甚至系统瘫痪**==。</font>
  
  - <font size=4 color=red>**服务器上出现大量的半连接状态时，特别是 源IP地址是随机的，基本上可以断定这是一次SYN攻击**</font>。
  
  - <font size=5 color=purple>如何防御 SYN攻击？</font>
  
    -  <font size=4>**延缓TCB分配方法**</font>
  
      从前面SYN Flood原理可以看到，<font color=purple>消耗服务器资源主要是因为当SYN数据报文一到达，系统立即分配TCB，从而占用了资源。而SYN Flood由于很难建立起正常连接，因此，**当正常连接建立起来后再分配TCB则可以有效地减轻服务器资源的消耗**</font>。常见的方法是<font color=red>**使用Syn Cache和Syn Cookie技术**</font>。
  
      - <font color=blue>**讲讲 SYN Cache 技术？**</font>
        - 这种技术是**在收到SYN数据报文时**不急于去分配TCB，而是**先回应一个SYN ACK报文**，并在一个**专用HASH表（Cache）中保存这种半开连接信息，**直到**收到正确的回应ACK报文再分配TCB**。 由于在发送的SYN ACK中需要使用客户端（发起方）的Sequence Number，且不能被攻击方知道。所以，SYN Cache 一般用加密算法 生成seq序列号。
      - SYN Cache 需要保存对端的 seq number，而**<font color=blue>SYN Cookie</font>** 不需要保存任何资源；该技术使用一种特殊的算法生成 seq number，并在 收到对端的ACK后，重新计算一遍，看其是否与对方回应报文中的 seq number -1 相同。
  
    - <font size=4>**释放无效连接**</font>
  
      - <font color=purple>不停监视系统的半开连接和不活动连接</font>，当达到一定阈值时拆除这些连接，从而释放系统资源。
  
- **如果已经建立了连接，但客户端出现了故障怎么办？**
  
  - <font color=blue>服务器</font>每收到一次客户端的请求后都会重新<font color=blue>复位一个计时器</font>。计时器超时后，还未接收到客户端的报文，就<font color=blue>不间断地发送 探测报文</font>；若一连发送 **10 次探测报文都没有回应**，则 服务器关闭连接。
- **三次握手过程中可以携带数据吗？**
  - **<font color=blue>在第一次和第二次不可以，在第三次握手的时候是可以携带数据的</font>。**（如果第一次握手携带报文的话，容易遭受攻击）
  - 对于第三次连接的时候，此时<font color=blue>客户端已经处于 ESTABLISHED 状态</font>。对于客户端来说，他已经建立起连接了，并且也已经知道服务器的接收、发送能力是正常的了，所以能携带数据。
- **队列？**
  
  - 半连接队列：
    
    - **服务器第一次收到客户端的 SYN 之后**，就会处于 SYN_RCVD 状态，此时双方还没有完全建立其连接，服务器会把此种状态下请求连接放在一个队列里，我们把这种队列称之为**半连接队列**。
  - 全连接队列：
    
    - **已经完成三次握手**，建立起连接的就会放在**全连接队列**中。如果**队列满了就有可能会出现丢包现象**。
  
- **为什么不把第二次和第三次挥手合并起来，二者有何区别？**
  
  - 当服务器收到客户端断开连接的请求时，可能还有一些数据没有发完，这时先回复ACK，表示接收到了断开连接的请求。等到数据发完之后再发FIN，断开服务器到客户端的数据传送。

### TCP 握手的异常情况

https://blog.csdn.net/plokmju88/article/details/104305897

<img src="https://imgconvert.csdnimg.cn/aHR0cHM6Ly9tbWJpei5xcGljLmNuL21tYml6X3BuZy9saWFjekQxOE9pY1N3WXdISHJpY3BsOUdXaEtab3cwcExqbkZVT29CSnRBR2VHNFgxOFJTanlvNUhITHFpYXR1emw5ZlpRbERQOVR3VGliVjlFMTRTMEVUZFh3LzY0MA?x-oss-process=image/format,png" alt="img" style="zoom:67%;" />

正常情况下：

1. 在初始时，双端处于 CLOSE 状态，服务端为了提供服务，会主动监听某个端口，进入 LISTEN 状态。

2. 客户端主动发送连接的「SYN」包，之后进入 SYN-SENT 状态，服务端在收到客户端发来的「SYN」包后，回复「SYN,ACK」包，之后进入 SYN-RCVD 状态。

3. 客户端收到服务端发来的「SYN,ACK」包后，可以确认对方存在，此时回复「ACK」包，并进入 ESTABLISHED 状态。

4. 服务端收到最后一个「ACK」包后，也进入 ESTABLISHED 状态。

这是正常的 TCP 三次握手，握手完成后双端都进入 ESTABLISHED 状态，在此之后，就是正常的数据传输过程。


-  **客户端第一个「SYN」包丢了，怎么办？**
  - 在 TCP 协议中，某端的一组「请求-应答」中，在一定时间范围内，只要没有收到应答的「ACK」包，无论是请求包对方没有收到，还是对方的应答包自己没有收到，均认为是丢包了，都<font color=blue>会触发超时重传机制。</font>
  - **<font color=blue>会尝试重传三次SYN包</font>**，间隔时间分别是 5.8s、24s、48s，三次时间大约是 76s 左右；
-  **服务端收到「SYN」并回复的「SYN,ACK」包丢了**
  - 对于 客户端而言，认为自己的[SYN包]丢失了，客户端会重传 [SYN]包。
  - 对于<font color=blue>服务器</font>，**<font color=blue>「SYN,ACK」包的重传次数</font>**，不同的操作系统下有不同的配置，例如在 Linux 下可以通过 `tcp_synack_retries` 进行配置，**默认值为 5**。如果这个重试次数内，仍未收到「ACK」应答包，那么**服务端会自动关闭这个连接**。
- **客户端最后一次回复「SYN,ACK」的「ACK」包丢了。**==（重点）==
  - 如果最后一个「ACK」包丢了，服务端因为收不到「ACK」会走重传机制，而**客户端此时进入 ESTABLISHED 状态**。多数情况下，客户端进入 ESTABLISHED 状态后，则认为连接已建立，会立即发送数据。但是**服务端因为没有收到最后一个「ACK」包，依然处于 SYN-RCVD 状态。**
  - 关键，就在==于**服务端在处于 SYN-RCVD 状态下，收到客户端的数据包后如何处理？**==
    - **<font color=blue>当客户端在 ESTABLISHED 状态下，==开始发送数据包时，会携带上一个「ACK」的确认序号==</font>**，所以哪怕客户端响应的「ACK」包丢了，**<font color=blue>服务端在收到这个数据包时，能够通过包内 ACK 的确认序号，正常进入 ESTABLISHED 状态。</font>**



## TCP 挥手的异常情况

<img src="https://imgconvert.csdnimg.cn/aHR0cHM6Ly9tbWJpei5xcGljLmNuL21tYml6X3BuZy9saWFjekQxOE9pY1N3WXdISHJpY3BsOUdXaEtab3cwcExqbjFlV3docXJlUEhuQXRTamZMeTc5cmJQTzFHaWFXR2lja1FCVFdJM2lic1MwZ2ljdzV3cmh2eXZuOFEvNjQw?x-oss-process=image/format,png" alt="img" style="zoom:50%;" />

1. 初始时双端还都处于 ESTABLISHED 状态并传输数据，某端可以主动发起「FIN」包准备断开连接，在这里的场景下，是客户端发起「FIN」请求。在发出「FIN」后，客户端进入 FIN-WAIT-1 状态。

2. 服务端收到「FIN」消息后，回复「ACK」表示知道了，并从 ESTABLISHED 状态进入 CLOSED-WAIT 状态，开始做一些断开连接前的准备工作。

3. 客户端收到之前「FIN」的回复「ACK」消息后，进入 FIN-WAIT-2 状态。而当服务端做好断开前的准备工作后，也会发送一个「FIN,ACK」的消息給客户端，表示我也好了，请求断开连接，并在发送消息后，服务端进入 LAST-ACK 状态。

4. 客户端在收到「FIN,ACK」消息后，会立即回复「ACK」，表示知道了，并进入 TIME_WAIT 状态，为了稳定和安全考虑，客户端会在 TIME-WAIT 状态等待 2MSL 的时长，最终进入 CLOSED 状态。

5. 服务端收到客户端回复的「ACK」消息后，直接从 LAST-ACK 状态进入 CLOSED 状态。


- <font color=purple>**断开连接的 FIN 包丢了。**（第一次挥手）</font>

  - 客户端率先发的「FIN」包丢了，或者没有收到对端的「ACK」回复，则会触发超时重传，**直到触发重传的次数，直接关闭连接**。
  - 对于服务器，因为没有收到 FIN，所以没有任何反应。在一段时间之后，也会关闭连接。

- <font color=purple>**服务端第一次回复的 ACK 丢了。**（）第二次挥手</font>

  - 客户端没有收到「ACK」应答，会尝试重传之前的「FIN」请求，服务端收到后，又会立即再重传「ACK」。**<font color=blue>此时服务端处于 CLOSE_WAIT 状态</font>**，当服务器**准备断开后，会回复「FIN,ACK**」，注意这个消息是**<font color=blue>携带了之前「ACK」的响应序号的。</font>**
  - 只要这个消息没丢（**「FIN,ACK」**），客户端可以凭借「FIN,ACK」包中的响应序号，**直接从 FIN-WAIT-1 状态，进入 TIME-WAIT 状态**，开始长达 2MSL 的等待。

- <font color=purple> **服务端发送的 FIN，ACK 丢了。**（第三次挥手）</font>
- 服务端在超时后会重传；而客户端  有两种状态：处于 FIN-WAIT-2 状态（之前的 ACK 也丢了），要么处于 TIME-WAIT 状态，会等待 2MSL 时间。
  
- <font color=purple>**客户端最后回复的 ACK 丢了（第四次挥手）**</font>
- **<font color=blue>客户端在回复「ACK」后，会进入 TIME-WAIT 状态</font>**，**开始长达 2MSL 的等待**，服务端因为没有收到「ACK」的回复，会重试一段时间，直到**<font color=blue>服务端重试超时后主动断开。</font>**
  - **<font color=blue>或者等待新的客户端接入后，收到服务端重试的「FIN」消息后，回复「RST」消息</font>**，在收到「RST」消息后，复位服务端的状态。
  
- <font color=purple>**客户端收到 ACK 后，服务端跑路了。**</font>
- 客户端**在收到「ACK」后，进入了 FIN-WAIT-2 状态**，**等待服务端发来的「FIN」包**，而如果服务端跑路了，这个包永远都等不到。
  - **<font color=blue>在 TCP 协议中，是没有对这个状态的处理机制的。操作系统会接管这个状态</font>**。**<font color=red>超过 tcp_fin_timeout 的限制后</font>**，状态并不是切换到 TIME_WAIT，而是**<font color=red>直接进入 CLOSED 状态。</font>**



## **SYN-ACK 重传次数**的问题：

​		服务器发送完SYN-ACK包，如果未收到客户确认包，服务器进行首次重传，等待一段时间仍未收到客户确认包，进行第二次重传。**<font color=red>如果重传次数超过系统规定的最大重传次数，系统将该连接信息从半连接队列中删除（即，服务器将自动关闭连接）。</font>**注意，**每次重传等待的时间不一定相同**，一般会是指数增长，例如间隔时间为 1s，2s，4s，8s......



## 存在大量 TIME_WAIT的原因、怎么处理？(高并发短连接)

https://blog.csdn.net/zhangjunli/article/details/89321202?utm_source=app&app_version=4.14.0

#### 大量的 `TIME_WAIT` 状态 TCP 连接存在，其本质原因是什么？

- <font color=blue size=4>**高并发短连接**存在</font>；**短连接表示“业务处理+传输数据的时间 远远小于 TIMEWAIT超时的时间”的连接**。
- <font color=blue>特别是 HTTP 请求中</font>，如果 `connection` 头部  被设置为 `close` 时，<font color=blue>基本都由「**服务端**」发起**主动关闭连接**</font>
- 而，`TCP 四次挥手`关闭连接机制中，为了保证 `ACK 重发`和`丢弃延迟数据`，设置 `time_wait` 为 2 倍的 `MSL`（报文最大存活时间）

TIME_WAIT ：主动关闭连接的一方，会出现  TIME_WAIT 状态。



#### **如何尽量处理TIMEWAIT过多?**

<font color=blue>编辑内核文件**/etc/sysctl.conf**，加入以下内容：</font>

```
net.ipv4.tcp_syncookies = 1 表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，表示关闭；
net.ipv4.tcp_tw_reuse = 1 表示开启重用。允许将TIME-WAIT sockets重新用于新的TCP连接，默认为0，表示关闭；
net.ipv4.tcp_tw_recycle = 1 表示开启TCP连接中TIME-WAIT sockets的快速回收，默认为0，表示关闭。
net.ipv4.tcp_fin_timeout 修改系默认的 TIMEOUT 时间
```

<font color=blue>然后执行 /sbin/sysctl -p 让参数生效.</font>

1. 打开系统的 <font color=red>TIMEWAIT 重用 和 快速回收。</font>
3. 

**结论**：几个核心要点：

1. <font color=purple> **time_wait 状态的影响**：</font>
   1. TCP 连接中，「主动发起关闭连接」的一端，会进入 time_wait 状态；
   2. <font size=4>time_wait 状态下，TCP 连接占用的端口，无法被再次使用；</font>
   3. TCP 端口数量，上限是 6.5w（`65535`，16 bit）
   4. 大量 time_wait 状态存在，会导致新建 TCP 连接出错，**address already in use : connect** 异常；
   
   

<font color=purple>**TIME_WAIT 存在的必要性：**</font>

- **可靠地 终止 TCP连接**
- **处理延迟到达的报文**；采用 2MSL   确保两个传输方向上的延迟报文都已被丢弃，使得  新连接 可以在 2MSL  后安全地建立。



## TCP 粘包 ？

<font size=4>TCP 粘包</font>：发送方发送的若干包数据到接收方接收时，粘成一包。从接收缓存区来看，后一包数据的头紧接着前一包数据的尾。

<font size=4 color=red>造成 粘包的原因？</font>

- 由 TCP连接复用 造成的粘包问题；
- 因为 TCP默认会使用 Nagle 算法，此算法会导致 粘包问题；
  - 只有一个分组得到确认，才会发送下一个分组；
  - 收集多个小分组，在一个确认到来时一起发送。
- 流量控制、拥塞控制也可能导致粘包
- 接收方不及时接收缓冲区的包，造成多个包接收。

<font size=4 color=red>怎么解决？</font>

1. 尾部标记序列：**通过特殊标识符表示数据包的边界**； 例如 \n\r
2. 头部标记分步接收：**在 TCP 报文的头部加上表示 数据长度的标识**；
3. **应用层 定长发送数据；**



## 假设慢启动阶段发生了丢包，会怎么样？(字节一面)

先慢启动，达到cwnd=ssthresh时，进入拥塞避免。

<font color=red size=4>如果发生丢包，进入拥塞发生。因为丢包有两种不同处理办法，所以拥塞发生也有两种不同的处理办法。</font>

1.（计数器超时）计算ssthresh,cwnd，重新进入慢启动，再等到cwnd=ssthresh时,再次进入拥塞避免。

2.（重复的3个ACK）计算ssthresh,cwnd,快速重传，再等到cwnd=ssthresh时,再次进入拥塞避免。（步骤2就称为快速恢复）

## listen 时候参数 backlog 的意义？

Linux内核中会，<font color=blue>维护两个队列：</font>

- 未完成连接队列（<font color=blue>SYN 队列</font>）：接收到一个 SYN 建立连接请求，处于 SYN_RCVD 状态；
- 已完成连接队列（<font color=blue>Accpet 队列</font>）：已完成 TCP 三次握手过程，处于 ESTABLISHED 状态；

<img src="https://pic.leetcode-cn.com/1616140750-lBwkum-file_1616140748744" alt=" SYN 队列 与 Accpet 队列 " style="zoom:60%;" />

```
int listen (int socketfd, int backlog)
```

**在早期 Linux 内核 backlog 是 SYN 队列大小**，也就是未完成的队列大小。

**在 Linux 内核 2.2 之后，backlog 变成 accept 队列**，也就是已完成连接建立的队列长度，所以现在通常认为 backlog 是 accept 队列。

但是上限值是内核参数 somaxconn 的大小，也就说 accpet 队列长度 = min(backlog, somaxconn)。



<font size=5>为了方便调试服务器程序，一般会在服务端设置 SO_REUSEADDR 选项，这样服务器程序在重启后，可以立刻使用。这里设置<font color =red>SO_REUSEADDR </font>是不是就等价于对这个 socket 设置了内核中的 <font color =red>net.ipv4.tcp_tw_reuse=1 </font>这个选项？”</font>

- <font color =red size=4>tcp_tw_reuse 是内核选项，主要用在连接的发起方（客户端）</font>。TIME_WAIT 状态的连接创建时间超过 1 秒后，新的连接才可以被复用，注意，这里是「连接的发起方」；
- <font color =red size=4>SO_REUSEADDR 是用户态的选项，用于「连接的服务方」</font>，用来告诉操作系统内核，**如果端口已被占用，但是 TCP 连接状态位于 TIME_WAIT ，可以重用端口**。如果端口忙，而 TCP 处于其他状态，重用会有 “Address already in use” 的错误信息。

<font size=4 color=blue>总结：</font>

​		**tcp_tw_reuse 是为了缩短 time_wait 的时间**，避免出现大量的 time_wait 连接而占用系统资源**，解决的是 accept 后的问题。**

​		**SO_REUSEADDR 是为了解决 time_wait 状态带来的端口占用问题**，以及支持同一个 port 对应多个 ip，**解决的是 bind 时的问题**。



<font size=5>如果是服务提供方发起的 close ，引起过多的 time_wait 状态的 tcp 链接，time_wait 会影响服务端的端口吗？==(重点)==</font>

**如果发起连接一方（客户端）的 TIME_WAIT 状态过多，占满了所有端口资源，则会导致无法创建新连接。**

**客户端受端口资源限制：**

- 客户端TIME_WAIT过多，就会导致端口资源被占用，因为端口就65536个，被占满就会导致无法创建新连接。

==**服务端受系统资源限制：**==

- 由于一个 TCP 四元组表示 TCP 连接，**理论上服务端可以建立很多连接，服务端只监听一个端口，但是会把连接扔给处理线程，**所以理论上监听的端口可以继续监听。但是**线程池处理不了那么多一直不断的连接了**。所以当服务端出现大量 TIMEWAIT 时，系统资源容易被耗尽。

## linux socket编程中的recv和send的返回值及其含义（我不是很懂）

https://blog.csdn.net/qq_26105397/article/details/80988429

```
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

当应用程序调用recv接收数据的时候，recv函数会等待sockfd中发送数据的缓冲区的协议发送完数据，**如果在等待过程中出现网络错误，则会返回SOCKET_ERROR**。如果sockfd中的缓冲区中没有数据或者协议已经发送完数据，则recv会检查sockfd的接受缓冲区，如果该缓冲区正在接受数据，则recv会一直等待，知道缓冲区接受数据完毕，之后<font size=4 color=blue> recv 将数据从缓冲区拷贝一份到 buf 中</font>，数据通过协议转发的，recv只是将数据从缓冲区拷贝过来。注，<font size=4 color=blue>如果recv在拷贝数据时出现错误，则返回SOCKET_ERROT，如果在协议传输数据中出现网络错误，则返回0。</font>

阻塞与非阻塞 recv **返回值**没有区别，都是：

>  **<0 出错**  
>
>  **=0 对方调用了close API来关闭连接**
>
> \>0 接收到的数据大小，
>
> 特别地：**返回值 ** <0  时  并且(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) 的情况下认为连接是正常的，继续接收。

只是阻塞模式下recv会一直阻塞直到接收到数据，**非阻塞模式下如果没有数据就会返回，不会阻塞着读，因此需要循环读取**。

返回说明：   

（1）成功执行时，返回接收到的字节数。

（2）若另一端已关闭连接则返回0，这种关闭是对方主动且正常的关闭

（3）失败返回-1，errno  被设为以下的某个值 ：

> EAGAIN：套接字已标记为非阻塞，而接收操作被阻塞或者接收超时
>
> EBADF：sock不是有效的描述词
>
> ECONNREFUSE：远程主机阻绝网络连接
>
> EFAULT：内存空间访问出错
>
> EINTR：操作被信号中断
>
> EINVAL：参数无效
>
> ENOMEM：内存不足
>
> ENOTCONN：与面向连接关联的套接字尚未被连接上
>
> ENOTSOCK：sock索引的不是套接字

send 函数返回值和 recv 返回值的含义一样。

send()方法的行为：

​		对于send方法，将需要发送的数据拷贝至发送缓冲区，否则进入阻塞或者进入超时等待。如果改变这种状态，将发送缓冲区大小设置为0，这样，<font size=4 color=blue>当send成功返回，代表的是所发送的数据  到达了目的机器。但是，只是到达目标服务器的接受缓冲区，并不保证数据以被应用层所接收</font>。另外， 在发送数据时，协议根据滑动窗口和MSS值来确定tcp报文段的数据字段大小，这样就能保证接收缓冲区不会溢出。如果接收方的滑动窗口为0，但是发送方还有数据尚未发送完成，就是用探测机制，一方面检测对方的滑动窗口的大小变化（探测机制是通过每次发送一个字节来进行检测，由先前的30s到之后的1分钟，最终达到2分钟间隔）)，另一方面检测对方的连接是否异常。

​		push标志指示接收端应尽快将数据提交给应用层。如果send函数提交的待发送数据量较小，例如小于1460B（参照MSS值确定），那么协议层会将该报文中的TCP头部的push字段置为1；如果待发送的数据量较大，需要拆成多个数据段发送时，协议层只会将最后一个分段报文的TCP头部的push字段置1。



### **1、TCP三次握手的过程，accept发生在三次握手的哪一个阶段?**

**accept 过程发生在 三次握手 之后**，三次握手完成后，客户端和服务器就建立了tcp连接并可以进行数据交互了。这时可以调用accept函数获得此连接。



### **2、epoll 边缘模式建议尽量一次把数据读完，怎样判断当前数据已经读完？**

将 **socket 设成 NONBLOCK**（使用fcntl函数），然后select到该socket可读之后，使用read/recv来读取数据。**当函数返回-1，同时errno是EAGAIN或EWOULDBLOCK的时候，表示数据已经全部读取完毕**。

使用 epoll 边缘模式，**需要 循环调用 recv 函数直到 recv 出错**，且 错误码是 EWOULDBLOCK。



###  **3、select 函数的第一个参数怎么设置？select 函数的超时参数如果设置为 NULL 是什么行为？**

`int select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset,  const struct timeval* timeout);`

select 第一个参数 maxfd，表示监视对象文件描述符数量；

select 如果最后一个参数设置为NULL，则表示如果没有socket读或者写就绪，则一直会堵塞在那。



### **4、TCP 连接如何实现半关闭？**

close函数会实现半关闭，但有时候存在一些情况，close()不足以实现半关闭。比如使用dup2函数，实现有2个文件描述符指向同一个套接字，此时调用close(fd)；只close了一个文件描述符连接(通过fd不能写入数据，只能读数据），但，此时，还有另一个文件描述符指向该socket，还可以通过这个文件描述符进行读写。这就不是真正意义半关闭，此时就要使用**shutdown函数**了。

**使用shutdown比close的优势是，可以仅关闭读，仅关闭写，关闭读写，情况更加精细。同时，当出现多个文件描述符指向同一socket时，调用shutdown，可以将所有的指向都断开。**

> SHUT_RD (0)：关闭 sockfd 上的读功能；
>
> SHUT_WR (1)：关闭 sockfd 上的写功能；
>
> SHUT_RDWR (2)：关闭 读写功能；

 使用 close 终止一个连接，但只是减少引用计数，并不直接关闭连接，只有当引用计数为 0 时，才关闭连接；

 shutdown，不考虑引用计数，直接关闭 sockfd；



### 三次握手四次挥手，和socekt api的对应

- 三次握手对应的 API：connect（客户端）、listen、accept（服务器）

  - connect  发送一个 SYN，收到Server 的SYN+ACK后，代表连接成功。
  - listen ，Server 有 半连接队列，已完成队列
  - accept，检查已完成队列，返回已完成连接。

  建立连接请求？

  connect 完成3次握手之后，accept 所监听的 fd 上，是否有可读事件；

  关闭连接请求？

  read 读取数据时，返回 0，表示有断开连接请求。

  

4