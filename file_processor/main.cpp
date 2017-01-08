#include "stdafx.h"
#include "FileProcessor.h"

FileProcessor::Event get_event(boost::any& arg){
    FileProcessor::Event e(0);
    char charRead=EOF;
	switch(charRead=getchar()) {
		case EOF:
            e=State::on_end;
			break;
		case ' ': case '\t':
            e = State::on_space;
			break;
		case '\n':
			e = State::on_newline;
			break;
		default:
            e=State::on_normal;
            break;
    }

    arg=charRead;
    return e;
}


int main(int argc, char* argv[]){
    using namespace std;

	char test_string[]="this is a test of	   \
	\n       the state      \t       machine     \n  \n \
	\n \n   \n\n  \t           called file  \t \t processor \t\t				\
	\n       It should collapse     extra    spaces	\
	\n\n     and tabs.  and remove            extra newlines	\t		\
	\n  \n\t \n\n see     \t  FileProcessor class \t\t \n   \n\t	";

    //create the file
    ofstream fout("test.txt",ios::out);
	fout<<test_string;
	fout.close();
    fout.clear();
	
	cout<<endl<<"_____________________this is the file test.txt:____________________________________________________"<<endl<<endl;

    //display the file
    system("type test.txt");
		
	cout<<endl<<"_____________________now we'll run our state machine on it:________________________________________"<<endl<<endl;

    //Reassign stdin to test.txt
    freopen( "test.txt", "r", stdin );


    FileProcessor a;

    for(;;){
        boost::any arg;
        FileProcessor::Event e=get_event(arg);
        State* next_state=0;
        try{
          next_state=a.dispatch(e,arg);
        }
        catch(const boost::bad_any_cast& e){
          cout<<"\nmain::exception:"<<e.what();
        }
        if(0==next_state) break;
    }
    cout<<"\n\n\nmain::goodbye";
    return 0;
}