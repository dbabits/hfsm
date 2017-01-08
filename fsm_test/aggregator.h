#if !defined AGGREGATOR_FSM_TEST
#define AGGREGATOR_FSM_TEST


#include "../include/experimental/fsm_with_dynamic_ver.h"
#include "../include/announcer.h"



class Aggregator; 

//this just declares all possible events for this application, across all states.
//no transition by default
class NO_VTABLE State:public dbabits::IState_base<Aggregator>
{
public:
        virtual void on_exit(Aggregator*, State*){delete this;}
        virtual Factory evt1(Aggregator*,const boost::any&){return no_transition;}
        virtual Factory evt2(Aggregator*,const boost::any&){return no_transition;}
        virtual Factory evt3(Aggregator*,const boost::any&){return no_transition;}
};


//concrete class that has a number of states and implements Fsm
class Aggregator:public dbabits::Fsm<Aggregator,State >{
public:
    Aggregator(){
        transition(state1::instance);
    }

    /*  State hierarhy.
        Each state needs to only define events that it responds to
        Note, state classes do not have to be nested inside here, but i just thought it more clearly shows affiliation:
              it's a state of our object

        IState_base<Aggregator>
           |
         State          <<-app-specific, prototypes every event handled by the system
         / | \
       /   |   \
     /     |     \
state1  state2    state3 <<each state implements functions for events that it cares about
                    |
                 state4
                    |
                 state5
    */



    class state1:public State{
        virtual Factory evt1(Aggregator*,const boost::any&);
        virtual Factory evt2(Aggregator*,const boost::any&);
    protected:
                
    public:
        state1(){}
        state1(Aggregator*){}
        //an example of static creation
        static  dbabits::IState_base<Aggregator>* instance(Aggregator* p){return &p->st1;}
        virtual void on_exit(Aggregator*, State*){
            printf("\nstatically allocated %s::on_exit()",typeid(*this).name());
        }
    };
    class state2:public State{
        virtual Factory evt1(Aggregator*,const boost::any&);
    public:
        state2(Aggregator*){}
    };
    class state3:public State{
        virtual Factory evt1(Aggregator*,const boost::any&);
        virtual Factory evt2(Aggregator*,const boost::any&);
        virtual Factory evt3(Aggregator*,const boost::any&);
    public:
        state3(Aggregator*){}
    };
    class state4:public state3,announcer<state4>
    {
        virtual Factory evt1(Aggregator*,const boost::any&);
    public:
        state4(Aggregator* p):state3(p){}
    };
    class state5:public state4,announcer<state5>
    {
        virtual Factory evt2(Aggregator*,const boost::any&);
        virtual Factory evt3(Aggregator*,const boost::any&);
    public:
        state5(Aggregator* p):state4(p){}
    };

    state1 st1;
};
#endif //AGGREGATOR_FSM_TEST

