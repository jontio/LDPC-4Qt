#ifndef RCODEVARS_H
#define RCODEVARS_H

#include "../LDPC-codes/alloc.h"
#include "../LDPC-codes/intio.h"
#include "../LDPC-codes/open.h"
#include "../LDPC-codes/mod2sparse.h"
#include "../LDPC-codes/mod2dense.h"
#include "../LDPC-codes/mod2convert.h"
#include "../LDPC-codes/rcode.h"


struct RCodeVars
{
    mod2sparse *H;		/* Parity check matrix */

    int M;			/* Number of rows in parity check matrix */
    int N;			/* Number of columns in parity check matrix */

    char type;		/* Type of generator matrix representation (s/d/m) */
    int *cols;		/* Ordering of columns in generator matrix */

    mod2sparse *L;
    mod2sparse *U;	/* Sparse LU decomposition, if type=='s' */
    int *rows;		/* Ordering of rows in generator matrix (type 's') */

    mod2dense *G;		/* Dense or mixed representation of generator matrix,
                   if type=='d' or type=='m' */

    RCodeVars()
    {
        clearlocal();
    }

    void clearlocal()
    {
        cols=NULL;
        G=NULL;
        H=NULL;
        L=NULL;
        M=0;
        N=0;
        rows=NULL;
        type='s';
        U=NULL;
    }

    void clearglobal()
    {
        ::cols=NULL;
        ::G=NULL;
        ::H=NULL;
        ::L=NULL;
        ::M=0;
        ::N=0;
        ::rows=NULL;
        ::type='s';
        ::U=NULL;
    }

    void freelocal()
    {
        if(cols!=NULL)free(cols);
        if(G!=NULL)mod2dense_free(G);
        if(H!=NULL)mod2sparse_free(H);
        if(L!=NULL)mod2sparse_free(L);
        if(rows!=NULL)free(rows);
        if(U!=NULL)mod2sparse_free(U);
        clearlocal();
    }

    void freeglobal()//carefull
    {
        if(::cols!=NULL)free(::cols);
        if(::G!=NULL)mod2dense_free(::G);
        if(::H!=NULL)mod2sparse_free(::H);
        if(::L!=NULL)mod2sparse_free(::L);
        if(::rows!=NULL)free(::rows);
        if(::U!=NULL)mod2sparse_free(::U);
        clearglobal();
    }

    void copylocaltoglobal()
    {
        ::cols=cols;
        ::G=G;
        ::H=H;
        ::L=L;
        ::M=M;
        ::N=N;
        ::rows=rows;
        ::type=type;
        ::U=U;
    }

    void copyglobaltolocal()
    {
        cols=::cols;
        G=::G;
        H=::H;
        L=::L;
        M=::M;
        N=::N;
        rows=::rows;
        type=::type;
        U=::U;
    }

    void movelocaltoglobal()
    {
        copylocaltoglobal();
        clearlocal();
    }

    void moveglobaltolocal()
    {
        copyglobaltolocal();
        clearglobal();
    }

};

#endif // RCODEVARS_H
