// Copyright 2003 by Dmitry Babitsky
// All Rights Reserved.
// Permission is granted to use this code without restriction as long as this copyright notice appears in this file.
#ifndef DMITRY_BABITSKY_FSM_JAN_2004
#define DMITRY_BABITSKY_FSM_JAN_2004

#if defined _MSC_VER
#define NO_VTABLE _declspec(novtable)
#else
#define NO_VTABLE 
#endif

namespace boost{class any;}


namespace dbabits{
    template<class DERIVED_FSM,class STATE>
    class static_alloc{
        typedef typename STATE* (STATE::*Event)(DERIVED_FSM*,const boost::any&); //prototype for all event functions

        static STATE* dispatch(Event e, const boost::any& arg,STATE* curr_state){


        }
        static void transition(STATE* next_state,STATE* curr_state){

        }
    };

    template<class DERIVED_FSM,class STATE>
    class dynamic_alloc{
        typedef typename STATE::Factory (STATE::*Event)(DERIVED_FSM*,const boost::any&); //prototype for all event functions

        static STATE* dispatch(Event e, const boost::any& arg,STATE* curr_state){


        }
        static void transition(typename STATE::Factory next_state_factory){

        }
    };

template <class DERIVED_FSM,class STATE, class traits>
class NO_VTABLE Fsm{
public:
   
    //////simple version, for stack allocated states/////////////////////////////////////////////
    //note that constructors/destructors should not be used in this case, instead rely on on_enter/on_exit
    typedef typename traits::Event Event;
    
    STATE* dispatch(Event e, const boost::any& arg){
        return traits::dispatch(e,arg,current_state);
    }

    void transition(STATE* next_state){
        if(curr_state==next_state) return;

        if(curr_state) curr_state->on_exit(that()); 
        if(next_state) next_state->on_enter(that());

        curr_state=next_state;
    }



protected:
    Fsm():curr_state(0){}
    virtual ~Fsm(){}
private:
    STATE* curr_state;
    inline DERIVED_FSM* that(){ return static_cast<DERIVED_FSM*>(this);}
};

template<class FSM>
class NO_VTABLE IState_base{
protected:
    virtual ~IState_base(){}
public:
    virtual void on_exit(FSM*){}
    virtual void on_enter(FSM*){}

    typedef IState_base<FSM>* (*Factory)(FSM*);
    static IState_base<FSM>* no_transition(FSM*){/*assert(0)*/return 0;}
};

//helpers for the more complex(dynamic) allocation of states
template<class FSM,class STATE>
struct instantiator:public STATE{
    static IState_base<FSM>* instance(FSM* fsm){ return new STATE(fsm); }
};

//need to pass dummy state to make this function work correctly in VC6! otherwise it just instantiates the same fn for every template parameter
//not a problem in VC 7
template<class FSM,class STATE> inline
typename IState_base<FSM>::Factory factory(STATE* p=0){ return instantiator<FSM,STATE>::instance;}

};//dbabits


#endif //DMITRY_BABITSKY_FSM_JAN_2004

