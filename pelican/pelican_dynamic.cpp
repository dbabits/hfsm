#include "stdafx.h"
#include "resource.h"          
#include "pelican_dynamic.h"

using namespace dbabits;
HWND hWnd_=0;


Pelican::Off::Off(Pelican*){
    EndDialog(hWnd_, 0);
}


State<Pelican>::Factory Pelican::operational::off(Pelican*,const boost::any&){
    return factory<Pelican,Off>();
    //can simply return 0;
}

Pelican::vehiclesEnabled::vehiclesEnabled(Pelican* p){
    signalPedestrians(DONT_WALK);
}

Pelican::vehiclesGreen::vehiclesGreen(Pelican* p):vehiclesEnabled(p),isPedestrianWaiting_(FALSE)
{
    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreen");
    SetTimer(hWnd_, 1, VEHICLES_GREEN_MIN_TOUT, 0);
    signalVehicles(GREEN); 
}
Pelican::vehiclesGreen::~vehiclesGreen(){
    KillTimer(hWnd_, 1); 
}


State<Pelican>::Factory Pelican::vehiclesGreen::pedestrianWaiting(Pelican*,const boost::any&){
    isPedestrianWaiting_ = TRUE;
    return no_transition;
}
State<Pelican>::Factory Pelican::vehiclesGreen::timeout(Pelican* p,const boost::any&){
    if (isPedestrianWaiting_) 
        return factory<Pelican,vehiclesYellow>();
    else 
        
    #ifdef VEHICLESGREENINT_ON_STACK //use custom allocator then
        return vehiclesGreenInt::instance;
    #else
        return factory<Pelican,vehiclesGreenInt>();
    #endif
}




#ifdef VEHICLESGREENINT_ON_STACK
    IState_base<Pelican>* Pelican::vehiclesGreenInt::instance(Pelican* p){
        return &p->m_vehiclesGreenInt;
    }

    Pelican::vehiclesGreenInt::vehiclesGreenInt(Pelican* p):vehiclesEnabled(p){ }

    void Pelican::vehiclesGreenInt::on_enter(Pelican* p){
        vehiclesEnabled::on_enter(p);
        SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt"); 
        signalVehicles(GREEN); 
    }

    void Pelican::vehiclesGreenInt::on_exit(Pelican* p){
        vehiclesEnabled::on_exit(p);//not really necessary
    }

    void Pelican::vehiclesGreenInt::operator delete(void* p){}//this is a stack object!

#else //on the heap
    Pelican::vehiclesGreenInt::vehiclesGreenInt(Pelican* p):vehiclesEnabled(p){ 
        SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt"); 
        signalVehicles(GREEN); 
    }

#endif


State<Pelican>::Factory Pelican::vehiclesGreenInt::pedestrianWaiting(Pelican* p,const boost::any&){
    return factory<Pelican,vehiclesYellow>();
}



Pelican::vehiclesYellow::vehiclesYellow(Pelican* p):vehiclesEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesYellow");
    SetTimer(hWnd_, 1, VEHICLES_YELLOW_TOUT, 0);
    signalVehicles(YELLOW);
}

Pelican::vehiclesYellow::~vehiclesYellow(){
    KillTimer(hWnd_, 1);
}

State<Pelican>::Factory Pelican::vehiclesYellow::timeout(Pelican* p,const boost::any&){
    return factory<Pelican,pedestriansWalk>(); 
}

Pelican::pedestriansEnabled::pedestriansEnabled(Pelican* p){ 
    signalVehicles(RED); 
}


Pelican::pedestriansWalk::pedestriansWalk(Pelican* p):pedestriansEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansWalk");
    signalPedestrians(WALK);
    UINT_PTR t=SetTimer(hWnd_, 1, PEDESTRIANS_WALK_TOUT, 0);
}
        
Pelican::pedestriansWalk::~pedestriansWalk(){ 
    KillTimer(hWnd_, 1); 
}

State<Pelican>::Factory Pelican::pedestriansWalk::timeout(Pelican* p,const boost::any&){
    return factory<Pelican,pedestriansFlash>(); 
}

Pelican::pedestriansFlash::pedestriansFlash(Pelican* p):pedestriansEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansFlash");
    SetTimer(hWnd_, 1, PEDESTRIANS_FLASH_TOUT, 0);
    pedestrianFlashCtr_ = PEDESTRIANS_FLASH_NUM*2 + 1;
}

Pelican::pedestriansFlash::~pedestriansFlash(){
    KillTimer(hWnd_, 1);
}

State<Pelican>::Factory Pelican::pedestriansFlash::timeout(Pelican* p,const boost::any&){
    if (--pedestrianFlashCtr_ == 0)              //done flashing?
        return factory<Pelican,vehiclesGreen>(); //note the change-we enter derived state directly
    
    //even counter?DONT_WALK:BLANK
    signalPedestrians((pedestrianFlashCtr_ & 1) == 0? DONT_WALK : BLANK);

    return no_transition;
}

Pelican::Pelican()
        #ifdef VEHICLESGREENINT_ON_STACK
            :m_vehiclesGreenInt(this)
        #endif
{}

void Pelican::init(){
    operational::signalVehicles(operational::RED);
    operational::signalPedestrians(operational::DONT_WALK);

    //take the initial transition. note the change here
    transition(factory<Pelican,vehiclesGreen>());
}

void Pelican::operational::signalVehicles(enum VehiclesSignal sig) {
   ShowWindow(GetDlgItem(hWnd_, IDC_RED), sig == RED);
   ShowWindow(GetDlgItem(hWnd_, IDC_YELLOW), sig == YELLOW);
   ShowWindow(GetDlgItem(hWnd_, IDC_GREEN), sig == GREEN);
}

void Pelican::operational::signalPedestrians(enum PedestriansSignal sig) {
   ShowWindow(GetDlgItem(hWnd_, IDC_DONT_WALK), sig == DONT_WALK);
   ShowWindow(GetDlgItem(hWnd_, IDC_BLANK), sig == BLANK);
   ShowWindow(GetDlgItem(hWnd_, IDC_WALK), sig == WALK);
}



BOOL CALLBACK Pelican::dlgproc(HWND hwnd,UINT iMsg,WPARAM wParam, LPARAM lParam){
   static Pelican app;
   Pelican::Event2 e(0);

   switch (iMsg) {
    case WM_INITDIALOG:
        hWnd_=hwnd;
        app.init();
        SendMessage(hwnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1)));
        break;
    case WM_TIMER:
        e =State<Pelican>::timeout;
        break;
    case WM_COMMAND:                     
        switch (LOWORD(wParam)) {
        case IDCANCEL:
            e =State<Pelican>::off;
            break;
        case IDC_PEDESTRIAN_WAITING:
            e = State<Pelican>::pedestrianWaiting;
            break;
        }      
   }

   if(e){
    boost::any arg;
    if(0==app.dispatch(e,arg))
        EndDialog(hWnd_, 0);
   }
   return FALSE;            
}

int main(int argc, char* argv[]){
    return DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG), 0, Pelican::dlgproc);                  
}





