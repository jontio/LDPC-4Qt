#include "ldpcqconversions.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>

LDPCQConversions::LDPCQConversions()
{

}

//writes

bool LDPCQConversions::write_qvector_to_file(QString filename,QVector<int> &from)
{
    //this is not the best way of a format all etries shouls have the same size not a mix of 8 and 32 bits
    //this is done for compatiblitiy of the main LDPC software
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Can't open file for writing: can't save qvector";
        return false;
    }
    QDataStream out(&file);
    out<<(quint32)0x182358;
    out<<from;

    return true;
}

bool LDPCQConversions::write_qvector_to_file_old_format(QString filename,QVector<int> &from)
{
    //this is not the best way of a format, all etries shouls have the same size and not a mix of 8 and 32 bits
    //this is done for compatiblitiy with the main LDPC software
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"Can't open file for writing: can't save qvector";
        return false;
    }
    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);//this is a bother but this matches the orgigonal code
    int i=0;
    int type=from.at(0);
    char ch=(char)((qint32)((type-0x80)>>8));
    out<<(qint32)type;
    switch(ch)
    {
    case 'G':
        ch=from.at(1);
        out.writeRawData(&ch,1);
        for(i=2;i<from.size();i++)out<<(qint32)from.at(i);
        break;
    case 'P':
        for(i=1;i<from.size();i++)out<<(qint32)from.at(i);
        break;
    default:
        qDebug()<<"Unknown qvector format: can't save";
        return false;
        break;
    }

    return true;
}

bool LDPCQConversions::mod2sparse_save(QVector<int> &to)
{
    to.clear();
    return mod2sparse_write_to_qvector(to,::H);
}

bool LDPCQConversions::mod2sparse_write_to_qvector(QVector<int> &to,mod2sparse *m)
{
    if(m==NULL)
    {
        qDebug()<<"Error writing to qvector: matrix is NULL";
        return false;
    }
    mod2entry *e;
    int i;
    to.clear();
    to.push_back(m->n_rows);
    to.push_back(m->n_cols);
    for (i = 0; i<mod2sparse_rows(m); i++)
    {
        e = mod2sparse_first_in_row(m,i);
        if (!mod2sparse_at_end(e))
        {
            to.push_back(-(i+1));
            while (!mod2sparse_at_end(e))
            {
                to.push_back(mod2sparse_col(e)+1);
                e = mod2sparse_next_in_row(e);
            }
        }
    }
    to.push_back(0);
    return true;
}

bool LDPCQConversions::mod2sparse_append_to_qvector(QVector<int> &to,mod2sparse *m)
{
  if(m==NULL)
  {
      qDebug()<<"Error appending to qvector: matrix is NULL";
      return false;
  }
  mod2entry *e;
  int i;
  to.push_back(m->n_rows);
  to.push_back(m->n_cols);
  for (i = 0; i<mod2sparse_rows(m); i++)
  {
    e = mod2sparse_first_in_row(m,i);
    if (!mod2sparse_at_end(e))
    {
      to.push_back(-(i+1));
      while (!mod2sparse_at_end(e))
      {
        to.push_back(mod2sparse_col(e)+1);
        e = mod2sparse_next_in_row(e);
      }
    }
  }
  to.push_back(0);
  return true;
}

bool LDPCQConversions::mod2sparse_append_to_qvector(QVector<int> &to)
{
    return mod2sparse_append_to_qvector(to,::H);
}

bool LDPCQConversions::mod2sparse_save_parity_check_to_qvector(QVector<int> &to)
{
    to.clear();
    to.push_back(('P'<<8)+0x80);
    return mod2sparse_append_to_qvector(to);
}

bool LDPCQConversions::save_generator_to_qvector(QVector<int> &to,generator_type gentype)
{
    to.clear();
    to.push_back(('G'<<8)+0x80);

    switch (gentype)
    { case Sparse:
      {

        to.push_back('s');
        to.push_back(::M);
        to.push_back(::N);
        for (int i = 0; i<(::N); i++)to.push_back(::cols[i]);
        for (int i = 0; i<(::M); i++)to.push_back(::rows[i]);
        if(!mod2sparse_append_to_qvector(to,::L)||!mod2sparse_append_to_qvector(to,::U))
        {
            qDebug()<<"Failed saving generator";
            return false;
        }

        break;
      }
      case Dense: case Mixed:
      {

        if(gentype==Dense)to.push_back('d');
         else to.push_back('m');
        to.push_back(::M);
        to.push_back(::N);
        for (int i = 0; i<(::N); i++)to.push_back(::cols[i]);

        return mod2dense_append_to_qvector(to,::G);

        break;
      }
      default: abort();
    }

    return true;
}

