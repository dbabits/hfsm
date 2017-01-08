#if !defined DMITRY_PELICAN_JAN_2004
#define DMITRY_PELICAN_JAN_2004

#include "../include/fsm.h"
#include "../include/announcer.h"

class Pelican;

//this just declares all possible events for this application, across all states.
template<class FSM>
class _declspec(novtable) State:public dbabits::IState_base<FSM,State>
{
public:
    //no transition by default
    virtual State<FSM>* off(FSM*,const boost::any&) {return this;}
    virtual State<FSM>* pedestrianWaiting(FSM*,const boost::any&){return this;}
    virtual State<FSM>* timeout(FSM*,const boost::any&){return this;}
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
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
    }m_Off;

    class operational:public State<Pelican>
                    ,announcer<operational>
    {
        virtual State<Pelican>* off(Pelican*,const boost::any&);

    public:
        enum VehiclesSignal { RED, YELLOW, GREEN };
        enum PedestriansSignal { DONT_WALK, BLANK, WALK };

        static void signalVehicles(enum VehiclesSignal sig);
        static void signalPedestrians(enum PedestriansSignal sig);
    };

    class vehiclesEnabled:public operational,announcer<vehiclesEnabled>
    {
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
        virtual void on_exit(Pelican*, State<Pelican>* new_state);

    };

    class vehiclesGreen:public vehiclesEnabled
                       ,announcer<vehiclesGreen>
    {
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
        virtual void on_exit(Pelican*, State<Pelican>* new_state);

    private:
        virtual State<Pelican>* pedestrianWaiting(Pelican*,const boost::any&);
        virtual State<Pelican>* timeout(Pelican*,const boost::any&);

        BOOL m_isPedestrianWaiting;
    }m_vehiclesGreen;

    class vehiclesGreenInt:public vehiclesEnabled
                          ,announcer<vehiclesGreenInt>
    {
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
    private:
        virtual State<Pelican>* pedestrianWaiting(Pelican*,const boost::any&);
    }m_vehiclesGreenInt;
    
    class vehiclesYellow:public vehiclesEnabled,announcer<vehiclesYellow>{
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
        virtual void on_exit(Pelican*, State<Pelican>* new_state);
    private:
        virtual State<Pelican>* timeout(Pelican*,const boost::any&);
    }m_vehiclesYellow;

    class pedestriansEnabled:public operational,announcer<pedestriansEnabled>
    {
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
    }m_pedestriansEnabled;

    class pedestriansWalk:public pedestriansEnabled,announcer<pedestriansWalk>
    {
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
        virtual void on_exit(Pelican*, State<Pelican>* new_state);

    private:
        virtual State<Pelican>* timeout(Pelican*,const boost::any&);
    }m_pedestriansWalk;

    class pedestriansFlash:public pedestriansEnabled,announcer<pedestriansFlash>{
    protected:
        virtual void on_enter(Pelican*,State<Pelican>* old_state);
        virtual void on_exit(Pelican*,State<Pelican>* new_state);

    private:
        int  pedestrianFlashCtr_;
        virtual State<Pelican>* timeout(Pelican*,const boost::any&);
    }m_pedestriansFlash;

   static BOOL CALLBACK dlgproc(HWND hwnd, UINT iEvt, WPARAM wParam, LPARAM lParam);
};

#endif //DMITRY_PELICAN_JAN_2004