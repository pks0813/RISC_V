#ifndef QUEUE_HPP
#define QUEUE_HPP
template<class T>
class PKSQueue{
    public:
    T* ARR;
    int size;
    int L,R;
    PKSQueue(int X):size(X){
        ARR=new T[size];
        L=0;R=0;
    }
    int Size(){
        if (R>=L) return R-L;
        return R-L+size;
    }
    int Insert(const T &X){
        int tmp=R;
        ARR[R]=X;
        (++R)%=size;
        return tmp;
    }
    void Delete(){
        if (L==R) {printf("Queue delete Error\n");exit(0);}
        (++L)%=size;
    }
    T& operator[] (const int &X){
        return ARR[(X+L)%size];
    }    
    T& Front(){
        return ARR[L];
    }    
    T& Back(){
        return ARR[((R-1+size)%size)];
    }
    void clear(){
        L=0;
        R=0;
    }
    PKSQueue<T>& operator= (const PKSQueue<T> &X){
        size=X.size;
        L=X.L;R=X.R;
        for (int i=0;i<size;i++)
            ARR[i]=X.ARR[i];
        return *this;
    }
    ~PKSQueue(){    delete []ARR;  }
}; 
#endif