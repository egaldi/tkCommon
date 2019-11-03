#include <tkCommon/terminalFormat.h>
#include <iostream>
#include <cxxabi.h>

namespace tk{
class tkClass{
    public:
        void error(){
            clsErr("error message\n");
        }
        void success(){
            clsSuc("success message\n");
        }
        void warning(){
            clsWrn("warning message\n");
        }
        void message(){
            clsMsg("clasic message. Each type message print reading the terminal column in a way that a single print remain at left of class name. This permits to have a clean print. I don't konw what'else to write for have a long phrase for demostrating the work. Bye Bye\n");
        }
};
}


int main(int argc, char* argv[]){

    std::cout<<"this is how "
             <<tk::tformat::print("colorful", tk::tformat::lightRed, tk::tformat::predefined, tk::tformat::underlined)
             <<" is "
             <<tk::tformat::print("tk:\n\n", tk::tformat::lightYellow, tk::tformat::predefined, tk::tformat::bold);

    //Print usage
    for(uint8_t i=tk::tformat::white; i<=tk::tformat::lightGray; i++) {
        std::cout<<tk::tformat::print("Color code:  ", i);
        std::cout<<tk::tformat::print(std::to_string(i)+"\n", i, tk::tformat::predefined, tk::tformat::reverse);
    }

    //Set unset usage
    std::cout<<"\nUsing set/unset:\n";

    std::cout<<tk::tformat::set(tk::tformat::yellow);
    std::cout<<"This is tk"<<tk::tformat::unset()<<"\n";

    std::cout<<tk::tformat::set(tk::tformat::yellow,tk::tformat::red);
    std::cout<<"This is tk"<<tk::tformat::unset()<<"\n";

    std::cout<<tk::tformat::set(tk::tformat::predefined,tk::tformat::red);
    std::cout<<"This is tk"<<tk::tformat::unset()<<"\n";

    std::cout<<tk::tformat::set(tk::tformat::predefined,tk::tformat::red,tk::tformat::bold);
    std::cout<<"This is tk"<<tk::tformat::unset()<<"\n";

    std::cout<<tk::tformat::set(tk::tformat::predefined,tk::tformat::red,tk::tformat::reverse);
    std::cout<<"This is tk"<<tk::tformat::unset()<<"\n\n";


    //Class printing
    tk::tkClass a;
    a.error();
    a.message();
    a.success();
    a.warning();

    return  0;
}