bool LDPCQConversions::mod2dense_append_to_qvector(QVector<int> &to,mod2dense *m)
{
    int j, k;

    if(m==NULL)
    {
        qDebug()<<"mod2dense append to qvector failed: matrix is NULL";
        return false;
    }

    to.push_back(m->n_rows);
    to.push_back(m->n_cols);

    for (j = 0; j<mod2dense_cols(m); j++)
    {
        for (k = 0; k<m->n_words; k++)
        {
            to.push_back(m->col[j][k]);
        }
    }

    return true;
}

//reads


bool LDPCQConversions::read_qvector_to_file(QString filename,QVector<int> &to)
{
    //this is not the best way of a format all etries shouls have the same size not a mix of 8 and 32 bits
    //this is done for compatiblitiy of the main LDPC software
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can't open file for reading: can't read qvector";
        return false;
    }
    QDataStream in(&file);
    quint32 magic;
    in>>magic;
    if(magic!=(quint32)0x182358)
    {
        qDebug()<<"bad file format cant read";
        return false;
    }
    in>>to;

    return true;
}

bool LDPCQConversions::read_qvector_to_file_old_format(QString filename,QVector<int> &to)
{

//not implimented

    return false;
}



mod2dense *LDPCQConversions::mod2dense_read_from_qvector(QVector<int> &from,int &offset)
{
    int n_rows, n_cols;
    mod2dense *m;
    int j, k;
    int orgoffset=offset;

    if((from.size()-offset)<2)
    {
        qDebug()<<"Error converting qvector to mod2sparse, qvector to small and/or read offset to big";
        return NULL;
    }

    n_rows = from[0+offset];
    n_cols = from[1+offset];
    offset+=2;

    m = mod2dense_allocate(n_rows,n_cols);

    for (j = 0; j<mod2dense_cols(m); j++)
    {
        for (k = 0; k<m->n_words; k++)
        {
            if(offset>=from.size())
            {
                mod2dense_free(m);
                qDebug()<<"Error converting qvector to mod2sparse, qvector not right";
                offset=orgoffset;
                return NULL;
            }
            m->col[j][k] = from.at(offset);offset++;
        }
    }

    return m;
}

bool LDPCQConversions::mod2sparse_load(QVector<int> &from)
{
    ::H=mod2sparse_read_from_qvector(from);

    if(::H==NULL)return false;
    ::M = mod2sparse_rows(::H);
    ::N = mod2sparse_cols(::H);

    return true;
}

mod2sparse *LDPCQConversions::mod2sparse_read_from_qvector(QVector<int> &from,int &offset)
{
    int n_rows, n_cols;
    mod2sparse *m;
    int v, row, col;

    if((from.size()-offset)<2)
    {
        qDebug()<<"Error converting qvector to mod2sparse, qvector to small and/or read offset to big";
        return NULL;
    }

    n_rows = from[0+offset];
    n_cols = from[1+offset];

    m = mod2sparse_allocate(n_rows,n_cols);

    row = -1;

    for (int idx=2;(idx+offset)<from.size();idx++)
    {
        v = from.at(idx+offset);
        if (v==0)
        {
            offset+=idx;
            offset++;
            return m;
        }
         else if (v<0)
         {
            row = -v-1;
            if (row>=n_rows) break;
         }
          else
          {
              col = v-1;
              if (col>=n_cols) break;
              if (row==-1) break;
              mod2sparse_insert(m,row,col);
          }
    }

    /* Error if we get here. */

    mod2sparse_free(m);

    qDebug()<<"Error converting qvector to mod2sparse";

    return NULL;
}

mod2sparse *LDPCQConversions::mod2sparse_read_from_qvector(QVector<int> &from)
{
    int offset=0;
    return mod2sparse_read_from_qvector(from,offset);
}

