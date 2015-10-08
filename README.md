# LDPC-4Qt
LDPC (Low Density Parity Check) FEC (Forward error correction) for use with Qt C++ projects. This is a Qt C++ wrapper for the C implementation of [radfordneal/LDPC-codes](https://github.com/radfordneal/LDPC-codes)

##Example usage

```C++
    //create an ldpc code via random methods
    LDPCCode ldpccode(1000,2000,2,LDPCMakeLDPC::Evenboth,"2x2/7x3/1x7",true);

    //create an encoder for this code
    LDPCEncode encode(ldpccode);

    //make 100 blocks of random bits
    QByteArray src;
    encode.createrandomdata(100,src);

    //encode the source
    QByteArray enc;
    encode.encode(src,enc);

    //make an AWGN channel with an EbNo of 0.92dB
    AWGNinfo channelinfo;
    channelinfo.setEbNo(0.92);

    //initalize the simulation with this AWGN channel
    LDPCTransmitSimulation txsim(channelinfo);

    //run the encoded data through the channel simulation
    QVector<double> rec;
    txsim.transmit(enc,rec);

    //create a decoding method that uses probability propagation decoding with a maximum of 250 itterations
    Prprpinfo decodingmethodinfo;
    decodingmethodinfo.max_iterations=250;

    //make a decoder for the LDPC code that takes into account the AWGN channel and uses the decoding method
    LDPCDecode decode(ldpccode,channelinfo,decodingmethodinfo);

    //decode the received data
    QByteArray dec;
    decode.decode(rec,dec);

    //have a look at the current decoding status
    qDebug()<<decode.getdecoingstatus();

    //make an object to compare the source with the decoded data using the LDPC code
    LDPCVerify verify(ldpccode);

    //perfom the comparison
    verify.verify(src,dec);
    
    //have a look at the BER (Bit Error Rate)
    qDebug()<<"BER ="<<verify.getber();
```

