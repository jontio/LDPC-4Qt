#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>


#include <QDebug>


//-----------test 1

//basic test.
void MainWindow::on_pushTest_clicked()
{

    ui->plainTextEdit->clear();
    message("this test does the basic stuff of code reneration uses and validation.");

    //------------------
    //  start the test
    //------------------

    message("create an ldpc code via random methods");
    LDPCCode ldpccode(1000,2000,2,LDPCMakeLDPC::Evenboth,"3",true);

    message("create an encoder for this code");
    LDPCEncode encode(ldpccode);

    message("make 100 blocks of random bits");
    QByteArray src;
    encode.createrandomdata(100,src);

    message("encode the source");
    QByteArray enc;
    encode.encode(src,enc);

    message("make an AWGN channel with an EbNo of 0.92dB");
    AWGNinfo channelinfo;
    channelinfo.setEbNo(0.92);

    message("initalize the simulation with this AWGN channel");
    LDPCTransmitSimulation txsim(channelinfo);

    message("run the encoded data through the channel simulation");
    QVector<double> rec;
    txsim.transmit(enc,rec);

    message("create a decoding method that uses probability propagation decoding with a maximum of 250 itterations");
    Prprpinfo decodingmethodinfo;
    decodingmethodinfo.max_iterations=250;

    message("make a decoder for the LDPC code that takes into account the AWGN channel and uses the decoding method");
    LDPCDecode decode(ldpccode,channelinfo,decodingmethodinfo);

    message("decode the received data... (this make take a while and this application will freeze till this is done)");
    QByteArray dec;
    decode.decode(rec,dec);

    message("have a look at the current decoding status");
    message("&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"Blue\">"+decode.getdecoingstatus()+"</>");

    message("make an object to compare the source with the decoded data using the LDPC code");
    LDPCVerify verify(ldpccode);

    message("perfom the comparison");
    verify.verify(src,dec);

    message("have a look at the BER (Bit Error Rate)");
    qDebug()<<"BER ="<<verify.getber();


    //--------------------
    //  the test is done
    //--------------------

}


//----------------test 2

//this test will try to load an LDPC code from disk if not it will create one and save it to disk for later use.
void MainWindow::on_pushTest2_clicked()
{
    ui->plainTextEdit->clear();
    message("this test will try to load an LDPC code from disk if not it will create one and save it to disk for later use. It is slow as it uses a 1/2 rate code with a message word length of 5000 bits");


    //------------------
    //  start the test
    //------------------


    if(ldpccode.isEmpty())
    {
        if(!ldpccode.loadfromfile("ex-ldpcvar-5000a"))
        {
            message("No LDPC code. creating a large 5000x10000 code with a sparse generator matrix via random methods and saving to disk... this will take some time and this application will freeze untill finished");
            ldpccode.create(5000,10000,2,LDPCMakeLDPC::Evenboth,"2x2/7x3/1x7",true,LDPCMakeGen::Sparse);
            ldpccode.savetofile("ex-ldpcvar-5000a");
        } else message("Loaded previous LDPC code from disk");
    }

    message("create an encoder for this code");
    LDPCEncode encode(ldpccode);

    message("make 100 blocks of random bits");
    QByteArray src;
    encode.createrandomdata(100,src);

    message("encode the source");
    QByteArray enc;
    encode.encode(src,enc);

    message("make an AWGN channel with an EbNo of 0.92dB");
    AWGNinfo channelinfo;
    channelinfo.setEbNo(0.92);

    message("initalize the simulation with this AWGN channel");
    LDPCTransmitSimulation txsim(channelinfo);

    message("run the encoded data through the channel simulation");
    QVector<double> rec;
    txsim.transmit(enc,rec);

    message("create a decoding method that uses probability propagation decoding with a maximum of 250 itterations");
    Prprpinfo decodingmethodinfo;
    decodingmethodinfo.max_iterations=250;

    message("make a decoder for the LDPC code that takes into account the AWGN channel and uses the decoding method");
    LDPCDecode decode(ldpccode,channelinfo,decodingmethodinfo);

    message("decode the received data... (this make take a while and this application will freeze till this is done)");
    QByteArray dec;
    decode.decode(rec,dec);

    message("have a look at the current decoding status");
    message("&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"Blue\">"+decode.getdecoingstatus()+"</>");

    message("make an object to compare the source with the decoded data using the LDPC code");
    LDPCVerify verify(ldpccode);

    message("perfom the comparison");
    verify.verify(src,dec);

    message("have a look at the BER (Bit Error Rate)");
    qDebug()<<"BER ="<<verify.getber();


    //--------------------
    //  the test is done
    //--------------------


}


