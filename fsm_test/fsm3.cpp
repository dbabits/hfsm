// Copyright 2003 by Dmitry Babitsky
// All Rights Reserved.

#include "stdafx.h"
#include "../include/announcer.h"
#include "../include/fsm.h"
#include "aggregator.h"

//#include <boost/variant.hpp>

#include <memory>


using namespace ver;


///App-specific stuff-----------------------------------------------------------------










template<class T>
struct Creator{
    static T* instance(){return new T;}
};




version1::IState* version1::Aggregator::state1::evt1(version1::Aggregator* pfsm,const boost::any& arg){

    printf("\nstate1::evt1");
    return &pfsm->st2;

    //pfsm->dispatch(evt3);//raise INTERNAL event TODO: is it the right place?
}

version1::IState* version1::Aggregator::state1::evt2(version1::Aggregator* pfsm,const boost::any& arg){
    std::string s;
    try
    {

        s=boost::any_cast<std::string>(arg);
    }
    catch(const boost::bad_any_cast& e)
    {
        printf("\nstate1::evt2->%s",e.what());
        return new version1::Aggregator::state5(pfsm);
    }

    printf("\nstate1::evt2. arg=%s", s.c_str());
    return &pfsm->st3;
}


version1::IState* version1::Aggregator::state2::evt1(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate2::evt1");

    return &pfsm->st1;
}
version1::IState* version1::Aggregator::state3::evt1(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt1");
    //return heap_only::instance<Aggregator::state4>();
    //return heap_only<Aggregator::state4>::instance();
    return new version1::Aggregator::state4;
}
version1::IState* version1::Aggregator::state3::evt2(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt2");
    return this; //no transition
}

version1::IState* version1::Aggregator::state3::evt3(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt3");

    

    return new version1::Aggregator::state5(pfsm);

    //return heap_only<Aggregator::state5>::instance();
        //return create<Aggregator::state5>();
    //return heap_only::instance<Aggregator::state5>();
    //Creator<Aggregator::state5>::create();
}






version1::IState* version1::Aggregator::state4::evt1(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate4::evt1");
    return &pfsm->st1;          //transition to..
}

void version1::Aggregator::state4::on_exit(version1::Aggregator*){
    printf("\nstate4::on_exit");
    //heap_only<Aggregator::state4>::destroy(this);
    //destroy(this);
    delete this;
}


version1::IState* version1::Aggregator::state5::evt2(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate5::evt2");
    return &pfsm->st1;
}
version1::IState* version1::Aggregator::state5::evt3(version1::Aggregator* pfsm,const boost::any& arg){
    printf("\nstate5::evt3");
    return 0;
}




/*
    
    struct Key{
        IState* state;
        IState::Event event;
    }; 
    struct state_tran_table{
        Key key;
        IState* next_state;
    };


    bool operator <(const Key& lhs,const Key& rhs){
        return lhs.state<rhs.state && lhs.event != rhs.event;
    }

    std::map<Key,IState*> state_map;

    state_tran_table state_transition_table[]={
         {{&a.st1,IState::evt1},&a.st3}
        ,{{&a.st1,IState::evt1},&a.st3}
    };
    
    state_map[state_transition_table[0].key]=state_transition_table[0].next_state;

    //printf("\n IState::evt1=0x%x",reinterpret_cast<void (*)(void)>(IState::evt1));


struct IEventData{
    virtual void release(){}
protected:
    virtual ~IEventData(){}
};

template<class T>
struct traits{
    static void release(T data){}
};
#if _MSC_VER >1200
template<class T>
struct traits<T*>{
    static void release(T* data){delete data;}
};

template<class T>
struct traits<T (*) []>{//does not get generated
    static void release(T* data){delete []data;}
};
#endif

template<class T,class trait=traits<T> >
struct Argument{
    T data;
    Argument(T d):data(d){}
    ~Argument(){trait::release(data);}
};

                Argument<int> a(15);
                Argument<char*> pa(new char[15]);
                Argument<int*>  pi(new int);


struct Variant{
    enum {Empty,String,Int,Float,Double,IEventDataPtr,CharPtr} type;

    Variant():type(Empty),_dbl(0){}
    virtual ~Variant(){
        if(type==IEventDataPtr && pData) pData->release();
        else if(type==CharPtr && pData) delete [] pSz;
    }

    //Variant(std::string* p):type(String),pStr(p){}
    Variant(float* p)      :type(Float), pFlt(p){}
    Variant(int*   p)      :type(Int),   pInt(p){}
    Variant(double* p)     :type(Double),pDbl(p){}
    Variant(IEventData* p) :type(IEventDataPtr),pData(p){}
    
    union {
        //std::string* pStr;
        float*       pFlt;
        int*         pInt;
        double*      pDbl;
        char*        pSz;
        IEventData*  pData;
        float        _flt;
        int          _int;
        double       _dbl;
    };
};


*/