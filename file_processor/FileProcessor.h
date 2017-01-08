#if !defined(AFX_FILEPROCESSOR_H__B80894FD_3CC5_40AE_9045_B69D21CBD95F__INCLUDED_)
#define AFX_FILEPROCESSOR_H__B80894FD_3CC5_40AE_9045_B69D21CBD95F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/fsm.h"

/*  
     ___________          _______________________  
    |initial    |        |on_normal(print char)  |   
    |           V        |                       V   
------------------default state--------------------------------------------------|
|                                                                                |
|                                             ___________                        |
|                                            |on_space   |                       |
|                                            |           V                       |
|                                    ------------sawSpace------------            |
|                                    |                               |           |
|    ----on_space(print char)-->     |                               |           |
|                                    |                               |           |
|                                    |                               |           |
|                                    --------------------------------            |
|                                                                                |
|                                                 ___________   ___________      |
|                                                |on_space   | |on_newline |     |
|                                                |           V |           V     |
|                                            ------------sawNewLine-----------   |
|                                            |                                |  |
|    ----on_newline(print char)-->           |                                |  |
|                                            |                                |  |
|                                            ---------------------------------|  |
|---------------------------------------------------------------------------------

*/


class FileProcessor; 

//this just declares all possible events for this application, across all states.
//no transition by default
class NO_VTABLE State:public dbabits::IState_base<FileProcessor,State>
{
public:
    virtual State* on_space  (FileProcessor*,const boost::any&){return this;}
    virtual State* on_newline(FileProcessor*,const boost::any&){return this;}
    virtual State* on_normal (FileProcessor*,const boost::any&){return this;}
    virtual State* on_end    (FileProcessor*,const boost::any&){return 0;}
};

class FileProcessor : public dbabits::Fsm<FileProcessor,State >{

public:
    FileProcessor(){transition(&m_default);}

    class defaultState:public State{
        virtual State* on_space(FileProcessor* p,  const boost::any& arg) {std::cout<<boost::any_cast<char>(arg);return &p->m_space;}
        virtual State* on_newline(FileProcessor* p,const boost::any& arg) {std::cout<<boost::any_cast<char>(arg);return &p->m_newline;}
        virtual State* on_normal(FileProcessor* p, const boost::any& arg) {std::cout<<boost::any_cast<char>(arg);return &p->m_default;}
    } m_default;

    class sawSpace:public defaultState{
        virtual State* on_space(FileProcessor* p,const boost::any& arg)   {return this;}
    } m_space;

    class sawNewline:public defaultState{
        virtual State* on_space(FileProcessor* p,const boost::any& arg)   {return this;}
        virtual State* on_newline(FileProcessor* p,const boost::any& arg) {return this;}
    } m_newline;
};

#endif // !defined(AFX_FILEPROCESSOR_H__B80894FD_3CC5_40AE_9045_B69D21CBD95F__INCLUDED_)