//------------ test 3

//This test creates a BER versus EbNo plot file (called \"results.m\") that can be displayed using matlab or octave
void MainWindow::on_pushTest3_clicked()
{
    ui->plainTextEdit->clear();
    message("This test creates a BER versus EbNo plot file (called \"results.m\") that can be displayed using matlab or octave");
    message("WARNING THIS IS REALLY SLOW!!!! go and have a cup of tea while it does its thing");

    //------------------
    //  start the test
    //------------------

    QByteArray src;
    QByteArray enc;
    QVector<double> rec;
    QByteArray dec;

    message("create an ldpc code size of 1000x2000 via random methods...");
    LDPCCode ldpccode(1000,2000,1,LDPCMakeLDPC::Evenboth,"3",true,LDPCMakeGen::Sparse);

    message("create an encoder for this code");
    LDPCEncode encode(ldpccode);

    message("make 1000 blocks of random bits");

    encode.createrandomdata(1000,src);

    message("encode the source");
    encode.encode(src,enc);

    message("make an AWGN channel");
    AWGNinfo channelinfo;

    message("create a decoding method that uses probability propagation decoding with a maximum of 250 itterations");
    Prprpinfo decodingmethodinfo;
    decodingmethodinfo.max_iterations=250;

    message("make a decoder");
    LDPCDecode decode;

    message("create a channel simulation");
    LDPCTransmitSimulation txsim;

    message("make an object to compare the source with the decoded data using the LDPC code");
    LDPCVerify verify(ldpccode);

    message("create a file to save results");
    QFile file("results.m");
    if(!file.open(QIODevice::WriteOnly))
    {
        message("Can't open file for writing: failing");
        return;
    }
    QTextStream out(&file);

    //run through some EbNo values
    const QVector<double> EbNos = QVector<double>() << 0.6 << 0.8 << 0.9 << 1.0 << 1.2 << 1.5 << 2.0 << 4.0 << 5.0 << 6.0 << 9.0;
    out<<"EbNos=[";for(int i=0;i<EbNos.size();i++){if(i==(EbNos.size()-1))out<<EbNos.at(i)<<"];\n";else out<<EbNos.at(i)<<",";}
    out<<"BERs=[";
    for(int i=0;i<EbNos.size();i++)
    {
        double EbNo=EbNos.at(i);

        message(((QString)"set the AWGN noise of the channel to %1dB and change the decoder to match").arg(EbNo));
        channelinfo.setEbNo(EbNo);
        txsim.setChannel(channelinfo);
        decode.init(ldpccode,channelinfo,decodingmethodinfo);

        message("run the encoded data through the channel simulation");
        txsim.transmit(enc,rec);

        message("decode the received data... (this make take a while and this application will freeze till this is done)");
        decode.decode(rec,dec);

        verify.verify(src,dec);

        message("&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"Blue\">EbNo = "+((QString)"%1").arg(EbNo)+" BER = "+((QString)"%1").arg(verify.getber())+"</>");

        if(i==(EbNos.size()-1))out<<verify.getber()<<"];\n";
         else out<<verify.getber()<<",";
    }
    out<<"plot(EbNos,BERs,'o:','MarkerFaceColor',[0 0 1],'MarkerSize',8);set(gca,'yscale','log');xlabel('EbNo (dB)');ylabel('BER');grid('on');\n";
    out<<"EbNosNoCodingTheroy= 0:0.1:max(EbNos);BERsNoCodingTheroy = 0.5*erfc(sqrt(10.^(EbNosNoCodingTheroy/10)));hold on;plot(EbNosNoCodingTheroy,BERsNoCodingTheroy,'r-');hold off;";
    out<<"legend('LDPC Code','Uncoded theory');";
    out<<"title('LDPC 1000x2000 code simulation result');";

    message("now open \"results.m\" with Octave (free program) or Matlab to see the plot");


    //--------------------
    //  the test is done
    //--------------------



}

//---- end of all tests


MainWindow *thisinit;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    thisinit->message("&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"Red\">"+msg+"</>");
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
    fflush(stderr);
}

void MainWindow::message(const QString &msg)
{    
    ui->plainTextEdit->appendHtml(msg);
    QApplication::processEvents(QEventLoop::AllEvents);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thisinit=this;
    qInstallMessageHandler(myMessageOutput);

   // setRandFileName(((QString)(QApplication::applicationDirPath()+"/randfile")).toLocal8Bit().data());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::binarymessage(const QByteArray &ba)
{
    QByteArray tba=ba;
    for(int i=0;i<tba.size();i++)tba[i]=tba[i]+48;
    ui->plainTextEdit->appendPlainText(tba);
}

