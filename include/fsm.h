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
class NO_VTABLE Fsm{
public:
   
    //for stack allocated states
    //note that constructors/destructors should not be used in this case, instead rely on on_enter/on_exit
    typedef typename STATE* (STATE::*Event)(DERIVED_FSM*,const boost::any&); //prototype for all event functions
    
    STATE* dispatch(Event e, const boost::any& arg){
        if(curr_state && e){
            STATE* next_state=(curr_state->*e)(that(),arg);
            if(next_state!=curr_state) transition(next_state);
        }
        return curr_state;
    }

    void transition(STATE* next_state){
        if(curr_state==next_state) return;
  
        if(curr_state) curr_state->on_exit(that(),next_state); 
        if(next_state) next_state->on_enter(that(),curr_state);

        curr_state=next_state;
    }

protected:
    Fsm():curr_state(0){}
    virtual ~Fsm(){}
private:
    STATE* curr_state;
    inline DERIVED_FSM* that(){ return static_cast<DERIVED_FSM*>(this);}
};

template<class FSM,class STATE>
class NO_VTABLE IState_base{
protected:
    virtual ~IState_base(){}
public:
    virtual void on_exit (FSM*,STATE* new_state){}
    virtual void on_enter(FSM*,STATE* old_state){}

    #if _MSC_VER < 1200 
     #error derives_from() will not compile in VC++ before version 7. create this as stand-alone fn then.
    #endif

    template<class T>
    bool derives_from(T*){ 
        const char* derives= 0!=dynamic_cast<T*>(this)?"YES":"NO";
        printf("\n%s derives from %s ?-%s",typeid(*this).name(),typeid(T).name(),derives);
        return 0!=dynamic_cast<T*>(this);
    }

};

//template<class T,class FSM>
//bool derives_from(IState_base<FSM>* that){ return 0!=dynamic_cast<T*>(that);}

};//namespace dbabits


#endif //DMITRY_BABITSKY_FSM_JAN_2004

