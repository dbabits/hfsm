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

    template <class DERIVED_FSM,class STATE>
    class stack_policy{
        typedef typename STATE*         (STATE::*Event)(DERIVED_FSM*,const boost::any&); //prototype for all event functions
    
        STATE* dispatch(Event e, const boost::any& arg){
            if(curr_state && e){
                STATE* next_state=(curr_state->*e)(that(),arg);
                if(next_state!=curr_state) transition(next_state);
            }
            return curr_state;
        }

        void transition(STATE* next_state){
            if(curr_state==next_state) return;

            if(curr_state) curr_state->on_exit(that()); 
            if(next_state) next_state->on_enter(that());

            curr_state=next_state;
        }
    protected:
        stack_policy():curr_state(0){}
        ~stack_policy(){}
    private:
        STATE* curr_state;
        inline DERIVED_FSM* that(){ return static_cast<DERIVED_FSM*>(this);}
    };


    template <class DERIVED_FSM,class STATE>
    class heap_policy{

        typedef typename STATE::Factory (STATE::*Event2)(DERIVED_FSM*,const boost::any&); //prototype for all event functions

        STATE* dispatch(Event2 e, const boost::any& arg){
            if(curr_state && e){

                STATE::Factory next_state_factory=(curr_state->*e)(that(),arg);
            
                if(next_state_factory!=STATE::no_transition)  transition(next_state_factory);
            }
            return curr_state;
        }

        void transition(typename STATE::Factory next_state_factory){
            if(next_state_factory==STATE::no_transition) return; 

            if(curr_state) curr_state->on_exit(that());
        
            STATE* next_state=0;
            if(next_state_factory){
                next_state=static_cast<STATE*>((*next_state_factory)(that()));
                if(next_state) next_state->on_enter(that());
            }

            curr_state=next_state;
        }
    };



template <class DERIVED_FSM,class STATE,class STATE_ALLOC_POLICY=stack_policy>
class NO_VTABLE Fsm: public STATE_ALLOC_POLICY<DERIVED_FSM, STATE> {
    protected:
        Fsm(){}
        virtual ~Fsm(){}
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

};//namespace dbabits


#endif //DMITRY_BABITSKY_FSM_JAN_2004

