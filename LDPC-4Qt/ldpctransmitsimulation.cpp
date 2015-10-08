#include "ldpctransmitsimulation.h"



LDPCTransmitSimulation::LDPCTransmitSimulation()
{
    privateinfo=new AWGNinfo;
}

LDPCTransmitSimulation::LDPCTransmitSimulation(ChannelInfo &channelinfo)
{
    privateinfo=new AWGNinfo;
    setChannel(channelinfo);
}

LDPCTransmitSimulation::~LDPCTransmitSimulation()
{
    delete privateinfo;
}

bool LDPCTransmitSimulation::setChannel(ChannelInfo &channelinfo)
{
    BSCinfo *bsc=NULL;
    AWGNinfo *awgn=NULL;
    AWLNinfo *awln=NULL;
    switch (channelinfo.channel)
    {
    case BSC:
    {
        bsc = (BSCinfo*)(&channelinfo);
        delete privateinfo;
        privateinfo=new BSCinfo;
        ((BSCinfo*)privateinfo)->error_prob=bsc->error_prob;
        break;
    }
    case AWGN:
    {
        awgn = (AWGNinfo*)(&channelinfo);
        delete privateinfo;
        privateinfo=new AWGNinfo;
        ((AWGNinfo*)privateinfo)->std_dev=awgn->std_dev;
        break;
    }
    case AWLN:
    {
        awln = (AWLNinfo*)(&channelinfo);
        delete privateinfo;
        privateinfo=new AWLNinfo;
        ((AWLNinfo*)privateinfo)->lwidth=awln->lwidth;
        break;
    }
    default:
        qDebug()<<"Error transmitting: Unknown channel";
        return false;
    }

    return true;
}

bool LDPCTransmitSimulation::transmit(QByteArray &from, QVector<double> &to)
{
    return transmit(from,to,*privateinfo);
}

bool LDPCTransmitSimulation::transmit(QByteArray &from, QVector<double> &to, ChannelInfo &channelinfo)
{
    BSCinfo *bsc=NULL;
    AWGNinfo *awgn=NULL;
    AWLNinfo *awln=NULL;
    switch (channelinfo.channel)
    {
    case BSC:
    {
        bsc = (BSCinfo*)(&channelinfo);
        break;
    }
    case AWGN:
    {
        awgn = (AWGNinfo*)(&channelinfo);
        break;
    }
    case AWLN:
    {
        awln = (AWLNinfo*)(&channelinfo);
        break;
    }
    default:
        qDebug()<<"Error transmitting: Unknown channel";
        return false;
    }

    double likelihoodratio=0;
    int b;

    to.resize(from.size());
    for(int i=0;i<from.size();i++)
    {

        if((qint8)from.at(i)>=1)b=1;
         else b=0;

        switch (channelinfo.channel)
        {
        case BSC:
        {
            int bsc_noise;
            bsc_noise = rand_uniform() < bsc->error_prob;
            likelihoodratio = b^bsc_noise;
            break;
        }
        case AWGN:
        {
            double awgn_noise;
            awgn_noise = awgn->std_dev * rand_gaussian();
            likelihoodratio =  b ? 1.0+awgn_noise : -1.0+awgn_noise;
            break;
        }
        case AWLN:
        {
            double awln_noise;
            awln_noise = awln->lwidth * rand_logistic();
            likelihoodratio =  b ? 1.0+awln_noise : -1.0+awln_noise;
            break;
        }
        default:
            abort();
        }

        to[i]=likelihoodratio;

    }

    return true;


}

QVector<double> &LDPCTransmitSimulation::transmit(QByteArray &from)
{
    if(!transmit(from,result,*privateinfo))result.clear();
    return result;
}
