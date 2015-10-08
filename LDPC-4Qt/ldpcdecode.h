#ifndef LDPCDECODE_H
#define LDPCDECODE_H


#include <QByteArray>
#include <QVector>
#include <QDebug>
#include "ldpcchannel.h"
#include "ldpcdecodingmethod.h"
#include "ldpcmakegen.h"
#include "rcodevars.h"
#include "ldpcqconversions.h"
#include "../LDPC-codes/check.h"
#include "../LDPC-codes/dec.h"
#include "ldpccode.h"
#include <math.h>

class LDPCDecode
{
public:
    typedef enum { DontExtractMessage, ExtractMessage } messageextraction;
    typedef enum { BSC, AWGN, AWLN } channel_type;
    typedef enum { Enum_block, Enum_bit, Prprp } decoding_method;
    LDPCDecode();
    LDPCDecode(LDPCCode &ldpccode, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);
    LDPCDecode(QVector<int> &ParityCheckMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);
    LDPCDecode(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);
    ~LDPCDecode();
    bool init(LDPCCode &ldpccode, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);
    bool init(QVector<int> &ParityCheckMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);
    bool init(QVector<int> &ParityCheckMatrix, QVector<int> &GeneratorMatrix, ChannelInfo &channelinfo, DecodingMethodInfo &decodingmethodinfo, messageextraction extractmessagefromcodeword=DontExtractMessage, int table=0);//table shows information via printf. valid numbers are 0 1 and 2. 0 for no information
    bool decodeblock(QVector<double> &from, QByteArray &to);
    bool decode(QVector<double> &from, QByteArray &to);
    bool extract(QByteArray &fromto);
    QVector<double> &getbitprobabilities(){return bitpr;}
    QString getdecoingstatus();
    int getblocksizeout(){return (vars.N-vars.M);}
    int getblocksizein(){return (vars.N);}
    void reset();
private:
    ChannelInfo *privatechannelinfo;
    DecodingMethodInfo *privatedecodingmethodinfo;
    LDPCQConversions qconversions;
    RCodeVars vars;//stores stuff from rcode.c/rcode.h
    RCodeVars tmpvars;//stores stuff from rcode.c/rcode.h

    QVector<char> pchk;
    QVector<double> lratio;
    QVector<double> bitpr;

    double tot_iter;		// Double because can be huge for enum
    double chngd, tot_changed;	// Double because can be fraction if lratio==1

    int tot_valid;
    int valid;

    int block_no;
    int table;

    unsigned iters;

    messageextraction extractmessagefromcodeword;

    bool generatormatrixloaded;
    QByteArray totmp;

    QVector<double> tmpin,remainder;
    QByteArray tmpout;

};

#endif // LDPCDECODE_H
