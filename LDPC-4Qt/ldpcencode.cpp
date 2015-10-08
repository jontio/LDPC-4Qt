#include "ldpcencode.h"


LDPCEncode::LDPCEncode(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix)
{
    vars.clearglobal();
    u=v=NULL;blocksizein=0;
    init(ParityCheckMatrix,GeneratorMatrix);
}

LDPCEncode::LDPCEncode(QVector<int> &ParityCheckMatrix)
{
    vars.clearglobal();
    u=v=NULL;blocksizein=0;
    init(ParityCheckMatrix);
}

LDPCEncode::LDPCEncode(LDPCCode &ldpccode)
{
    vars.clearglobal();
    u=v=NULL;blocksizein=0;
    init(ldpccode);
}

LDPCEncode::LDPCEncode()
{
    vars.clearglobal();
    u=v=NULL;blocksizein=0;
}

LDPCEncode::~LDPCEncode()
{
    vars.freelocal();
    if(u!=NULL){mod2dense_free(u);u=NULL;}
    if(v!=NULL){mod2dense_free(v);v=NULL;}
}

bool LDPCEncode::init(QVector<int> &ParityCheckMatrix)
{
    LDPCMakeGen ldpcmakegen(ParityCheckMatrix,LDPCMakeGen::Dense);
    if(!ldpcmakegen.GeneratorMatrix.isEmpty())return init(ParityCheckMatrix,ldpcmakegen.GeneratorMatrix);
    return false;
}

bool LDPCEncode::init(LDPCCode &ldpccode)
{
    return init(ldpccode.ParityCheckMatrix,ldpccode.GeneratorMatrix);
}

bool LDPCEncode::init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix)
{
    remainder.clear();

    tmpvars.copyglobaltolocal();
    vars.freelocal();
    vars.clearglobal();

    //load parity check matrix
    if(!qconversions.mod2sparse_load_parity_check_from_qvector(ParityCheckMatrix))
    {
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }
    if (::N<=::M)
    {
        qDebug("Can't encode if number of bits (%d) isn't greater than number of checks (%d)",::N,::M);
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

    //load generator matrix
    if(!qconversions.load_generator_from_qvector(GeneratorMatrix,0,0))
    {
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

    //Allocate needed space.
    if(u!=NULL){mod2dense_free(u);u=NULL;}
    if(v!=NULL){mod2dense_free(v);v=NULL;}
    if (::type=='d')
    {
        u = mod2dense_allocate(N-M,1);
        v = mod2dense_allocate(M,1);
    }
    if (::type=='m')
    {
        u = mod2dense_allocate(M,1);
        v = mod2dense_allocate(M,1);
    }
    chks.resize(M);

    blocksizein=N-M;

    vars.copyglobaltolocal();
    tmpvars.copylocaltoglobal();
    return true;

}

bool LDPCEncode::encodeblock(QByteArray &from, QByteArray &to)
{
    tmpvars.copyglobaltolocal();
    vars.copylocaltoglobal();

    if(::H==NULL)
    {
        qDebug()<<"error encoding block: no parity check matrix given";
        to.clear();
        tmpvars.copylocaltoglobal();
        return false;
    }

    if(((::type=='d'||::type=='m')&&::G==NULL)||(::type=='s'&&(::U==NULL||::L==NULL)))
    {
        qDebug()<<"error encoding block: no generator matrix given";
        to.clear();
        tmpvars.copylocaltoglobal();
        return false;
    }

    if(from.size()!=(N-M))
    {
        qDebug()<<"error encoding block: wrong input block size given";
        to.clear();
        tmpvars.copylocaltoglobal();
        return false;
    }

    chks.resize(M);

    if (::type=='d')
    {
        if(u==NULL)u = mod2dense_allocate(N-M,1);
        if(v==NULL)v = mod2dense_allocate(M,1);
        if((u->n_rows!=(N-M))||(u->n_cols!=1))
        {
            mod2dense_free(u);
            u = mod2dense_allocate(N-M,1);
        }
        if((v->n_rows!=(M))||(v->n_cols!=1))
        {
            mod2dense_free(v);
            v = mod2dense_allocate(M,1);
        }

    }

    if (::type=='m')
    {
        if(u==NULL)u = mod2dense_allocate(M,1);
        if(v==NULL)v = mod2dense_allocate(M,1);
        if((u->n_rows!=M)||(u->n_cols!=1))
        {
            mod2dense_free(u);
            u = mod2dense_allocate(M,1);
        }
        if((v->n_rows!=(M))||(v->n_cols!=1))
        {
            mod2dense_free(v);
            v = mod2dense_allocate(M,1);
        }
    }

    to.resize(N);

    // Compute encoded block.
    switch (::type)
    { case 's':
      { sparse_encode (from.data(),to.data());
        break;
      }
      case 'd':
      { dense_encode (from.data(),to.data(), u, v);
        break;
      }
      case 'm':
      { mixed_encode (from.data(),to.data(), u, v);
        break;
      }
    }

    // Check that encoded block is a code word.
    mod2sparse_mulvec (H, to.data(), chks.data());

    for (int i = 0; i<M; i++)
    {
        if (chks.at(i))
        {
            qDebug("Block is not a code word!  (Fails check %d)",i);
            to.clear();
            tmpvars.copylocaltoglobal();
            return false;
        }
    }

    tmpvars.copylocaltoglobal();
    return true;

}

bool LDPCEncode::encode(QByteArray &from, QByteArray &to)
{
    to.clear();

    if(blocksizein<=0)
    {
        qDebug()<<"Error encoding: input block size to small";
        return false;
    }

    if(remainder.size()>blocksizein)
    {
        qDebug()<<"Warning encoding: remainder vector bigger than input block size flushing remainder";
        remainder.clear();
    }

    tmpin=remainder;
    tmpin+=from.left(blocksizein-remainder.size());

    for(int i=(blocksizein-remainder.size());;i+=blocksizein)
    {
        if(tmpin.size()<blocksizein)
        {
            remainder=tmpin;
            return true;
        }

        if(!encodeblock(tmpin,tmpout))return false;
        to+=tmpout;

        tmpin=from.mid(i,blocksizein);
    }


}
