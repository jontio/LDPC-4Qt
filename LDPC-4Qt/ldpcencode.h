#ifndef LDPCENCODE_H
#define LDPCENCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../LDPC-codes/rand.h"
#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/blockio.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/rcode.h"
#include "../LDPC-codes/enc.h"

#include <QByteArray>
#include <QVector>
#include <QDebug>

#include "ldpcmakegen.h"
#include "rcodevars.h"
#include "ldpcqconversions.h"
#include "ldpccode.h"


class LDPCEncode
{
public:
    LDPCEncode(QVector<int> &ParityCheckMatrix,QVector<int> &GeneratorMatrix);
    LDPCEncode(QVector<int> &ParityCheckMatrix);
    LDPCEncode(LDPCCode &ldpccode);
    LDPCEncode();
    ~LDPCEncode();

    QByteArray createrandomdata(int blocks)
    {
        QByteArray src;
        for(int i=0;i<blocks;i++){for(int i=0;i<getblocksizein();i++)src.push_back(rand()%2);}
        return src;
    }

    void createrandomdata(int blocks, QByteArray &to)
    {
        to.clear();
        for(int i=0;i<blocks;i++){for(int i=0;i<getblocksizein();i++)to.push_back(rand()%2);}
    }

    bool encode(QByteArray &from, QByteArray &to);
    bool encodeblock(QByteArray &from, QByteArray &to);
    bool init(LDPCCode &ldpccode);
    bool init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix);
    bool init(QVector<int> &ParityCheckMatrix);
    int getblocksizein(){return (vars.N-vars.M);}
    int getblocksizeout(){return (vars.N);}
private:
    RCodeVars vars;//stores stuff from rcode.c/rcode.h
    RCodeVars tmpvars;//stores stuff from rcode.c/rcode.h
    LDPCQConversions qconversions;
    mod2dense *u, *v;
    QByteArray chks;
    QByteArray tmpin,tmpout,remainder;
    int blocksizein;
};

#endif // LDPCENCODE_H
