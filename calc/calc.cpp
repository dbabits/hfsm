#include "stdafx.h"
#include "calc.h"
#include "resource.h"


void Calc::dispState(const char* s) { SetDlgItemText(m_hwnd, IDC_STATE, s);}

void Calc::clear() { SetDlgItemText(m_hwnd, IDC_DISPLAY, "0");}

void Calc::read(double* pOperand){
    char sz[40]={0};
    GetDlgItemText(m_hwnd,IDC_DISPLAY,sz,sizeof(sz)-1);
    sscanf(sz, "%lf", pOperand);
}

void Calc::insert(int keyId) {
   
   char sz[40]={0};
   GetDlgItemText(m_hwnd,IDC_DISPLAY,sz,sizeof(sz)-2);

   char c=(keyId == IDC_POINT) ? '.'
         :(keyId == IDC_MINUS) ? '-'
         :keyId - IDC_0 + '0';

   strncat(sz,(char*)&c,1);
   
   char* p=(sz[0]=='0' && sz[1] !='.')? &sz[1]:&sz[0];//skip passed leading 0

   SetDlgItemText(m_hwnd, IDC_DISPLAY, p);
}


Calc::Calc(HWND hwnd):m_Operator(-1),m_Operand1(-1),m_Operand2(-1),m_hwnd(hwnd)
{
    clear();
    transition(&m_begin);
}


void Calc::eval() {
   double r;
   switch (m_Operator) {
    case IDC_PLUS:
        r = m_Operand1 + m_Operand2;
        break;
    case IDC_MINUS:
        r = m_Operand1 - m_Operand2;
        break;
    case IDC_MULT:
        r = m_Operand1 * m_Operand2;
        break;
    case IDC_DIVIDE:
        if (m_Operand2 != 0.0) {
            r = m_Operand1 / m_Operand2;
        }
        else {
            MessageBox(m_hwnd, "Cannot Divide by 0", "Calculator", MB_OK);
            r = 0.0;
        } 
        break;
    default:
        MessageBox(m_hwnd, "unknown operator!!!", "Calculator", MB_OK);
   }

   char sz[40]={0};
   if (-1.0e10 < r && r < 1.0e10) {		
       _snprintf(sz,sizeof(sz)-1,"%24.11g",r);
   }
   else {
      MessageBox(m_hwnd, "Result out of range", "Calculator", MB_OK);
      clear();
   }
   SetDlgItemText(m_hwnd, IDC_DISPLAY, sz);
}

void Calc::calc::on_enter(Calc* p,State* old_state){
    p->dispState(typeid(*this).name());
}


void Calc::negated1::on_exit(Calc* p,State* new_state){
    if (new_state && new_state->derives_from(this))  return;
    
    calc::on_exit(p,new_state);
}


 void Calc::negated1::on_enter(Calc* p ,State* old_state){
   #if _MSC_VER > 1200
    //if (old_state->derives_from<Calc::negated1>()) return;//this only works in VS 7
    if (old_state && old_state->derives_from(this)) return;
   #else
    if (dbabits::derives_from<Calc::negated1,Calc>(old_state)) return;
   #endif


    calc::on_enter(p,old_state);
    p->insert(IDC_MINUS);
}
void Calc::negated2::on_enter(Calc* p,State* old_state){
    if (old_state && old_state->derives_from(this)) return;
    calc::on_enter(p,old_state);
    p->clear();
    p->insert(IDC_MINUS);
}   
void Calc::result::on_enter(Calc* p,State* old_state){
    if (old_state && old_state->derives_from(this)) return;
    ready::on_enter(p,old_state);
    p->eval();
}


State* Calc::calc::on_term(Calc* p,const boost::any&){
    return 0;
}

State* Calc::calc::on_C(Calc* p,const boost::any&){
    p->clear();
    return &p->m_begin;
}


State* Calc::ready::on_oper(Calc* p,const boost::any& arg){
    p->m_Operator=boost::any_cast<int>(arg);

    p->read(&p->m_Operand1);

    return &p->m_opEntered;
}

State* Calc::ready::on_0(Calc* p,const boost::any& arg){
    p->clear();
    return &p->m_zero1;
}

State* Calc::ready::on_1_9(Calc* p,const boost::any& arg){
    p->clear();
    p->insert(boost::any_cast<int>(arg));
    return &p->m_int1;
}

State* Calc::ready::on_point(Calc* p,const boost::any& arg){
    p->clear();
    p->insert(IDC_0);
    p->insert(boost::any_cast<int>(arg));

    return &p->m_frac1;
}

State* Calc::begin::on_oper(Calc* p,const boost::any& arg){
    int key=boost::any_cast<int>(arg);

    if (key==IDC_MINUS) return &p->m_negated1;
    else return this;
}

State* Calc::negated1::on_CE(Calc* p,const boost::any& arg){
    p->clear();
    return &p->m_begin;
}

State* Calc::negated1::on_0(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_zero1;
}
State* Calc::negated1::on_1_9(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_int1;
}

State* Calc::negated1::on_point(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_frac1;
}

