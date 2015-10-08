#ifndef LDPCCODE_H
#define LDPCCODE_H

#include "ldpcmakeldpc.h"
#include "ldpcmakegen.h"
#include "ldpcqconversions.h"
#include <QApplication>

#include <qDebug>

class LDPCCode
{
public:
    typedef enum { OldFormat, NewFormat1 } file_format;
    LDPCCode();
    LDPCCode(QString filename);
    LDPCCode(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method=LDPCMakeGen::Dense, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);
    bool create(int M, int N, int seed, LDPCMakeLDPC::make_method parity_check_make_method, QString distrib, int no4cycle, LDPCMakeGen::make_method generator_make_method=LDPCMakeGen::Dense, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);

    bool savetofile(QString filename, file_format fileformat=NewFormat1);
    bool loadfromfile(QString filename, file_format fileformat=NewFormat1);

    QVector<int> ParityCheckMatrix;
    QVector<int> GeneratorMatrix;

    bool setRandFileName(QString filename);

    bool isEmpty()
    {
        if(ParityCheckMatrix.isEmpty())return true;
        if(GeneratorMatrix.isEmpty())return true;
        return false;
    }

};

#endif // LDPCCODE_H
