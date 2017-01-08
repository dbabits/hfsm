//dmitry

#include "stdafx.h"
#include "resource.h"                             // pelican resources
#include "pelican.h"

HWND hWnd_=0;

Pelican::Off::Off(){
    EndDialog(hWnd_, 0);
}


PelicanState* Pelican::operational::off(Pelican*,const boost::any&){
    return new Pelican::Off;
}

Pelican::vehiclesEnabled::vehiclesEnabled(Pelican* p){
    p->signalPedestrians(DONT_WALK);
}

//----just for demonstration, we'll use on_enter/on_exit instead
Pelican::vehiclesGreen::vehiclesGreen(Pelican* p):vehiclesEnabled(p){}
Pelican::vehiclesGreen::~vehiclesGreen(){}

void Pelican::vehiclesGreen::on_enter(Pelican* p){
    vehiclesEnabled::on_enter(p);//must call superstate first, happens implicitly with ctor

    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreen");
    SetTimer(hWnd_, 1, VEHICLES_GREEN_MIN_TOUT, 0);
    p->signalVehicles(GREEN);
    isPedestrianWaiting_ = FALSE;

}

void Pelican::vehiclesGreen::on_exit(Pelican* p){
    BOOL success=KillTimer(hWnd_, 1); 
    vehiclesEnabled::on_exit(p);//must call superstate last, happens implicitly with dtor
}
//------------------------------------------



PelicanState* Pelican::vehiclesGreen::pedestrianWaiting(Pelican*,const boost::any&){
    isPedestrianWaiting_ = TRUE;
    return this;
}
PelicanState* Pelican::vehiclesGreen::timeout(Pelican* p,const boost::any&){
    if (isPedestrianWaiting_) 
        return new Pelican::vehiclesYellow(p);
    else 
        //return Pelican::vehiclesGreenInt::instance(p);
        return instance<vehiclesGreenInt>(p);
}

/*
PelicanState* Pelican::vehiclesGreenInt::instance(Pelican* p){
    return new (p) Pelican::vehiclesGreenInt(p);
    //return new Pelican::vehiclesGreenInt(p);
    //return &p->vehiclesGreenInt_state;
}
*/
Pelican::vehiclesGreenInt::vehiclesGreenInt(Pelican* p):vehiclesEnabled(p){ 
    //SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt"); 
}
void Pelican::vehiclesGreenInt::on_enter(Pelican* p){
    vehiclesEnabled::on_enter(p);//not really necessary
    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt"); 
}

void Pelican::vehiclesGreenInt::on_exit(Pelican* p){
    vehiclesEnabled::on_exit(p);//not really necessary
}

//this ensures that the ctor will still be called, even for an existing object
inline void* Pelican::vehiclesGreenInt::operator new(size_t nbytes, Pelican* p){
    return &p->vehiclesGreenInt_state;
}

void Pelican::vehiclesGreenInt::operator delete(void* p){}


PelicanState* Pelican::vehiclesGreenInt::pedestrianWaiting(Pelican* p,const boost::any&){
    return new Pelican::vehiclesYellow(p);
}



Pelican::vehiclesYellow::vehiclesYellow(Pelican* p):vehiclesEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "vehiclesYellow");
    SetTimer(hWnd_, 2, VEHICLES_YELLOW_TOUT, 0);
    p->signalVehicles(YELLOW);
}

Pelican::vehiclesYellow::~vehiclesYellow(){
    BOOL success=KillTimer(hWnd_, 2);
}

PelicanState* Pelican::vehiclesYellow::timeout(Pelican* p,const boost::any&){
    return new Pelican::pedestriansWalk(p);//Note the change
}

Pelican::pedestriansEnabled::pedestriansEnabled(Pelican* p){ 
    p->signalVehicles(RED); 
}

Pelican::pedestriansWalk::pedestriansWalk(Pelican* p):pedestriansEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansWalk");
    p->signalPedestrians(WALK);
    UINT_PTR t=SetTimer(hWnd_, 3, PEDESTRIANS_WALK_TOUT, 0);
}
        
Pelican::pedestriansWalk::~pedestriansWalk(){ 
    BOOL success=KillTimer(hWnd_, 3); 
}

