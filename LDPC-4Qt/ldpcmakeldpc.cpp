#include "ldpcmakeldpc.h"
#include <QDebug>

#include <iostream>

LDPCMakeLDPC::~LDPCMakeLDPC()
{
    vars.freelocal();
}

LDPCMakeLDPC::LDPCMakeLDPC()
{
    //
}

LDPCMakeLDPC::LDPCMakeLDPC(int M, int N, int seed, make_method method, QString _distrib, int no4cycle)
{
    create(M, N, seed, method, _distrib, no4cycle);
}

LDPCMakeLDPC::LDPCMakeLDPC(int M, int N, int seed, make_method method, distrib *d, int no4cycle)
{
    create(M, N, seed, method, d, no4cycle);
}

/**
 * @brief LDPCMakeLDPC::create
 * CREATE A SPARSE PARITY-CHECK MATRIX. Of size innumber by outnumber, stored in ParityCheckMatrix in a vector form.
 * @param M
 * Number of Rows
 * @param N
 * Number of Columns
 * @param seed
 * Random number seed
 * @param method
 * How to make matrix.
 * Either "Evencol" a uniform number of bits per column, with number specified.
 * Or "Evenboth" uniform (as possible) over both columns and rows
 * @param distrib
 * Distribution list specified in text format
 * @param no4cycle
 * Eliminate cycles of length four?
 */
void LDPCMakeLDPC::create(int M, int N, int seed, make_method method, QString _distrib, int no4cycle)
{
    ::M=M;
    ::N=N;
    distrib *d = distrib_create(_distrib.toLatin1().data());
    if(d!=NULL)
    {
        _makeldpc(seed,method,d,no4cycle);
        qconversions.mod2sparse_save_parity_check_to_qvector(ParityCheckMatrix);
        distrib_free(d);
        vars.copyglobaltolocal();
    }
     else qDebug()<<"LDPC parity check matrix creation failure due to distribution parseing error";
}

/**
 * @brief LDPCMakeLDPC::create
 * CREATE A SPARSE PARITY-CHECK MATRIX. Of size innumber by outnumber, stored in ParityCheckMatrix in a vector form.
 * @param M
 * Number of Rows
 * @param N
 * Number of Columns
 * @param seed
 * Random number seed
 * @param method
 * How to make matrix.
 * Either "Evencol" a uniform number of bits per column, with number specified.
 * Or "Evenboth" uniform (as possible) over both columns and rows
 * @param d
 * Distribution list specified
 * @param no4cyclev
 * Eliminate cycles of length four?
 */
void LDPCMakeLDPC::create(int M, int N, int seed, make_method method, distrib *d, int no4cycle)
{
    if(d==NULL)
    {
        qDebug()<<"LDPC parity check matrix creation failure due to NULL distribution";
        return;
    }
    ::M=M;
    ::N=N;
    _makeldpc(seed,method,d,no4cycle);
    qconversions.mod2sparse_save_parity_check_to_qvector(ParityCheckMatrix);
    vars.copyglobaltolocal();
}

