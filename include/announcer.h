#ifndef DMITRY_ANNOUNCER_H_2_25_1997
#define DMITRY_ANNOUNCER_H_2_25_1997
#pragma once
	
/*derive from this-will show somth. like: class Session2 +
  can also use typeid(*this)-> produces struct dmitry::announcer<class Session2> +
*/
#include <typeinfo>
#pragma warning(push,4)

template<class Derived> 
struct announcer {
    announcer() {echo("+");}
    ~announcer() {echo("~");}
    announcer<Derived>(const announcer<Derived>& rhs){echo("copy+",rhs);}
    announcer<Derived>& operator=(const announcer<Derived>& rhs){echo("operator=",rhs); return *this;}
protected:
    void echo(const TCHAR* s) const{printf("\n%hs %s(this=0x%x)",typeid(Derived).name(),s,this);}
    void echo(const TCHAR* s,const announcer<Derived>& rhs) const{
        printf(_T("\n%hs %s(this=0x%x, rhs=0x%x)"),typeid(Derived).name(),s,this,&rhs);
    }
};

#pragma warning(pop)

#endif DMITRY_ANNOUNCER_H_2_25_1997