#ifndef LDPCMAKEGEN_H
#define LDPCMAKEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/intio.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/rcode.h"
#include "rcodevars.h"

#include "ldpcqconversions.h"


class LDPCMakeGen
{
public:
    typedef enum { Sparse, Dense, Mixed } make_method;      /* Ways of making it */
    LDPCMakeGen();
    LDPCMakeGen(QVector<int> &pchk, make_method method, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);
    LDPCMakeGen(QVector<int> &pchk, make_method method, QVector<int> &other_gen);
    void create(QVector<int> &pchk, make_method method, mod2sparse_strategy strategy = Mod2sparse_minprod, int abandon_number=0, int abandon_when=0);
    void create(QVector<int> &pchk, make_method method, QVector<int> &other_gen);
    ~LDPCMakeGen();
    QVector<int> GeneratorMatrix;
    RCodeVars vars;//stores stuff from rcode.c/rcode.h
private:
    LDPCQConversions qconversions;
    bool make_sparse(QVector<int> &gen, mod2sparse_strategy strategy, int abandon_number, int abandon_when);
    bool make_dense_mixed(QVector<int> &gen, make_method method, QVector<int> &other_gen);
    bool make_dense_mixed(QVector<int> &gen, make_method method);
};

#endif // LDPCMAKEGEN_H
