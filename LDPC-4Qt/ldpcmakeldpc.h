#ifndef LDPCMAKELDPC_H
#define LDPCMAKELDPC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../LDPC-codes/rand.h"
#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/intio.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/rcode.h"
#include "../LDPC-codes/distrib.h"

#include "rcodevars.h"

#include <QString>
#include "ldpcqconversions.h"

/*typedef struct		// Representation of a sparse matrix
{
  int n_rows;		  // Number of rows in the matrix
  int n_cols;		  // Number of columns in the matrix

  QVector<mod2entry> rows;	  // Pointer to array of row headers
  QVector<mod2entry> cols;	  // Pointer to array of column headers

  QVector<mod2block> blocks;	  // Blocks that have been allocated ??? whats this ???

} Mod2Sparse;*/



class LDPCMakeLDPC
{
public:

    typedef enum
    { Evencol, 	/* Uniform number of bits per column, with number specified */
      Evenboth 	/* Uniform (as possible) over both columns and rows */
    } make_method;

    LDPCMakeLDPC();
    LDPCMakeLDPC(int M, int N, int seed, make_method method, distrib *d, int no4cycle);
    LDPCMakeLDPC(int M, int N, int seed, make_method method, QString distrib, int no4cycle);

    void create(int M, int N, int seed, make_method method, distrib *d, int no4cycle);
    void create(int M, int N, int seed, make_method method, QString distrib, int no4cycle);

    ~LDPCMakeLDPC();
    QVector<int> ParityCheckMatrix;
    QString ErrorMsg;
    RCodeVars vars;//stores stuff from rcode.c/rcode.h
private:
    int *column_partition (distrib *d,int n);
    bool _makeldpc(int seed, make_method method, distrib *d, int no4cycle);
    LDPCQConversions qconversions;
};

#endif // LDPCMAKELDPC_H
