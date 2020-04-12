#define TIMER_ENABLE
#include "tkCommon/CmdParser.h"
#include "tkCommon/math/MatIO.h"
#include <thread>
#include <signal.h>

int main( int argc, char** argv){

    tk::common::CmdParser cmd(argv, "test matio");
    std::string matfile = cmd.addArg("matfile", "matlab.mat", "path of the mat file");
    cmd.print();
    
    tk::math::MatIO mat;
    mat.create(matfile);

    std::vector<tk::math::MatIO::var_t> structFields(3);
    int a = 24;
    double b = 2.5564;
    Eigen::MatrixXf c; c.resize(2, 2); c << 1, 2, 3, 4;
    structFields[0].set("a", a);
    structFields[1].set("b", b);
    structFields[2].set("c", c.matrix());

    tk::math::MatIO::var_t structVar;
    structVar.setStruct("lol", structFields);
    structVar.print();

    mat.writeVar(structVar);
    structVar.release();

    /*
    mat.open(matfile);
    mat.stats();
    tk::math::MatIO::var_t var;
    mat.readVar("key_45", var);
    var.print();
    */
    return 0;
}