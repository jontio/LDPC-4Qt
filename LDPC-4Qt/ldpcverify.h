#ifndef LDPCVERIFY_H
#define LDPCVERIFY_H

#include <QByteArray>
#include <QVector>
#include <QDebug>
#include "ldpcchannel.h"
#include "ldpcdecodingmethod.h"
#include "ldpcmakegen.h"
#include "rcodevars.h"
#include "ldpcqconversions.h"
#include "../LDPC-codes/check.h"
#include "../LDPC-codes/dec.h"
#include "ldpccode.h"
#include <math.h>
#include <limits>

class LDPCVerify
{
public:
    LDPCVerify();
    LDPCVerify(LDPCCode &ldpccode, int _table=0);
    LDPCVerify(QVector<int> &ParityCheckMatrix, int _table=0);
    LDPCVerify(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, int _table=0);
    ~LDPCVerify();
    bool init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, int table=0);//table shows information via printf. valid numbers are 0 1 and 2. 0 for no information
    bool init(QVector<int> &ParityCheckMatrix, int table=0);//table shows information via printf. valid numbers are 0 1 and 2. 0 for no information
    bool init(LDPCCode &ldpccode, int table=0);
    bool verify(QByteArray &from,QByteArray &to);
    double getber();
    QString message;
    void resetstats();
private:
    LDPCQConversions qconversions;
    RCodeVars vars;//stores stuff from rcode.c/rcode.h
    RCodeVars tmpvars;//stores stuff from rcode.c/rcode.h
    int table;
    QByteArray sblk;
    QByteArray cblk;
    QByteArray chks;
    int bit_errs;
    int tot_srcerrs, tot_chkerrs, tot_botherrs;
    double ber;
};

#endif // LDPCVERIFY_H
