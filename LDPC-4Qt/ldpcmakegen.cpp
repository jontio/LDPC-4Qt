#include "ldpcmakegen.h"
#include <QDebug>



LDPCMakeGen::~LDPCMakeGen()
{
    vars.freelocal();
}

LDPCMakeGen::LDPCMakeGen()
{
    //
}

LDPCMakeGen::LDPCMakeGen(QVector<int> &pchk, make_method method, mod2sparse_strategy strategy, int abandon_number,int abandon_when)
{
    create(pchk, method,  strategy, abandon_number,abandon_when);
}

LDPCMakeGen::LDPCMakeGen(QVector<int> &pchk, make_method method, QVector<int> &other_gen)
{
    create(pchk, method, other_gen);
}

void LDPCMakeGen::create(QVector<int> &pchk, make_method method, mod2sparse_strategy strategy, int abandon_number,int abandon_when)
{
    if(pchk.isEmpty())return;
    vars.clearglobal();

    //load parity check matrix
    if(!qconversions.mod2sparse_load_parity_check_from_qvector(pchk))
    {
        vars.freeglobal();
        return;
    }
    if (::N<=::M)
    {
        qDebug("Can't encode if number of bits (%d) isn't greater than number of checks (%d)\n",::N,::M);
        vars.freeglobal();
        return;
    }

    // Allocate space for row and column permutations.
    cols = (int*)chk_alloc(N, sizeof(*cols));
    rows = (int*)chk_alloc(M, sizeof(*rows));

    switch (method)
    { case Sparse:
      {

        make_sparse(GeneratorMatrix,strategy,abandon_number,abandon_when);
        break;
      }
      case Dense: case Mixed:
      {
        make_dense_mixed(GeneratorMatrix,method);
        break;
      }
      default: abort();
    }

    vars.moveglobaltolocal();
    return;
}

void LDPCMakeGen::create(QVector<int> &pchk, make_method method, QVector<int> &other_gen)
{
    if(pchk.isEmpty())return;
    vars.clearglobal();

    mod2sparse_strategy strategy=Mod2sparse_minprod;
    int abandon_number=0;
    int abandon_when=0;

    //load parity check matrix
    if(!qconversions.mod2sparse_load_parity_check_from_qvector(pchk))
    {
        vars.freeglobal();
        return;
    }
    if (::N<=::M)
    {
        qDebug("Can't encode if number of bits (%d) isn't greater than number of checks (%d)\n",::N,::M);
        vars.freeglobal();
        return;
    }

    // Allocate space for row and column permutations.
    cols = (int*)chk_alloc(N, sizeof(*cols));
    rows = (int*)chk_alloc(M, sizeof(*rows));

    switch (method)
    { case Sparse:
      {

        make_sparse(GeneratorMatrix,strategy,abandon_number,abandon_when);
        break;
      }
      case Dense: case Mixed:
      {
        make_dense_mixed(GeneratorMatrix,method,other_gen);
        break;
      }
      default: abort();
    }

    vars.moveglobaltolocal();
    return;
}


/* MAKE DENSE OR MIXED REPRESENTATION OF GENERATOR MATRIX. */

bool LDPCMakeGen::make_dense_mixed(QVector<int> &gen,make_method method)
{
    QVector<int> other_gen;
    return make_dense_mixed(gen,method,other_gen);
}