bool LDPCQConversions::mod2sparse_load_parity_check_from_qvector(QVector<int> &from)
{
    ::H=NULL;

    if((from.size()<3)||(from[0]!=(('P'<<8)+0x80)))
    {
        qDebug()<<"Error loading parity check matrix: qvector doesn't contain a parity check matrix";
        return false;
    }

    int offset=1;
    ::H=mod2sparse_read_from_qvector(from,offset);

    if(::H==NULL)
    {
        qDebug()<<"Error loading parity check matrix";
        return false;
    }
    ::M = mod2sparse_rows(::H);
    ::N = mod2sparse_cols(::H);

    return true;
}

/**
 * @brief QConversions::load_generator_from_qvector
 * @param from
 * qvector matrix to load from
 * @param cols_only
 * Read only column ordering?
 * @param no_pchk_file
 * No parity check file used?
 * @return
 */
bool LDPCQConversions::load_generator_from_qvector(QVector<int> &from,int cols_only,int no_pchk_file)
{

    int M2, N2;
    int i;

    if((from.size()<4)||(from[0]!=(('G'<<8)+0x80)))
    {
        qDebug()<<"Error loading generator matrix: qvector doesn't contain a generator matrix";
        return false;
    }

    ::type=from.at(1);
    M2 = from.at(2);
    N2 = from.at(3);
    int idx=4;


    if (no_pchk_file)
    {
        ::M = M2;
        ::N = N2;
    }
    else
    {
        if (M2!=M || N2!=N)
        {
            qDebug("Generator matrix and parity-check matrix are incompatible\n");
            return false;
        }
    }

    cols = (int*)chk_alloc (N, sizeof *cols);
    rows = (int*)chk_alloc (M, sizeof *rows);
    ::L=NULL;
    ::U=NULL;
    ::G=NULL;

    for (i = 0; i<N; i++)
    {
        if(idx>=from.size()) goto error;
        cols[i] = from.at(idx);idx++;

    }

    if (!cols_only)
    {
        switch (type)
        {
        case 's':
        {
            for (i = 0; i<M; i++)
            {
                if(idx>=from.size()) goto error;
                rows[i] = from.at(idx);idx++;
            }

            if ((L = mod2sparse_read_from_qvector(from,idx)) == 0) goto error;
            if ((U = mod2sparse_read_from_qvector(from,idx)) == 0) goto error;

            if (mod2sparse_rows(L)!=M || mod2sparse_cols(L)!=M) goto garbled;
            if (mod2sparse_rows(U)!=M || mod2sparse_cols(U)<M) goto garbled;

            break;
        }

        case 'd':
        {

            if ((G = mod2dense_read_from_qvector(from,idx)) == 0) goto error;

            if (mod2dense_rows(G)!=M || mod2dense_cols(G)!=N-M) goto garbled;

            break;
        }

        case 'm':
        {

            if ((G = mod2dense_read_from_qvector(from,idx)) == 0) goto error;

            if (mod2dense_rows(G)!=M || mod2dense_cols(G)!=M) goto garbled;

            break;
        }

        default:
        {
            qDebug("Unknown type of generator matrix in qvector");
            if(::L!=NULL){mod2sparse_free(::L);::L=NULL;}
            if(::U!=NULL){mod2sparse_free(::U);::U=NULL;}
            if(::G!=NULL){mod2dense_free(::G);::G=NULL;}
            if(cols!=NULL){free(cols);cols=NULL;}
            if(rows!=NULL){free(rows);rows=NULL;}
            M=N=0;
            return false;
        }
        }
    }

    return true;

error:
    qDebug("Error reading generator matrix from qvector");


    if(::L!=NULL){mod2sparse_free(::L);::L=NULL;}
    if(::U!=NULL){mod2sparse_free(::U);::U=NULL;}
    if(::G!=NULL){mod2dense_free(::G);::G=NULL;}
    if(cols!=NULL){free(cols);cols=NULL;}
    if(rows!=NULL){free(rows);rows=NULL;}
    M=N=0;

    return false;

garbled:
    qDebug("Garbled generator matrix in qvector");

    if(::L!=NULL){mod2sparse_free(::L);::L=NULL;}
    if(::U!=NULL){mod2sparse_free(::U);::U=NULL;}
    if(::G!=NULL){mod2dense_free(::G);::G=NULL;}
    if(cols!=NULL){free(cols);cols=NULL;}
    if(rows!=NULL){free(rows);rows=NULL;}
    M=N=0;

    return false;



}
