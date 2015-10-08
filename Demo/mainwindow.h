#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../LDPC-4Qt/ldpc4qt.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void message(const QString &msg);
    void binarymessage(const QByteArray &ba);
private slots:
    void on_pushTest_clicked();
    void on_pushTest2_clicked();
    void on_pushTest3_clicked();
private:
    Ui::MainWindow *ui;
    LDPCCode ldpccode;//the LDPC code
};

#endif // MAINWINDOW_H
