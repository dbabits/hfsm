#include "stdafx.h"
#include "aggregator.h"

using namespace dbabits;

State::Factory Aggregator::state1::evt1(Aggregator* pfsm,const boost::any& arg){

    printf("\nstate1::evt1");

    return factory<Aggregator,state2>();

}

State::Factory Aggregator::state1::evt2(Aggregator* pfsm,const boost::any& arg){
    std::string s;
    try
    {
        s=boost::any_cast<std::string>(arg);
    }
    catch(const boost::bad_any_cast& e)
    {
        printf("\nstate1::evt2->%s",e.what());
        return factory<Aggregator,state5>();
    }

    printf("\nstate1::evt2. arg=%s", s.c_str());
    return factory<Aggregator,state3>(); 
}


State::Factory Aggregator::state2::evt1(Aggregator* pfsm,const boost::any& arg){
    int i=-1;
    try{
       i=boost::any_cast<int>(arg);
    }
    catch(const boost::bad_any_cast& e)    {
        printf("\nstate2::evt1->%s",e.what());
    }

    printf("\nstate2::evt1. arg=%d", i);

    return factory<Aggregator,state1>();
}
State::Factory Aggregator::state3::evt1(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt1");
    return factory<Aggregator,state4>();
}
State::Factory Aggregator::state3::evt2(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt2");
    return no_transition; 
}

State::Factory Aggregator::state3::evt3(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate3::evt3");
    return factory<Aggregator,state5>();
}


State::Factory Aggregator::state4::evt1(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate4::evt1");
    return factory<Aggregator,state1>();
}


State::Factory Aggregator::state5::evt2(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate5::evt2");
    return factory<Aggregator,state1>();
}
State::Factory Aggregator::state5::evt3(Aggregator* pfsm,const boost::any& arg){
    printf("\nstate5::evt3");
    return 0;
}

//always return pointers to the base state fn here..
Aggregator::Event2 get_event(boost::any& arg){

    Sleep(1000);
    

    int random=rand() % 3; //rand() % n returns an integral random number in the range 0 - (n - 1)
    printf("->%d",random+1);
    switch (random){
        case 0:
            arg=55;
            return State::evt1;
        case 1:
            arg=std::string("hello world");
            return State::evt2;
        case 2:
            return State::evt3;
        default:
            return State::evt1;
    }
}

 

void main(int argc, char* argv[]){
    printf("%s compiled %s\n",argv[0],__TIMESTAMP__);
    srand( (unsigned)time(0));
 
    Aggregator a;

    for(;;){
        boost::any arg;
        Aggregator::Event2 e=get_event(arg);
        State* next_state=a.dispatch(e,arg);
        if(0==next_state) break;
    }
    printf("\nmain::goodbye");
}
