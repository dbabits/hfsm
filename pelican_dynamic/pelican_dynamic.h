#if !defined DMITRY_PELICAN_JAN_2004
#define DMITRY_PELICAN_JAN_2004

#include "../include/experimental/fsm_with_dynamic_ver.h"
#include "../include/announcer.h"


//#define VEHICLESGREENINT_ON_STACK

class Pelican;


//this just declares all possible events for this application, across all states.
template<class FSM>
class _declspec(novtable) State:public dbabits::IState_base<FSM>
{
public:
    
    //by default, we'll assume that all states are heap-allocated,and will destroy them here.
    //Owervise, override either this function or operator delete
    virtual void on_exit(FSM*,State* ){delete this;} 

    //no transition by default
    virtual Factory off(FSM*,const boost::any&) {return no_transition;}
    virtual Factory pedestrianWaiting(FSM*,const boost::any&){return no_transition;}
    virtual Factory timeout(FSM*,const boost::any&){return no_transition;}
};


enum PelicanTimeouts {              // various timeouts in milliseconds
   VEHICLES_GREEN_MIN_TOUT = 6000,  // minimum green for vehicles
   VEHICLES_YELLOW_TOUT    = 2000,  // yellow for vehicles
   PEDESTRIANS_WALK_TOUT   = 4000,  // walking time for pedestrians
   PEDESTRIANS_FLASH_TOUT  = 500,   // flashing interval for ped.
   PEDESTRIANS_FLASH_NUM   = 3,     // number of flashes for ped.
}; 



class Pelican : public dbabits::Fsm<Pelican,State<Pelican> > {
public:
   Pelican();
   void init();

    class Off:public State<Pelican>,announcer<Off>
    {
    public:
        Off(Pelican*);
    };
    class operational:public State<Pelican>
                    ,announcer<operational>
    {
        virtual Factory off(Pelican*,const boost::any&);

    public:
        enum VehiclesSignal { RED, YELLOW, GREEN };
        enum PedestriansSignal { DONT_WALK, BLANK, WALK };

        static void signalVehicles(enum VehiclesSignal sig);
        static void signalPedestrians(enum PedestriansSignal sig);
    };
    class vehiclesEnabled:public operational,announcer<vehiclesEnabled>
    {
    protected:
        vehiclesEnabled(Pelican*);
    };
    class vehiclesGreen:public vehiclesEnabled
                       ,announcer<vehiclesGreen>
    {
    public:
        vehiclesGreen(Pelican*);
        ~vehiclesGreen();
    private:
        virtual Factory pedestrianWaiting(Pelican*,const boost::any&);
        virtual Factory timeout(Pelican*,const boost::any&);
        BOOL isPedestrianWaiting_;
    };

    //this class is conditionally instantiated on the stack, just to demonstrate the technique
    class vehiclesGreenInt:public vehiclesEnabled
                          ,announcer<vehiclesGreenInt>
    {
        vehiclesGreenInt();

        virtual Factory pedestrianWaiting(Pelican*,const boost::any&);
    public:
        vehiclesGreenInt(Pelican*);

    #ifdef VEHICLESGREENINT_ON_STACK //create custom instantiator to return stack object instead
        static  IState_base<Pelican>* instance(Pelican* p);

        virtual void on_enter(Pelican*);
        virtual void on_exit(Pelican*);

        void operator delete(void* p);//this is a stack object!
    #endif

    };
    
    #ifdef VEHICLESGREENINT_ON_STACK
     vehiclesGreenInt  m_vehiclesGreenInt;
    #endif



    class vehiclesYellow:public vehiclesEnabled,announcer<vehiclesYellow>{
    public:
        vehiclesYellow(Pelican*);
        ~vehiclesYellow();
    private:
        virtual Factory timeout(Pelican*,const boost::any&);
    };

    class pedestriansEnabled:public operational,announcer<pedestriansEnabled>
    {
    public:
        pedestriansEnabled(Pelican*);
    };
    class pedestriansWalk:public pedestriansEnabled,announcer<pedestriansWalk>
    {
    public:
        pedestriansWalk(Pelican*);
        ~pedestriansWalk();
    private:
        virtual Factory timeout(Pelican*,const boost::any&);
    };
    class pedestriansFlash:public pedestriansEnabled,announcer<pedestriansFlash>{
    public:
        pedestriansFlash(Pelican*);
        ~pedestriansFlash();
    private:
        int  pedestrianFlashCtr_;
        virtual Factory timeout(Pelican*,const boost::any&);
    } ;


   static BOOL CALLBACK dlgproc(HWND hwnd, UINT iEvt, WPARAM wParam, LPARAM lParam);
};

#endif //DMITRY_PELICAN_JAN_2004