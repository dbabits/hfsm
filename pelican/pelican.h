#pragma once

#include "fsm_my.h"
#include "announcer.h"

class Pelican;
using namespace version1;
//this just declares all possible events for this application, across all states.
class _declspec(novtable) PelicanState:public IState_base<Pelican>
{
public:

    //these 2 must be here,for Fsm template to instantiate
    virtual void on_exit(Pelican*){}//by default, we'll assume that all states are heap-allocated,override if necessary
    virtual void on_enter(Pelican*){}//since we are assuming dynamic allocation, ctor can be used instead, override if necessary

    //no transition by default
    virtual PelicanState* off(Pelican*,const boost::any&) {return this;}
    virtual PelicanState* pedestrianWaiting(Pelican*,const boost::any&){return this;}
    virtual PelicanState* timeout(Pelican*,const boost::any&){return this;}
};


enum PelicanTimeouts {              // various timeouts in milliseconds
   VEHICLES_GREEN_MIN_TOUT = 8000,  // minimum green for vehicles
   VEHICLES_YELLOW_TOUT    = 2000,  // yellow for vehicles
   PEDESTRIANS_WALK_TOUT   = 4000,  // walking time for pedestrians
   PEDESTRIANS_FLASH_TOUT  = 500,   // flashing interval for ped.
   PEDESTRIANS_FLASH_NUM   = 3,     // number of flashes for ped.
}; 



class Pelican : public Fsm<Pelican,PelicanState> {
public:
   void init();
   Pelican():vehiclesGreenInt_state(this){}

    class Off:public PelicanState,announcer<Off>
    {
    public:
        Off();
    };
    class operational:public PelicanState,announcer<operational>
    {
        virtual PelicanState* off(Pelican*,const boost::any&);
    };
    class vehiclesEnabled:public operational,announcer<vehiclesEnabled>
    {
    protected:
        vehiclesEnabled(Pelican*);
    };
    class vehiclesGreen:public vehiclesEnabled,announcer<vehiclesGreen>
    {
    public:
        vehiclesGreen(Pelican*);
        ~vehiclesGreen();
    private:
        virtual void on_enter(Pelican*);
        virtual void on_exit(Pelican*);
        virtual PelicanState* pedestrianWaiting(Pelican*,const boost::any&);
        virtual PelicanState* timeout(Pelican*,const boost::any&);
        BOOL isPedestrianWaiting_;
    };

   

    //this class is instantiated on the stack, just to demonstrate the technique
    class vehiclesGreenInt:public vehiclesEnabled
                          ,announcer<vehiclesGreenInt>
    {
    public:
        vehiclesGreenInt(Pelican*);

        //override new to return the stack object instead;
        inline void* operator new(size_t nbytes, Pelican* p);
        void operator delete(void* p,Pelican*){}//just to prevent warning
    private:
        vehiclesGreenInt();
        inline void* operator new(size_t nbytes);//disallow
        void operator delete(void* p);

        virtual void on_enter(Pelican*);
        virtual void on_exit(Pelican*);

        virtual PelicanState* pedestrianWaiting(Pelican*,const boost::any&);
    } vehiclesGreenInt_state;



    class vehiclesYellow:public vehiclesEnabled,announcer<vehiclesYellow>{
    public:
        vehiclesYellow(Pelican*);
        ~vehiclesYellow();
    private:
        virtual PelicanState* timeout(Pelican*,const boost::any&);
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
        virtual PelicanState* timeout(Pelican*,const boost::any&);
    };
    class pedestriansFlash:public pedestriansEnabled,announcer<pedestriansFlash>{
    public:
        pedestriansFlash(Pelican*);
        ~pedestriansFlash();
    private:
        int  pedestrianFlashCtr_;
        virtual PelicanState* timeout(Pelican*,const boost::any&);
    };

private:
   enum VehiclesSignal { RED, YELLOW, GREEN };
   enum PedestriansSignal { DONT_WALK, BLANK, WALK };
   
   void signalVehicles(enum VehiclesSignal sig);
   void signalPedestrians(enum PedestriansSignal sig);

   friend BOOL CALLBACK PelicanDlg(HWND hwnd, UINT iEvt, WPARAM wParam, LPARAM lParam);
};

/*
template<class T>
T* instance(Pelican* p){return new T(p);}

template<>
Pelican::vehiclesGreenInt* instance<Pelican::vehiclesGreenInt>(Pelican* p){
    return new (p) Pelican::vehiclesGreenInt(p);
}
*/