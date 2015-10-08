#ifndef TRANSMITSIMULATION_H
#define TRANSMITSIMULATION_H

#include <QByteArray>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <QDebug>

#include "ldpcchannel.h"
#include "../LDPC-codes/rand.h"


class LDPCTransmitSimulation
{
public:
    typedef enum { BSC, AWGN, AWLN } channel_type;

    LDPCTransmitSimulation();
    LDPCTransmitSimulation(ChannelInfo &channelinfo);
    bool setChannel(ChannelInfo &channelinfo);
    ~LDPCTransmitSimulation();
    bool transmit(QByteArray &from, QVector<double> &to, ChannelInfo &channelinfo);
    bool transmit(QByteArray &from, QVector<double> &to);
    QVector<double> &transmit(QByteArray &from);
private:
    ChannelInfo *privateinfo;
    QVector<double> result;
};

#endif // TRANSMITSIMULATION_H
