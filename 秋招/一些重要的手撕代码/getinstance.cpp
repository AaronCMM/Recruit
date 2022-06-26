// 实现 双重检测的懒汉模式
#include<pthread.h>
class single{
private:
    static pthread_mutex_t lock;
    static single *p;
    single(){
        pthread_mutex_init(&lock,NULL);
    }
    ~single(){ }
public:
    static single* getinstance();
};

pthread_mutex_t single::lock;
single* single::p=NULL;
single* single::getinstance(){
    if(p==NULL){
        pthread_mutex_lock(&lock);
        if(p==NULL){
            p=new single();
        }
        pthread_mutex_unlock(&lock);
    }
    return p;
}