PelicanState* Pelican::pedestriansWalk::timeout(Pelican* p,const boost::any&){
    return new Pelican::pedestriansFlash(p);
}

Pelican::pedestriansFlash::pedestriansFlash(Pelican* p):pedestriansEnabled(p){
    SetDlgItemText(hWnd_, IDC_STATE, "pedestriansFlash");
    SetTimer(hWnd_, 4, PEDESTRIANS_FLASH_TOUT, 0);
    pedestrianFlashCtr_ = PEDESTRIANS_FLASH_NUM*2 + 1;
}

Pelican::pedestriansFlash::~pedestriansFlash(){
    BOOL success=KillTimer(hWnd_, 4);
}

PelicanState* Pelican::pedestriansFlash::timeout(Pelican* p,const boost::any&){
    if (--pedestrianFlashCtr_ == 0)               // done flashing?
        return new Pelican::vehiclesGreen(p);       //here's the punch-dmitry.
    
    //even counter?DONT_WALK:BLANK
    p->signalPedestrians((pedestrianFlashCtr_ & 1) == 0? DONT_WALK : BLANK);

    return this;//stay here
}


void Pelican::init(){
    signalVehicles(RED);
    signalPedestrians(DONT_WALK);
    transition(new vehiclesGreen(this));    //note the change here
}


void Pelican::signalVehicles(enum VehiclesSignal sig) {
   ShowWindow(GetDlgItem(hWnd_, IDC_RED), sig == RED);
   ShowWindow(GetDlgItem(hWnd_, IDC_YELLOW), sig == YELLOW);
   ShowWindow(GetDlgItem(hWnd_, IDC_GREEN), sig == GREEN);
}

void Pelican::signalPedestrians(enum PedestriansSignal sig) {
   ShowWindow(GetDlgItem(hWnd_, IDC_DONT_WALK), sig == DONT_WALK);
   ShowWindow(GetDlgItem(hWnd_, IDC_BLANK), sig == BLANK);
   ShowWindow(GetDlgItem(hWnd_, IDC_WALK), sig == WALK);
}


extern "C" void onAssert__(char const *file, unsigned line) {
   char str[160];
   sprintf(str, "Assertion failed in %s, line %d", file, line);        
   MessageBox(GetActiveWindow(), str, "Pelican", MB_ICONEXCLAMATION | MB_OK);
   exit(-1);
}

Pelican app;

BOOL CALLBACK PelicanDlg(HWND hwnd,UINT iMsg,WPARAM wParam, LPARAM lParam){
   Pelican::Event e(0);

   switch (iMsg) {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_QP)));
        hWnd_=hwnd;
        app.init();                     // take the initial transition
        break;//return TRUE;
    case WM_TIMER:
        e =PelicanState::timeout;
        break;
    case WM_COMMAND:                     
        switch (LOWORD(wParam)) {
        case IDCANCEL:
            e =PelicanState::off;
            break;
        case IDC_PEDESTRIAN_WAITING:
            e = PelicanState::pedestrianWaiting;
            break;
        }      
   }
   if(e){
    boost::any arg;
    app.dispatch(e,arg);
   }
   return FALSE;            
}

//#pragma comment(lib,"comctl32")
void main(int argc, char* argv[]){
   
   //InitCommonControls(); 
   DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG), 0, PelicanDlg);                  
}

