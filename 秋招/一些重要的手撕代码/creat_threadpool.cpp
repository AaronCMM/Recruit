template <typename T>
class creat_threadpool{
    pthread_t *p;    // 创建线程数组
    int size;
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行*/
    static void *worker(void *arg);    //注意： work()设置为 静态函数（全局共享，唯一性）
    void run();

    p = new pthread_t[size];
    for(int i=0;i<size;i++){
        // pthread_create 的第三个参数必须为 void *
        if(pthread_create(p+i,NULL,worker,this)!=0){
            delete [] p;
            exception();
        }
        if(pthread_detach(p[i])){
            delete [] p;
            exception();
        }
    }
};