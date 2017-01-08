#ifndef calc_h
#define calc_h

#include "../include/fsm.h"
#include "../include/announcer.h"

class Calc;
class _declspec(novtable) State:public dbabits::IState_base<Calc,State>
{
public:
    virtual State* on_oper(Calc*,const boost::any&)   {return this;}
    virtual State* on_point(Calc*,const boost::any&)  {return this;}
    virtual State* on_0(Calc*,const boost::any&)      {return this;}
    virtual State* on_1_9(Calc*,const boost::any&)    {return this;}
    virtual State* on_CE(Calc*,const boost::any&)     {return this;}
    virtual State* on_equals(Calc*,const boost::any&) {return this;}
    virtual State* on_C(Calc*,const boost::any&)      {return this;}
    virtual State* on_term(Calc*,const boost::any&)   {return this;}
};


class Calc : public dbabits::Fsm<Calc,State> {

public:
    static Calc *instance(HWND hwnd=0){static Calc app(hwnd); return &app;}           
    static BOOL CALLBACK calcDlg(HWND hwnd, UINT iEvt, WPARAM wParam, LPARAM lParam);
protected:
    Calc(HWND);

public:

    class calc:public State{
    protected:
        virtual void on_enter(Calc*,State* old_state ); 
    private:
        virtual State* on_term(Calc*,const boost::any&);
        virtual State* on_C(Calc*,const boost::any&);

    } m_calc;
    class ready:public calc{
        virtual State* on_oper(Calc*,const boost::any&);   
        virtual State* on_point(Calc*,const boost::any&);  
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
    } m_ready;

    class result:public ready{
    protected:
        virtual void on_enter(Calc*,State* old_state  ); 
    } m_result;

    class begin:public ready{
        virtual State* on_oper(Calc*,const boost::any&);
    } m_begin;

    class negated1:public calc{
    protected:
        virtual void on_enter(Calc*,State* old_state ); 
        virtual void on_exit(Calc*,State* new_state ); 

    private:
        virtual State* on_CE(Calc*,const boost::any&);
        virtual State* on_point(Calc*,const boost::any&);  
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
    } m_negated1;

    class operand1:public calc{
        virtual State* on_oper(Calc*,const boost::any&);
        virtual State* on_CE(Calc*,const boost::any&);
    } m_operand1;

    class zero1:public operand1{
        virtual State* on_point(Calc*,const boost::any&);          
        virtual State* on_1_9(Calc*,const boost::any&);    
    } m_zero1;

    class int1:public operand1{
        virtual State* on_point(Calc*,const boost::any&);                  
        virtual State* on_1_9(Calc*,const boost::any&);    
        virtual State* on_0(Calc*,const boost::any&);      
    } m_int1;

    class frac1:public operand1{
        virtual State* on_1_9(Calc*,const boost::any&);    
        virtual State* on_0(Calc*,const boost::any&);      
    } m_frac1;
    
    class opEntered:public calc{
        virtual State* on_point(Calc*,const boost::any&);  
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
        virtual State* on_oper(Calc*,const boost::any&);        
        virtual State* on_equals(Calc*,const boost::any&);
    } m_opEntered;

    class negated2:public calc{
    protected:
        virtual void on_enter(Calc*,State* old_state ); 
    private:

        virtual State* on_point(Calc*,const boost::any&);  
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
        virtual State* on_CE(Calc*,const boost::any&);
        
    } m_negated2;

    class operand2:public calc{
        virtual State* on_equals(Calc*,const boost::any&);
        virtual State* on_oper(Calc*,const boost::any&);        
        virtual State* on_CE(Calc*,const boost::any&);

    }m_operand2;
    
    class zero2:public operand2{
        virtual State* on_point(Calc*,const boost::any&);  
        virtual State* on_1_9(Calc*,const boost::any&);    
        
    }m_zero2;

    class int2:public operand2{
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
        virtual State* on_point(Calc*,const boost::any&);  
    }m_int2;

    class frac2:public operand2{
        virtual State* on_0(Calc*,const boost::any&);      
        virtual State* on_1_9(Calc*,const boost::any&);    
    } m_frac2;


   void clear();
   void insert(int keyId);
   void eval();
   void negate();
   void dispState(char const *s);
   void read(double *);

   double m_Operand1;
   double m_Operand2;
   int    m_Operator;
private:
   HWND m_hwnd;                       

};

#endif                                