///////////////////////////////////////////////
/*
//...................................................................
Pelican::Pelican() : QHsm((QPseudoState)&Pelican::initial) {
}
// HSM definition ---------------------------------------------------
void Pelican::initial(QEvent const *e) {
   
   signalVehicles(RED);
   signalPedestrians(DONT_WALK);
   Q_INIT(&Pelican::operational);
}
//...................................................................
QSTATE Pelican::off(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      EndDialog(hWnd_, 0);
      return 0;
   }
   return (QSTATE)&QHsm::top;
}
//...................................................................
QSTATE Pelican::operational(QEvent const *e) {
   switch (e->sig) {
   case Q_INIT_SIG:
      Q_INIT(&Pelican::vehiclesEnabled);
      return 0;
   case OFF_SIG:
      Q_TRAN(&Pelican::off);
      return 0;
   }
   if (e->sig >= Q_USER_SIG) {
      isHandled_ = FALSE;
   }
   return (QSTATE)&QHsm::top;
}
//...................................................................
QSTATE Pelican::vehiclesEnabled(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      signalPedestrians(DONT_WALK);
      return 0;
   case Q_INIT_SIG:
      Q_INIT(&Pelican::vehiclesGreen);
      return 0;
   }
   return (QSTATE)&Pelican::operational;
}
//...................................................................
QSTATE Pelican::vehiclesGreen(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreen");
      SetTimer(hWnd_, 1, VEHICLES_GREEN_MIN_TOUT, 0);
      signalVehicles(GREEN);
      isPedestrianWaiting_ = FALSE;
      return 0;
   case PEDESTRIAN_WAITING_SIG:
      isPedestrianWaiting_ = TRUE;
      return 0;
   case TIMEOUT_SIG:
      if (isPedestrianWaiting_) {
         Q_TRAN(&Pelican::vehiclesYellow);
      }
      else {
         Q_TRAN(&Pelican::vehiclesGreenInt);
      }
      return 0;
   case Q_EXIT_SIG:
      KillTimer(hWnd_, 1);                   // don't leak the timer!
      return 0;
   }
   return (QSTATE)&Pelican::vehiclesEnabled;
}
//...................................................................
QSTATE Pelican::vehiclesGreenInt(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      SetDlgItemText(hWnd_, IDC_STATE, "vehiclesGreenInt");
      return 0;
   case PEDESTRIAN_WAITING_SIG:
      Q_TRAN(&Pelican::vehiclesYellow);
      return 0;
   }
   return (QSTATE)&Pelican::vehiclesEnabled;
}
//...................................................................
QSTATE Pelican::vehiclesYellow(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      SetDlgItemText(hWnd_, IDC_STATE, "vehiclesYellow");
      SetTimer(hWnd_, 1, VEHICLES_YELLOW_TOUT, 0);
      signalVehicles(YELLOW);
      return 0;
   case TIMEOUT_SIG:
      Q_TRAN(&Pelican::pedestriansEnabled);
      return 0;
   case Q_EXIT_SIG:
      KillTimer(hWnd_, 1);                   // don't leak the timer!
      return 0;
   }
   return (QSTATE)&Pelican::vehiclesEnabled;
}
//...................................................................
QSTATE Pelican::pedestriansEnabled(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      signalVehicles(RED);
      return 0;
   case Q_INIT_SIG:
      Q_INIT(&Pelican::pedestriansWalk);
      return 0;
   }
   return (QSTATE)&Pelican::operational;
}
//...................................................................
QSTATE Pelican::pedestriansWalk(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      SetDlgItemText(hWnd_, IDC_STATE, "pedestriansWalk");
      signalPedestrians(WALK);
      SetTimer(hWnd_, 1, PEDESTRIANS_WALK_TOUT, 0);
      return 0;
   case TIMEOUT_SIG:
      Q_TRAN(&Pelican::pedestriansFlash);
      return 0;
   case Q_EXIT_SIG:
      KillTimer(hWnd_, 1);                   // don't leak the timer!
      return 0;
   }
   return (QSTATE)&Pelican::pedestriansEnabled;
}
//...................................................................
QSTATE Pelican::pedestriansFlash(QEvent const *e) {
   switch (e->sig) {
   case Q_ENTRY_SIG:
      SetDlgItemText(hWnd_, IDC_STATE, "pedestriansFlash");
      SetTimer(hWnd_, 1, PEDESTRIANS_FLASH_TOUT, 0);
      pedestrianFlashCtr_ = PEDESTRIANS_FLASH_NUM*2 + 1;
      return 0;
   case TIMEOUT_SIG:
      --pedestrianFlashCtr_;
      if (pedestrianFlashCtr_ == 0) {               // done flashing?
         Q_TRAN(&Pelican::vehiclesEnabled);
      }
      else if ((pedestrianFlashCtr_ & 1) == 0) {     // even counter?
         signalPedestrians(DONT_WALK);
      }
      else {                                   // must be odd counter
         signalPedestrians(BLANK);
      }
      return 0;
   case Q_EXIT_SIG:
      KillTimer(hWnd_, 1);                   // don't leak the timer!
      return 0;
   }
   return (QSTATE)&Pelican::pedestriansEnabled;
}
//...................................................................
*/

