#ifndef LDPCDECODINGMETHOD_H
#define LDPCDECODINGMETHOD_H

#include <QVector>

class DecodingMethodInfo
{
public:
    typedef enum { Enum_block, Enum_bit, Prprp } decoding_method;
    DecodingMethodInfo(decoding_method method):dec_method(method){}
    const decoding_method dec_method; // Decoding method to use
};

class Enum_blockinfo : public  DecodingMethodInfo
{
public:
    Enum_blockinfo(): DecodingMethodInfo(DecodingMethodInfo::Enum_block){}
};

class Enum_bitinfo : public  DecodingMethodInfo
{
public:
    Enum_bitinfo(): DecodingMethodInfo(DecodingMethodInfo::Enum_bit){}

};

class Prprpinfo : public  DecodingMethodInfo
{
public:
    Prprpinfo(): DecodingMethodInfo(DecodingMethodInfo::Prprp){max_iterations=100;}
    int max_iterations; // Max Iterations (if -ve then always this number of itterations)
};

#endif // LDPCDECODINGMETHOD_H
