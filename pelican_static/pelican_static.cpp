#include "stdafx.h"
#include "resource.h"          
#include "pelican_static.h"

using namespace dbabits;
HWND hWnd_=0;

void Pelican::Off::on_enter(Pelican*,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in Off");
        return;
    }
    printf("->Off::on_enter");
    EndDialog(hWnd_, 0);
}

void Pelican::vehiclesEnabled::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in vehiclesEnabled");
        return;
    }
    operational::on_enter(p,old_state);

    printf("->vehiclesEnabled::on_enter");
    signalPedestrians(DONT_WALK);
}

void Pelican::vehiclesGreen::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in vehiclesGreen");
        return;
    }
    vehiclesEnabled::on_enter(p,old_state);

    printf("->vehiclesGreen::on_enter");


    m_isPedestrianWaiting=FALSE;
    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreen");
    SetTimer(hWnd_, 1, VEHICLES_GREEN_MIN_TOUT, 0);
    signalVehicles(GREEN); 
}

void Pelican::vehiclesGreenInt::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in vehiclesGreenInt");
        return;
    }
    vehiclesEnabled::on_enter(p,old_state);

    printf("->vehiclesGreenInt::on_enter");

    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt"); 
    signalVehicles(GREEN); 
}

void Pelican::vehiclesYellow::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in vehiclesYellow");
        return;
    }
    vehiclesEnabled::on_enter(p,old_state);
    printf("->vehiclesYellow::on_enter");

    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesYellow");
    SetTimer(hWnd_, 1, VEHICLES_YELLOW_TOUT, 0);
    signalVehicles(YELLOW);
}

void Pelican::pedestriansEnabled::on_enter(Pelican* p,State<Pelican>* old_state){ 
    if (old_state && old_state->derives_from(this)) {
        printf("-already in pedestriansEnabled");
        return;
    }
    operational::on_enter(p,old_state);

    printf("->pedestriansEnabled::on_enter");

    signalVehicles(RED); 
}

void Pelican::pedestriansWalk::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in pedestriansWalk");
        return;
    }
    pedestriansEnabled::on_enter(p,old_state);

    printf("->pedestriansWalk::on_enter");

    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansWalk");
    signalPedestrians(WALK);
    UINT_PTR t=SetTimer(hWnd_, 1, PEDESTRIANS_WALK_TOUT, 0);
}

void Pelican::pedestriansFlash::on_enter(Pelican* p,State<Pelican>* old_state){
    if (old_state && old_state->derives_from(this)) {
        printf("-already in pedestriansFlash");
        return;
    }
    pedestriansEnabled::on_enter(p,old_state);

    printf("->pedestriansFlash::on_enter");

    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansFlash");
    SetTimer(hWnd_, 1, PEDESTRIANS_FLASH_TOUT, 0);
    pedestrianFlashCtr_ = PEDESTRIANS_FLASH_NUM*2 + 1;
}

void Pelican::vehiclesEnabled::on_exit(Pelican* p,State<Pelican>* new_state){ 
    if (new_state && new_state->derives_from(this)) {
        printf("-stay in vehiclesEnabled");
        return;
    }

    printf("->vehiclesEnabled::on_exit");

    operational::on_exit(p,new_state);
}


void Pelican::pedestriansWalk::on_exit(Pelican* p,State<Pelican>* new_state){ 
    if (new_state && new_state->derives_from(this)) {
        printf("-stay in pedestriansWalk");
        return;
    }
    printf("->pedestriansWalk::on_exit");

    KillTimer(hWnd_, 1); 
    pedestriansEnabled::on_exit(p,new_state);
}

void Pelican::vehiclesYellow::on_exit(Pelican* p,State<Pelican>* new_state){
    if (new_state && new_state->derives_from(this)) {
        printf("-stay in vehiclesYellow");
        return;
    }
    printf("->vehiclesYellow::on_exit");

    KillTimer(hWnd_, 1);

    vehiclesEnabled::on_exit(p,new_state);
}
void Pelican::vehiclesGreen::on_exit(Pelican* p,State<Pelican>* new_state){
    if (new_state && new_state->derives_from(this)) {
        printf("-stay in vehiclesGreen");
        return;
    }

    printf("->vehiclesGreen::on_exit");
    
    KillTimer(hWnd_, 1); 
    
    vehiclesEnabled::on_exit(p,new_state);
}

void Pelican::pedestriansFlash::on_exit(Pelican* p,State<Pelican>* new_state){
    if (new_state && new_state->derives_from(this)) {
        printf("-stay in pedestriansFlash");
        return;
    }

    printf("->pedestriansFlash::on_exit");

    KillTimer(hWnd_, 1);

    pedestriansEnabled::on_exit(p,new_state);
}

State<Pelican>* Pelican::operational::off(Pelican* p,const boost::any&){
    return &p->m_Off;    //can simply return 0;
}

State<Pelican>* Pelican::vehiclesGreen::pedestrianWaiting(Pelican*,const boost::any&){
    m_isPedestrianWaiting = TRUE;
    return this;
}
State<Pelican>* Pelican::vehiclesGreen::timeout(Pelican* p,const boost::any&){
    if (m_isPedestrianWaiting)return &p->m_vehiclesYellow;
    else                      return &p->m_vehiclesGreenInt;
}

State<Pelican>* Pelican::vehiclesGreenInt::pedestrianWaiting(Pelican* p,const boost::any&){
    return &p->m_vehiclesYellow;
}

State<Pelican>* Pelican::vehiclesYellow::timeout(Pelican* p,const boost::any&){
    return  &p->m_pedestriansWalk;
}

State<Pelican>* Pelican::pedestriansWalk::timeout(Pelican* p,const boost::any&){
    return &p->m_pedestriansFlash;
}

State<Pelican>* Pelican::pedestriansFlash::timeout(Pelican* p,const boost::any&){
    if (--pedestrianFlashCtr_ == 0) //done flashing?
        return &p->m_vehiclesGreen; //note the change from Miro Samek's-we enter derived state directly
    
    //even counter?DONT_WALK:BLANK
    signalPedestrians((pedestrianFlashCtr_ & 1) == 0? DONT_WALK : BLANK);

    return this;
}

Pelican::Pelican(){}

void Pelican::init(){
    operational::signalVehicles(operational::RED);
    operational::signalPedestrians(operational::DONT_WALK);

    //make the initial transition. note the change from Miro Samek's-we enter derived state directly
    transition(&m_vehiclesGreen);
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
   Pelican::Event e(0);

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
