
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>
#include <QWidget>
#include <QToolButton>

namespace Ui {
class Calculator;
}

class Calculator : public QMainWindow{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = 0);
    ~Calculator();


private:
    Ui::Calculator *ui;

    // for memory
    double MemSum;
    double SumTillNow;
    double FactorTillNow;
    bool waitingOp;

    // for operation
    void abortOp();
    bool calculate(double RVal, const QString &OpVal);

    QString AddOp;
    QString MultOp;


private slots :

    void NumPressed();

    void UnaryPressed();
    void AddPressed();
    void MultPressed();

    void DecimalPressed();
    void ChangeSignPressed();

    void BackSpacePressed();

    void ClearPressed();
    void ClearAllPressed();

    void SetMemory();
    void AddMemory();
    void SubtMemory();
    void MemClearPressed();
    void MemReadPressed();

    void CalculatePressed();

};

#endif
