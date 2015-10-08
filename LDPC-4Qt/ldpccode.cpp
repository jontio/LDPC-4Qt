#include "ldpccode.h"

LDPCCode::LDPCCode()
{
    //
}

LDPCCode::LDPCCode(QString filename)
{
    loadfromfile(filename);
}

LDPCCode::LDPCCode(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method, mod2sparse_strategy strategy, int abandon_number, int abandon_when)
{
    create(M, N, seed, parity_check_make_method, distrib, no4cycle, generator_make_method, strategy, abandon_number, abandon_when);
}

bool LDPCCode::create(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method, mod2sparse_strategy strategy, int abandon_number, int abandon_when)
{
    ParityCheckMatrix.clear();
    GeneratorMatrix.clear();

    LDPCMakeLDPC makeldpc(M, N, seed, parity_check_make_method, distrib, no4cycle);
    LDPCMakeGen makegen(makeldpc.ParityCheckMatrix, generator_make_method, strategy, abandon_number, abandon_when);

    if(makeldpc.ParityCheckMatrix.isEmpty())
    {
        qDebug()<<"failed to create parity check matrix";
        return false;
    }
    if(makegen.GeneratorMatrix.isEmpty())
    {
        qDebug()<<"failed to create generator";
        return false;
    }

    ParityCheckMatrix=makeldpc.ParityCheckMatrix;
    GeneratorMatrix=makegen.GeneratorMatrix;

    return true;
}

bool LDPCCode::savetofile(QString filename, file_format fileformat)
{
    bool good=false;
    switch(fileformat)
    {
    case NewFormat1:
        good=true;
        if(!LDPCQConversions::write_qvector_to_file(filename+".pchk",ParityCheckMatrix))good=false;
        if(!LDPCQConversions::write_qvector_to_file(filename+".gen",GeneratorMatrix))good=false;
        break;
    case OldFormat:
        good=true;
        if(!LDPCQConversions::write_qvector_to_file_old_format(filename+".pchk",ParityCheckMatrix))good=false;
        if(!LDPCQConversions::write_qvector_to_file_old_format(filename+".gen",GeneratorMatrix))good=false;
        break;
    default:
        qDebug()<<"Unkown file format to save in";
    }
    if(!good)qDebug()<<"Failed to save all files";
     else qDebug()<<"LDPC code saved to disk";
    return good;
}

bool LDPCCode::loadfromfile(QString filename, file_format fileformat)
{
    bool good=false;
    switch(fileformat)
    {
    case NewFormat1:
        good=true;
        if(!LDPCQConversions::read_qvector_to_file(filename+".pchk",ParityCheckMatrix))good=false;
        if(!LDPCQConversions::read_qvector_to_file(filename+".gen",GeneratorMatrix))good=false;
        break;
    case OldFormat:
        qDebug()<<"Old format not implimented";
        break;
        good=true;
        if(!LDPCQConversions::read_qvector_to_file_old_format(filename+".pchk",ParityCheckMatrix))good=false;
        if(!LDPCQConversions::read_qvector_to_file_old_format(filename+".gen",GeneratorMatrix))good=false;
        break;
    default:
        qDebug()<<"Unkown file format to load in";
    }
    if(!good)qDebug()<<"Failed to load all files";
     else qDebug()<<"LDPC code loaded from disk";
    return good;
}

