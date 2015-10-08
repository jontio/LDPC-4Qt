#include "ldpcverify.h"


LDPCVerify::LDPCVerify()
{
    vars.clearglobal();
    resetstats();
}

LDPCVerify::LDPCVerify(LDPCCode &ldpccode, int _table)
{
    vars.clearglobal();
    resetstats();
    init(ldpccode, _table);
}

LDPCVerify::LDPCVerify(QVector<int> &ParityCheckMatrix, int _table)
{
    vars.clearglobal();
    resetstats();
    init(ParityCheckMatrix, _table);
}

LDPCVerify::LDPCVerify(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, int _table)
{
    vars.clearglobal();
    resetstats();
    init(ParityCheckMatrix,GeneratorMatrix, _table);
}

void LDPCVerify::resetstats()
{
    tot_srcerrs = 0;
    tot_chkerrs = 0;
    tot_botherrs = 0;
    bit_errs = 0;
    ber=std::numeric_limits<double>::quiet_NaN();
}

LDPCVerify::~LDPCVerify()
{
    vars.freelocal();
}

bool LDPCVerify::init(LDPCCode &ldpccode, int _table)
{
    return init(ldpccode.ParityCheckMatrix,ldpccode.GeneratorMatrix,_table);
}

bool LDPCVerify::init(QVector<int> &ParityCheckMatrix, int _table)
{
    LDPCMakeGen ldpcmakegen(ParityCheckMatrix,LDPCMakeGen::Dense);
    if(!ldpcmakegen.GeneratorMatrix.isEmpty())return init(ParityCheckMatrix,ldpcmakegen.GeneratorMatrix,_table);
    return false;
}

bool LDPCVerify::init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, int _table)
{
    tmpvars.copyglobaltolocal();
    vars.freelocal();
    vars.clearglobal();

    message.clear();

    table=_table;

    //load parity check matrix
    if(!qconversions.mod2sparse_load_parity_check_from_qvector(ParityCheckMatrix))
    {
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }
    if (::N<=::M)
    {
        qDebug("Can't decode if number of bits (%d) isn't greater than number of checks (%d)",::N,::M);
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

    vars.copyglobaltolocal();
    tmpvars.copylocaltoglobal();
    return true;
}

bool LDPCVerify::verify(QByteArray &from,QByteArray &to)
{
    message.clear();

    if(!vars.N||!vars.M)
    {
        qDebug("Error: Parity Check Matrix is Empty");
        return false;
    }

    tmpvars.copyglobaltolocal();
    vars.copylocaltoglobal();

    ::table=table;

    // Print header for table.
    if (table)
    {
        printf("  block chkerrs srcerrs\n");
    }

    resetstats();

    sblk.resize(N-M);
    cblk.resize(N);
    chks.resize(M);

    // Verify successive blocks.

    int n;
    for (n = 0; ; n++)
    {

        sblk=from.mid(n*(N-M),N-M);
        cblk=to.mid(n*N,N);

        if((sblk.size()<(N-M))||(cblk.size()<N))break;

        // Check that received block is a code word, and if not find the number of
        // parity check errors.

        int chkerr = check(H,cblk.data(),chks.data());

        // Check against source block

        int srcerr = 0;

        for (int i = M; i<N; i++)
        {
            if (cblk[cols[i]]!=sblk[i-M])
            {
                srcerr += 1;
            }
        }

        bit_errs += srcerr;

        // Print table entry.

        if (table)
        {
            printf("%6d %7d %7d\n",n,chkerr,srcerr);
        }

        // Increment totals.

        if (chkerr) tot_chkerrs += 1;
        if (srcerr) tot_srcerrs += 1;
        if (srcerr && chkerr) tot_botherrs += 1;

    }

    ber=(double)bit_errs/(n*(N-M));

    message=(QString::asprintf("Block counts: tot %d, with chk errs %d, with src errs %d, both %d\n",n, tot_chkerrs, tot_srcerrs, tot_botherrs)+QString::asprintf("Bit error rate (on message bits only): %.3e",ber));

    qDebug("Block counts: tot %d, with chk errs %d, with src errs %d, both %d",n, tot_chkerrs, tot_srcerrs, tot_botherrs);
    qDebug("Bit error rate (on message bits only): %.3e",ber);

    tmpvars.copylocaltoglobal();

    if(table)fflush(stdout);

    return true;
}

double LDPCVerify::getber()
{
    if(vars.N&&vars.M&&(vars.H!=NULL))return ber;
    return std::numeric_limits<double>::quiet_NaN();
}