bool LDPCMakeGen::make_dense_mixed(QVector<int> &gen,make_method method,QVector<int> &other_gen)
{
  mod2dense *DH, *A, *A2, *AI, *B;
  int i, j, c, c2, n;
  int *rows_inv;

  DH=A=A2=AI=B=NULL;
  rows_inv=NULL;

  DH = mod2dense_allocate(M,N);
  AI = mod2dense_allocate(M,M);
  B  = mod2dense_allocate(M,N-M);
  G  = mod2dense_allocate(M,N-M);

  mod2sparse_to_dense(H,DH);

  /* If another generator matrix was specified, invert using the set of
     columns it specifies. */

  if (!other_gen.isEmpty())
  {
    qconversions.load_generator_from_qvector(other_gen,1,0);

    A = mod2dense_allocate(M,M);
    mod2dense_copycols(DH,A,cols);

    if (!mod2dense_invert(A,AI))
    {

        if(DH!=NULL){mod2dense_free(DH);DH=NULL;}
        if(AI!=NULL){mod2dense_free(AI);AI=NULL;}
        if(B!=NULL){mod2dense_free(B);B=NULL;}
        if(A!=NULL){mod2dense_free(A);A=NULL;}
        if(A2!=NULL){mod2dense_free(A2);A2=NULL;}
        if(rows_inv!=NULL){free(rows_inv);rows_inv=NULL;}

        qDebug("Couldn't invert sub-matrix with column order given in other file\n");
        return false;
    }

    mod2dense_copycols(DH,B,cols+M);
  }

  /* If no other generator matrix was specified, invert using whatever
     selection of rows/columns is needed to get a non-singular sub-matrix. */

  if (other_gen.isEmpty())
  {
    A  = mod2dense_allocate(M,N);
    A2 = mod2dense_allocate(M,N);

    n = mod2dense_invert_selected(DH,A2,rows,cols);
    mod2sparse_to_dense(H,DH);  /* DH was destroyed by invert_selected */

    if (n>0)
    {
        qDebug("Note: Parity check matrix has %d redundant checks\n",n);
    }

    rows_inv = (int*)chk_alloc (M, sizeof(*rows_inv));

    for (i = 0; i<M; i++)
    { rows_inv[rows[i]] = i;
    }

    mod2dense_copyrows(A2,A,rows);
    mod2dense_copycols(A,A2,cols);
    mod2dense_copycols(A2,AI,rows_inv);

    mod2dense_copycols(DH,B,cols+M);
  }

  /* Form final generator matrix. */

  if (method==Dense)
  { mod2dense_multiply(AI,B,G);
  }
  else if (method==Mixed)
  {
      if(G!=NULL){mod2dense_free(G);G=NULL;}
      G = AI;
      AI=NULL;
  }
  else
  { abort();
  }

  /* Compute and print number of 1s. */

  if (method==Dense)
  { c = 0;
    for (i = 0; i<M; i++)
    { for (j = 0; j<N-M; j++)
      { c += mod2dense_get(G,i,j);
      }
    }
    qDebug("Number of 1s per check in Inv(A) X B is %.1f", (double)c/M);
  }

  if (method==Mixed)
  { c = 0;
    for (i = 0; i<M; i++)
    { for (j = 0; j<M; j++)
      { c += mod2dense_get(G,i,j);
      }
    }
    c2 = 0;
    for (i = M; i<N; i++)
    { c2 += mod2sparse_count_col(H,cols[i]);
    }
    qDebug("Number of 1s per check in Inv(A) is %.1f, in B is %.1f, total is %.1f",(double)c/M, (double)c2/M, (double)(c+c2)/M);
  }

  /* Write the represention of the generator matrix to the file. */

  if(method==Dense)qconversions.save_generator_to_qvector(gen,LDPCQConversions::Dense);
   else if(method==Mixed)qconversions.save_generator_to_qvector(gen,LDPCQConversions::Mixed);
    else
    {
      if(DH!=NULL){mod2dense_free(DH);DH=NULL;}
      if(AI!=NULL){mod2dense_free(AI);AI=NULL;}
      if(B!=NULL){mod2dense_free(B);B=NULL;}
      if(A!=NULL){mod2dense_free(A);A=NULL;}
      if(A2!=NULL){mod2dense_free(A2);A2=NULL;}
      if(rows_inv!=NULL){free(rows_inv);rows_inv=NULL;}
      return false;
    }

  if(DH!=NULL){mod2dense_free(DH);DH=NULL;}
  if(AI!=NULL){mod2dense_free(AI);AI=NULL;}
  if(B!=NULL){mod2dense_free(B);B=NULL;}
  if(A!=NULL){mod2dense_free(A);A=NULL;}
  if(A2!=NULL){mod2dense_free(A2);A2=NULL;}
  if(rows_inv!=NULL){free(rows_inv);rows_inv=NULL;}

  return true;

}



/* MAKE SPARSE REPRESENTATION OF GENERATOR MATRIX. */

bool LDPCMakeGen::make_sparse(QVector<int> &gen,mod2sparse_strategy strategy,int abandon_number,int abandon_when)
{
  int n, cL, cU, cB;
  int i;

  gen.clear();

  /* Find LU decomposition. */


  ::L = mod2sparse_allocate(::M,::M);
  ::U = mod2sparse_allocate(::M,::N);

  n = mod2sparse_decomp(H,M,L,U,rows,cols,strategy,abandon_number,abandon_when);

  if (n!=0 && abandon_number==0)
  {
      qDebug("Note: Parity check matrix has %d redundant checks\n",n);
  }
  if (n!=0 && abandon_number>0)
  {
      qDebug("Note: Have %d dependent columns, but this could be due to abandonment.\n",n);
      qDebug("      Try again with lower abandonment number.\n");
      return false;
  }

  /* Compute and print number of 1s. */

  cL = cU = cB = 0;

  for (i = 0; i<M; i++) cL += mod2sparse_count_row(L,i);
  for (i = 0; i<M; i++) cU += mod2sparse_count_row(U,i);
  for (i = M; i<N; i++) cB += mod2sparse_count_col(H,cols[i]);

  qDebug("Number of 1s per check in L is %.1f, U is %.1f, B is %.1f, total is %.1f",(double)cU/M, (double)cL/M, (double)cB/M, (double)(cL+cU+cB)/M);

  /* Write it all to the generator matrix vector. */

  qconversions.save_generator_to_qvector(gen,LDPCQConversions::Sparse);

  return true;

}

