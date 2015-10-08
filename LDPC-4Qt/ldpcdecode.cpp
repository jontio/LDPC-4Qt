#include "ldpcdecode.h"

LDPCDecode::LDPCDecode()
{
    privatechannelinfo = new AWGNinfo;
    privatedecodingmethodinfo = new Prprpinfo;
    vars.clearglobal();
    generatormatrixloaded=false;
}

LDPCDecode::LDPCDecode(LDPCCode &ldpccode, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{
    privatechannelinfo = new AWGNinfo;
    privatedecodingmethodinfo = new Prprpinfo;
    vars.clearglobal();
    generatormatrixloaded=false;
    init(ldpccode.ParityCheckMatrix,ldpccode.GeneratorMatrix,channelinfo,decodingmethodinfo,_extractmessagefromcodeword,_table);
}

LDPCDecode::LDPCDecode(QVector<int> &ParityCheckMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{
    privatechannelinfo = new AWGNinfo;
    privatedecodingmethodinfo = new Prprpinfo;
    vars.clearglobal();
    generatormatrixloaded=false;
    init(ParityCheckMatrix,channelinfo,decodingmethodinfo,_extractmessagefromcodeword,_table);
}

LDPCDecode::LDPCDecode(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{
    privatechannelinfo = new AWGNinfo;
    privatedecodingmethodinfo = new Prprpinfo;
    vars.clearglobal();
    generatormatrixloaded=false;
    init(ParityCheckMatrix,GeneratorMatrix,channelinfo,decodingmethodinfo,_extractmessagefromcodeword,_table);
}

LDPCDecode::~LDPCDecode()
{
    vars.freelocal();
    delete privatedecodingmethodinfo;
    delete privatechannelinfo;
}

bool LDPCDecode::init(LDPCCode &ldpccode, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{
    return init(ldpccode.ParityCheckMatrix, ldpccode.GeneratorMatrix, channelinfo, decodingmethodinfo, _extractmessagefromcodeword, _table);
}

bool LDPCDecode::init(QVector<int> &ParityCheckMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{
    LDPCMakeGen ldpcmakegen(ParityCheckMatrix,LDPCMakeGen::Dense);
    return init(ParityCheckMatrix, ldpcmakegen.GeneratorMatrix, channelinfo, decodingmethodinfo, _extractmessagefromcodeword, _table);
}

bool LDPCDecode::init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction _extractmessagefromcodeword, int _table)
{

    tmpvars.copyglobaltolocal();
    vars.freelocal();
    vars.clearglobal();

    generatormatrixloaded=false;

    remainder.clear();

    extractmessagefromcodeword=_extractmessagefromcodeword;

    table=_table;

    BSCinfo *bsc=NULL;
    AWGNinfo *awgn=NULL;
    AWLNinfo *awln=NULL;
    switch (channelinfo.channel)
    {
    case BSC:
    {
        bsc = (BSCinfo*)(&channelinfo);
        delete privatechannelinfo;
        privatechannelinfo=new BSCinfo;
        ((BSCinfo*)privatechannelinfo)->error_prob=bsc->error_prob;
        break;
    }
    case AWGN:
    {
        awgn = (AWGNinfo*)(&channelinfo);
        delete privatechannelinfo;
        privatechannelinfo=new AWGNinfo;
        ((AWGNinfo*)privatechannelinfo)->std_dev=awgn->std_dev;
        break;
    }
    case AWLN:
    {
        awln = (AWLNinfo*)(&channelinfo);
        delete privatechannelinfo;
        privatechannelinfo=new AWLNinfo;
        ((AWLNinfo*)privatechannelinfo)->lwidth=awln->lwidth;
        break;
    }
    default:
        qDebug()<<"Error decoding: Unknown channel";
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

    //Enum_blockinfo *enum_block=NULL;
    //Enum_bitinfo *enum_bit=NULL;
    Prprpinfo *prprp=NULL;
    switch (decodingmethodinfo.dec_method)
    {
    case Enum_block:
    {
        //enum_block = (Enum_blockinfo*)(&decodingmethodinfo);
        delete privatedecodingmethodinfo;
        privatedecodingmethodinfo=new Enum_blockinfo;
        if(GeneratorMatrix.isEmpty())
        {
            qDebug()<<"Error decoding: need a Generator matrix for Enum_block decoding method";
            vars.freeglobal();
            tmpvars.copylocaltoglobal();
            return false;
        }
        break;
    }
    case Enum_bit:
    {
        //enum_bit = (Enum_bitinfo*)(&decodingmethodinfo);
        delete privatedecodingmethodinfo;
        privatedecodingmethodinfo=new Enum_bitinfo;
        if(GeneratorMatrix.isEmpty())
        {
            qDebug()<<"Error decoding: need a Generator matrix for Enum_bit decoding method";
            vars.freeglobal();
            tmpvars.copylocaltoglobal();
            return false;
        }
        break;
    }
    case Prprp:
    {
        prprp = (Prprpinfo*)(&decodingmethodinfo);
        delete privatedecodingmethodinfo;
        privatedecodingmethodinfo=new Prprpinfo;
        ((Prprpinfo*)privatedecodingmethodinfo)->max_iterations=prprp->max_iterations;
        break;
    }
    default:
        qDebug()<<"Error decoding: Unknown decoding method";
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

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

    //load generator matrix if available
    if((!GeneratorMatrix.isEmpty())&&(!qconversions.load_generator_from_qvector(GeneratorMatrix,0,0)))
    {
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

    // Allocate other space.

    lratio.resize(N);
    pchk.resize(M);
    bitpr.resize(N);

    // Print header for summary table.

    if(table==1)printf("  block iterations valid  changed\n");

    // Do the setup for the decoding method.

    switch (decodingmethodinfo.dec_method)
    {
    case Prprp:
    {
        if (table==2)printf("  block  iter  changed  perrs    loglik   Eperrs   Eloglik  entropy\n");
        break;
    }
    case Enum_block: case Enum_bit:
    {

        if (N-M>31)
        {
            qDebug("Trying to decode messages with %d bits by exhaustive enumeration is absurd!",N-M);
            vars.freeglobal();
            tmpvars.copylocaltoglobal();
            return false;
        }

        if (table==2) printf("  block   decoding  likelihood\n");


        break;
    }
    default:
        qDebug()<<"Error decoding: Unknown decoding method";
        vars.freeglobal();
        tmpvars.copylocaltoglobal();
        return false;
    }

    if(!GeneratorMatrix.isEmpty())generatormatrixloaded=true;

    tot_iter = 0;
    tot_valid = 0;
    tot_changed = 0;
    block_no = 0;

    if(table)fflush(stdout);

    vars.copyglobaltolocal();
    tmpvars.copylocaltoglobal();
    return true;
}

void LDPCDecode::reset()
{
    remainder.clear();
    tot_iter = 0;
    tot_valid = 0;
    tot_changed = 0;
    block_no = 0;
}

bool LDPCDecode::decodeblock(QVector<double> &from, QByteArray &to)
{

    tmpvars.copyglobaltolocal();
    vars.copylocaltoglobal();

    to.resize(N);//code word i think

    if(from.size()!=::N)
    {
        qDebug()<<"Error decoding: input block wrong size";
        tmpvars.copylocaltoglobal();
        return false;
    }

    ::block_no=block_no;
    ::table=table;

    // Find likelihood ratio for each bit.

    BSCinfo *bsc=NULL;
    AWGNinfo *awgn=NULL;
    AWLNinfo *awln=NULL;
    switch (privatechannelinfo->channel)
    {
    case BSC:
    {
        bsc=((BSCinfo*)privatechannelinfo);
        for (int i = 0; i<N; i++)
        {
            lratio[i] = from[i]==1 ? (1.0-(bsc->error_prob)) / (bsc->error_prob) : (bsc->error_prob) / (1.0-(bsc->error_prob));
        }
        break;
    }
    case AWGN:
    {
        awgn=((AWGNinfo*)privatechannelinfo);
        for (int i = 0; i<N; i++)
        {
            lratio[i] = exp(2.0*from[i]/((awgn->std_dev)*(awgn->std_dev)));
        }
        break;
    }
    case AWLN:
    {
        awln=((AWLNinfo*)privatechannelinfo);
        for (int i = 0; i<N; i++)
        {
            double e, d1, d0;
            e = exp(-(from[i]-1.0)/(awln->lwidth));
            d1 = 1.0 / ((1.0+e)*(1.0+1.0/e));
            e = exp(-(from[i]+1)/(awln->lwidth));
            d0 = 1.0 / ((1.0+e)*(1.0+1.0/e));
            lratio[i] = d1/d0;
        }
        break;
    }
    default:
        qDebug()<<"Error decoding: Unknown decoding method";
        tmpvars.copylocaltoglobal();
        return false;
    }

    // Try to decode using the specified method.

    //Enum_blockinfo *enum_block=NULL;
    //Enum_bitinfo *enum_bit=NULL;
    Prprpinfo *prprp=NULL;
    switch (privatedecodingmethodinfo->dec_method)
    {
    case Prprp:
    {
        prprp=((Prprpinfo*)privatedecodingmethodinfo);
        max_iter=prprp->max_iterations;
        iters = prprp_decode (H, lratio.data(), to.data(), pchk.data(), bitpr.data());
        break;
    }
    case Enum_block:
    {
        //enum_block=((Enum_blockinfo*)privatedecodingmethodinfo);
        iters = enum_decode (lratio.data(), to.data(), bitpr.data(), 1);
        break;
    }
    case Enum_bit:
    {
        //enum_bit=((Enum_bitinfo*)privatedecodingmethodinfo);
        iters = enum_decode (lratio.data(), to.data(), bitpr.data(), 0);
        break;
    }
    default:
        qDebug()<<"Error decoding: Unknown decoding method";
        tmpvars.copylocaltoglobal();
        return false;
    }

    // See if it worked, and how many bits were changed.

    valid = check(::H,to.data(),pchk.data())==0;

    chngd = changed(lratio.data(),to.data(),::N);

    tot_iter += iters;
    tot_valid += valid;
    tot_changed += chngd;

    // Print summary table entry.

    if (table==1)
    {
        printf("%7d %10f    %d  %8.1f\n",block_no, (double)iters, valid, (double)chngd);// iters is printed as a double to avoid problems if it's >= 2^31
    }

    if(table)fflush(stdout);

    if(extractmessagefromcodeword==ExtractMessage)extract(to);

    block_no++;

    tmpvars.copylocaltoglobal();
    return true;
}

bool LDPCDecode::decode(QVector<double> &from, QByteArray &to)
{
    to.clear();

    if(vars.N<=0)
    {
        qDebug()<<"Error encoding: input block size to small";
        return false;
    }

    if(remainder.size()>vars.N)
    {
        qDebug()<<"Warning encoding: remainder vector bigger than input block size flushing remainder";
        remainder.clear();
    }

    tmpin=remainder;
    tmpin+=from.mid(0,vars.N-remainder.size());

    for(int i=(vars.N-remainder.size());;i+=vars.N)
    {
        if(tmpin.size()<vars.N)
        {
            remainder=tmpin;
            return true;
        }

        if(!decodeblock(tmpin,tmpout))return false;
        to+=tmpout;

        tmpin=from.mid(i,vars.N);
    }


}

QString LDPCDecode::getdecoingstatus()
{
    if(!vars.N)return QString("No decoding information yet");
    return QString::asprintf("Decoded %d blocks, %d valid.  Average %.1f iterations, %.0f%% bit changes",block_no,tot_valid,(double)tot_iter/block_no,100.0*(double)tot_changed/(vars.N*block_no));
}

bool LDPCDecode::extract(QByteArray &fromto)
{
    if(generatormatrixloaded)
    {
        totmp.resize(vars.N-vars.M);
        int j=0;
        for (int i=(vars.M);i<(vars.N);i++)
        {
            totmp[j]=fromto.at(vars.cols[i]);
            j++;
        }
        fromto=totmp;
        return true;
    }
    return false;
}

