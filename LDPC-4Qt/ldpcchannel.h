#ifndef LDPCCHANNEL_H
#define LDPCCHANNEL_H

#include <QVector>
#include <math.h>

class ChannelInfo
{
public:
    typedef enum { BSC, AWGN, AWLN } channel_type;
    ChannelInfo(channel_type type):channel(type){}
    const channel_type channel;	// Type of channel
};

class BSCinfo : public  ChannelInfo
{
public:
    BSCinfo(): ChannelInfo(ChannelInfo::BSC){error_prob=0;}
    double error_prob;	// Error probability for BSC
};

class AWGNinfo : public  ChannelInfo
{
public:
    AWGNinfo(): ChannelInfo(ChannelInfo::AWGN){std_dev=0;}
    double std_dev;		// Noise standard deviation for AWGN
    void setEbNo(double EbNo_in_dB){std_dev=pow(10.0,(EbNo_in_dB/-20.0));}
    double getEbNo(){return -20.0*log10(std_dev);}
};

class AWLNinfo : public  ChannelInfo
{
public:
    AWLNinfo(): ChannelInfo(ChannelInfo::AWLN){lwidth=0;}
    double lwidth;		// Width of noise distributoin for AWLN
};


#endif // LDPCCHANNEL_H