State* Calc::operand1::on_oper(Calc* p,const boost::any& arg){
    p->read(&p->m_Operand1);

    p->m_Operator = boost::any_cast<int>(arg); //note the operator

    return &p->m_opEntered;
}

State* Calc::operand1::on_CE(Calc* p,const boost::any& arg){
    p->clear();
    return &p->m_begin;
}

State* Calc::zero1::on_point(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_frac1;
}
State* Calc::zero1::on_1_9(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_int1;
}

State* Calc::int1::on_0(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return this;
}

State* Calc::int1::on_1_9(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return this;
}

State* Calc::int1::on_point(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return &p->m_frac1;
}

State* Calc::frac1::on_0(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return this;
}

State* Calc::frac1::on_1_9(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg));
    return this;
}

State* Calc::opEntered::on_oper(Calc* p,const boost::any& arg){
    int key=boost::any_cast<int>(arg);

    if (key==IDC_MINUS) {
        return &p->m_negated2;
    }
    else{ 
        p->m_Operator = boost::any_cast<int>(arg); //note the operator in case u click on a different one. change
        return this;
    }
}
State* Calc::opEntered::on_0(Calc* p,const boost::any& arg){
    p->clear();
    return &p->m_zero2;
}

State* Calc::opEntered::on_1_9(Calc* p,const boost::any& arg){
    p->clear();
    p->insert(boost::any_cast<int>(arg));
    return &p->m_int2;
}


State* Calc::opEntered::on_point(Calc* p,const boost::any& arg){
    p->clear();
    p->insert(IDC_0);
    p->insert(boost::any_cast<int>(arg));
    return &p->m_frac2;
}


State* Calc::operand2::on_CE(Calc* p,const boost::any&){
    p->clear();
    return &p->m_opEntered;
}

State* Calc::operand2::on_oper(Calc* p,const boost::any& arg){
    p->read(&p->m_Operand2);

    p->eval();

    //use result as operand1
    p->read(&p->m_Operand1);

    p->m_Operator = boost::any_cast<int>(arg);
    return &p->m_opEntered;
}

State* Calc::operand2::on_equals(Calc* p,const boost::any&){

    p->read(&p->m_Operand2);

    return &p->m_result;
}

State* Calc::opEntered::on_equals(Calc* p,const boost::any&){

    p->read(&p->m_Operand2);

    return &p->m_result;
}


State* Calc::negated2::on_CE(Calc* p,const boost::any&){
    p->clear();//added
    return &p->m_opEntered;
}

State* Calc::negated2::on_0(Calc* p,const boost::any&){
    return &p->m_zero2;
}

State* Calc::negated2::on_1_9(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg)); 
    return &p->m_int2;
}

State* Calc::negated2::on_point(Calc* p,const boost::any& arg){
    p->insert(boost::any_cast<int>(arg)); 
    return &p->m_frac2;
}



State* Calc::zero2::on_point(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return &p->m_frac2;

}
State* Calc::zero2::on_1_9(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return &p->m_int2;
}



State* Calc::int2::on_point(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return &p->m_frac2;
}

State* Calc::int2::on_0(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return this;
}

State* Calc::int2::on_1_9(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return this;
}

State* Calc::frac2::on_0(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return this;
}

State* Calc::frac2::on_1_9(Calc* p,const boost::any& arg){
      p->insert(boost::any_cast<int>(arg)); 
      return this;
}


BOOL CALLBACK Calc::calcDlg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
   Calc::Event e(0);

   boost::any arg;
   switch (iMsg) {
       case WM_INITDIALOG:
          Calc::instance(hwnd);        
          SendMessage(hwnd, WM_SETICON, (WPARAM)TRUE,(LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1)));
          break;
       case WM_COMMAND:
          switch (LOWORD(wParam)) {
              case IDCANCEL:
                 e =State::on_term;
                 break;
              case IDC_POINT:
                  e =State::on_point;
                  break;
              case IDC_C:
                  e =State::on_C;
                  break;
              case IDC_CE:
                  e =State::on_CE;
                  break;
              case IDC_EQUALS:
                  e =State::on_equals;
                  break;
              case IDC_0:
                  e =State::on_0;
                  break;
              case IDC_1:case IDC_2:case IDC_3:case IDC_4:
              case IDC_5:case IDC_6:case IDC_7:case IDC_8:
              case IDC_9:
                 e =State::on_1_9;
                 break;
              case IDC_PLUS:case IDC_MINUS:case IDC_MULT:case IDC_DIVIDE:
                 e =State::on_oper;
                 break;
              default:
                 break; 
          }
          arg=(int)LOWORD(wParam);
          break;
   }

    try{
        if(e && 0==Calc::instance()->dispatch(e,arg))
            EndDialog(Calc::instance()->m_hwnd, 0);
    }
    catch(const boost::bad_any_cast& e){
        MessageBox(Calc::instance()->m_hwnd, e.what(), "boost::bad_any_cast", MB_OK);
    }

   return FALSE;
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,PSTR cmdLine, int iCmdShow){
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG), 0, Calc::calcDlg);                  
}