bool LDPCMakeLDPC::_makeldpc(int seed, make_method method, distrib *d, int no4cycle)
{

    ErrorMsg.clear();

    // Check for some problems.

    if(d==NULL)
    {
        ErrorMsg="NULL distribution pointer";
        qDebug()<<ErrorMsg;
        return false;
    }

    if (distrib_max(d)>::M)
    {
        ErrorMsg=((QString)"At least one checks per bit (%1) is greater than total checks (%2)").arg(distrib_max(d)).arg(::M);
        qDebug()<<ErrorMsg;
        return false;
    }

    if (distrib_max(d)==M && N>1 && no4cycle)
    {
        ErrorMsg="Can't eliminate cycles of length four with this many checks per bit";
        qDebug()<<ErrorMsg;
        return false;
    }

    mod2entry *e, *f, *g, *h;
    int added, uneven, elim4, all_even, n_full, left;
    int i, j, k, t, z, cb_N;
    int *part=NULL;
    int *u=NULL;

    rand_seed(10*seed+1);

    ::H = mod2sparse_allocate(::M,::N);
    part = column_partition(d,::N);

    // Create the initial version of the parity check matrix.

    switch (method)
    {
      case Evencol:
      {
        z = 0;
        left = part[z];

        for (j = 0; j<N; j++)
        { while (left==0)
          { z += 1;
            if (z>distrib_size(d))
            { abort();
            }
            left = part[z];
          }
          for (k = 0; k<distrib_num(d,z); k++)
          { do
            { i = rand_int(M);
            } while (mod2sparse_find(H,i,j));
            mod2sparse_insert(H,i,j);
          }
          left -= 1;
        }

        break;
      }

      case Evenboth:
      {
        cb_N = 0;
        for (z = 0; z<distrib_size(d); z++)
        { cb_N += distrib_num(d,z) * part[z];
        }

        u = (int*)chk_alloc (cb_N, sizeof(*u));

        for (k = cb_N-1; k>=0; k--)
        { u[k] = k%M;
        }

        uneven = 0;
        t = 0;
        z = 0;
        left = part[z];

        for (j = 0; j<N; j++)
        {
          while (left==0)
          { z += 1;
            if (z>distrib_size(d))
            { abort();
            }
            left = part[z];
          }

      for (k = 0; k<distrib_num(d,z); k++)
          {
            for (i = t; i<cb_N && mod2sparse_find(H,u[i],j); i++) ;

            if (i==cb_N)
            { uneven += 1;
              do
              { i = rand_int(M);
              } while (mod2sparse_find(H,i,j));
              mod2sparse_insert(H,i,j);
            }
            else
            { do
              { i = t + rand_int(cb_N-t);
              } while (mod2sparse_find(H,u[i],j));
              mod2sparse_insert(H,u[i],j);
              u[i] = u[t];
              t += 1;
            }
          }

          left -= 1;
        }

        if (uneven>0)qDebug("Had to place %d checks in rows unevenly",uneven);

        break;
      }

      default: abort();
    }

    // Add extra bits to avoid rows with less than two checks.

    added = 0;

    for (i = 0; i<M; i++)
    { e = mod2sparse_first_in_row(H,i);
      if (mod2sparse_at_end(e))
      { j = rand_int(N);
        e = mod2sparse_insert(H,i,j);
        added += 1;
      }
      e = mod2sparse_first_in_row(H,i);
      if (mod2sparse_at_end(mod2sparse_next_in_row(e)) && N>1)
      { do
        { j = rand_int(N);
        } while (j==mod2sparse_col(e));
        mod2sparse_insert(H,i,j);
        added += 1;
      }
    }

    if (added>0)qDebug("Added %d extra bit-checks to make row counts at least two",added);

    // Add extra bits to try to avoid problems with even column counts.

    n_full = 0;
    all_even = 1;
    for (z = 0; z<distrib_size(d); z++)
    { if (distrib_num(d,z)==M)
      { n_full += part[z];
      }
      if (distrib_num(d,z)%2==1)
      { all_even = 0;
      }
    }

    if (all_even && N-n_full>1 && added<2)
    { int a;
      for (a = 0; added+a<2; a++)
      { do
        { i = rand_int(M);
          j = rand_int(N);
        } while (mod2sparse_find(H,i,j));
        mod2sparse_insert(H,i,j);
      }
      qDebug("Added %d extra bit-checks to try to avoid problems from even column counts",a);
    }

    // Eliminate cycles of length four, if asked, and if possible.

    if (no4cycle)
    {
      elim4 = 0;

      for (t = 0; t<10; t++)
      { k = 0;
        for (j = 0; j<N; j++)
        { for (e = mod2sparse_first_in_col(H,j);
               !mod2sparse_at_end(e);
               e = mod2sparse_next_in_col(e))
          { for (f = mod2sparse_first_in_row(H,mod2sparse_row(e));
                 !mod2sparse_at_end(f);
                 f = mod2sparse_next_in_row(f))
            { if (f==e) continue;
              for (g = mod2sparse_first_in_col(H,mod2sparse_col(f));
                   !mod2sparse_at_end(g);
                   g = mod2sparse_next_in_col(g))
              { if (g==f) continue;
                for (h = mod2sparse_first_in_row(H,mod2sparse_row(g));
                     !mod2sparse_at_end(h);
                     h = mod2sparse_next_in_row(h))
                { if (mod2sparse_col(h)==j)
                  { do
                    { i = rand_int(M);
                    } while (mod2sparse_find(H,i,j));
                    mod2sparse_delete(H,e);
                    mod2sparse_insert(H,i,j);
                    elim4 += 1;
                    k += 1;
                    goto nextj;
                  }
                }
              }
            }
          }
        nextj: ;
        }
        if (k==0) break;
      }

      if (elim4>0)qDebug("Eliminated %d cycles of length four by moving checks within column",elim4);

      if (t==10)qDebug("Couldn't eliminate all cycles of length four in 10 passes");
    }


    if(part!=NULL){free(part);part=NULL;}
    if(u!=NULL){free(u);u=NULL;}

    return true;

}

/**
 * @brief LDPCMakeLDPC::column_partition
 * PARTITION THE COLUMNS ACCORDING TO THE SPECIFIED PROPORTIONS.  It
 * may not be possible to do this exactly.
 * @param d
 * List of proportions and number of check-bits
 * @param n
 * Total number of columns to partition
 * @return
 * Returns a pointer to an array of integers containing the numbers of
 * columns corresponding to the entries in the distribution passed.
 */
int *LDPCMakeLDPC::column_partition(distrib *d,int n)
{
  double *trunc;
  int *part;
  int cur, used;
  int i, j;

  if(d==NULL)return NULL;

  trunc = (double*)chk_alloc (distrib_size(d), sizeof(double));
  part = (int*)chk_alloc (distrib_size(d), sizeof(int));

  used = 0;
  for (i = 0; i<distrib_size(d); i++)
  { cur = floor(distrib_prop(d,i)*n);
    part[i] = cur;
    trunc[i] = distrib_prop(d,i)*n - cur;
    used += cur;
  }

  if (used>n)
  { abort();
  }

  while (used<n)
  { cur = 0;
    for (j = 1; j<distrib_size(d); j++)
    { if (trunc[j]>trunc[cur])
      { cur = j;
      }
    }
    part[cur] += 1;
    used += 1;
    trunc[cur] = -1;
  }

  free(trunc);
  return part;
}


