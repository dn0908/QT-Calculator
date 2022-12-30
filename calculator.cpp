#include "calculator.h"
#include "ui_calculator.h"


#include <QRegularExpression>
#include <QtGui>
#include <math.h>
#include <QMainWindow>
#include <QPushButton>


Calculator::Calculator(QWidget *parent) :
    // (Constructor)
    QMainWindow(parent),

    // Create the UI class and assign it to the ui member
    ui(new Ui::Calculator)

{


    // set defaullt values
    MemSum = 0.0;
    SumTillNow = 0.0;
    FactorTillNow = 0.0;
    waitingOp = true;

    // Setup UI
    ui->setupUi(this);

    // Display
    ui->Display->setText(QString::number(SumTillNow));



    // CONNECTING BUTTONS //

    // ********** Numbers ********** //
    QPushButton *numButtons[10];

    for(int i = 0; i < 10; ++i){
        QString buttonName = "Button" + QString::number(i);

        // Get the buttons by name and add to array
        numButtons[i] = Calculator::findChild<QPushButton *>(buttonName);
        // When the button is released call num_pressed()
        connect(numButtons[i], SIGNAL(released()), this,
                SLOT(NumPressed()));
    } 

    // ********** Math Operations ********** //
    connect(ui->Add, SIGNAL(released()), this,
            SLOT(AddPressed()));
    connect(ui->Subtract, SIGNAL(released()), this,
            SLOT(AddPressed()));
    connect(ui->Multiply, SIGNAL(released()), this,
            SLOT(MultPressed()));
    connect(ui->Divide, SIGNAL(released()), this,
            SLOT(MultPressed()));
    connect(ui->SquareRoot, SIGNAL(released()), this,
            SLOT(UnaryPressed()));
    connect(ui->Square, SIGNAL(released()), this,
            SLOT(UnaryPressed()));
    connect(ui->Reciprocal, SIGNAL(released()), this,
            SLOT(UnaryPressed()));

    // ********** Backspace ********** //
    connect(ui->BackSpace, SIGNAL(released()), this,
            SLOT(BackSpacePressed()));

    // **********  C & CE : clear buttons ********** //
    connect(ui->Clear, SIGNAL(released()), this,
            SLOT(ClearPressed()));
    connect(ui->ClearAll, SIGNAL(released()), this,
            SLOT(ClearAllPressed()));

    connect(ui->Decimal, SIGNAL(released()), this,
            SLOT(DecimalPressed()));
    // **********  +/-  button ********** //
    connect(ui->ChangeSign, SIGNAL(released()), this,
            SLOT(ChangeSignPressed()));

    // **********  Memory buttons ********** //
    connect(ui->MemRead, SIGNAL(released()), this,
            SLOT(MemReadPressed()));
    connect(ui->MemClear, SIGNAL(released()), this,
            SLOT(MemClearPressed()));
    connect(ui->MemSet, SIGNAL(released()), this,
            SLOT(SetMemory()));
    connect(ui->MemPlus, SIGNAL(released()), this,
            SLOT(AddMemory()));
    connect(ui->MemMinus, SIGNAL(released()), this,
            SLOT(SubtMemory()));

    // **********  = calculating button ********** //
    connect(ui->Calculate, SIGNAL(released()), this,
            SLOT(CalculatePressed()));

}


// (Destructor) //
Calculator::~Calculator(){
    delete ui;
}


// WHEN NUMBER BUTTONS ARE PRESSED //
void Calculator::NumPressed(){

    QPushButton *button = (QPushButton *)sender();

    int numVal = button->text().toInt();

    if (ui->Display->text() == "0" && numVal == 0.0)
         return;

    if (waitingOp) {
        ui->Display->clear();
        waitingOp = false;
    }

    ui->Display->setText(ui->Display->text() + QString::number(numVal));
}


// WHEN UNARY OPERATORS ARE PRESSED //
void Calculator::UnaryPressed() {

    QPushButton *button = (QPushButton *)sender();
    QString clickedOperator = button->text();

    double operand = ui->Display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("√")) {
        if (operand < 0.0) {
            abortOp();
            return;
        }
        result = sqrt(operand);
    } else if (clickedOperator == tr("^2")) {
        result = operand * operand;
     } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
            abortOp();
            return;
        }
        result = 1.0 / operand;
    }
    ui->Display->setText(QString::number(result));
    waitingOp = true;
}


// WHEN ADDITIVE OPERATORS ARE PRESSED //
void Calculator::AddPressed()
 {

    QPushButton *button = (QPushButton *)sender();
    QString clickedOperator = button->text();

    double operand = ui->Display->text().toDouble();

    if (!MultOp.isEmpty()) {
        if (!calculate(operand, MultOp)) {
            abortOp();
            return;
        }
        ui->Display->setText(QString::number(FactorTillNow));
        operand = FactorTillNow;
        FactorTillNow = 0.0;
        MultOp.clear();
    }

    if (!AddOp.isEmpty()) {
        if (!calculate(operand, AddOp)) {
            abortOp();
            return;
        }
        ui->Display->setText(QString::number(SumTillNow));
    } else {
        SumTillNow = operand;
    }

    AddOp = clickedOperator;
    waitingOp = true;
 }


// WHEN MULTIPLYING OPERATORS ARE PRESSED //
void Calculator::MultPressed()
{

    QPushButton *button = (QPushButton *)sender();
    QString clickedOperator = button->text();

    double operand = ui->Display->text().toDouble();

    if (!MultOp.isEmpty()) {
        if (!calculate(operand, MultOp)) {
            abortOp();
            return;
        }
        ui->Display->setText(QString::number(FactorTillNow));
    } else {
        FactorTillNow = operand;
    }

    MultOp = clickedOperator;
    waitingOp = true;
}


// WHEN DECIMAL POINT BUTTON IS PRESSED //
void Calculator::DecimalPressed()
{
    if (waitingOp)
        ui->Display->setText("0");
    if (!ui->Display->text().contains("."))
        ui->Display->setText(ui->Display->text() + tr("."));
    waitingOp = false;
}


// WHEN +/- BUTTON IS PRESSED //
void Calculator::ChangeSignPressed()
 {
     QString text = ui->Display->text();
     double currVal = text.toDouble();

     if (currVal > 0.0) {
         text.prepend(tr("-"));
     } else if (currVal < 0.0) {
         text.remove(0, 1);
     }
     ui->Display->setText(text);
 }



// WHEN BACKSPACE BUTTON IS PRESSED //
void Calculator::BackSpacePressed()
 {
     if (waitingOp)
         return;

     QString text = ui->Display->text();
     text.chop(1);
     if (text.isEmpty()) {
         text = "0";
         waitingOp = true;
     }
     ui->Display->setText(text);
 }


// WHEN C (CLEAR) BUTTON IS PRESSED //
void Calculator::ClearPressed()
{
    if (waitingOp)
        return;

    ui->Display->setText("0");
    waitingOp = true;
}


// WHEN CE (CLEAR ALL) BUTTON IS PRESSED //
void Calculator::ClearAllPressed()
{
    SumTillNow = 0.0;
    FactorTillNow = 0.0;
    AddOp.clear();
    MultOp.clear();
    ui->Display->setText("0");
    waitingOp = true;
}


// WHEN MC (MEMORY CLEAR) BUTTON IS PRESSED //
void Calculator::MemClearPressed()
{
    MemSum = 0.0;
}


// WHEN MR (MEMORY READ) BUTTON IS PRESSED //
void Calculator::MemReadPressed()
{
    ui->Display->setText(QString::number(MemSum));
    waitingOp = true;
}


// WHEN MS (MEMORY SET) BUTTON IS PRESSED //
void Calculator::SetMemory()
{
    CalculatePressed();
    MemSum = ui->Display->text().toDouble();
}


// WHEN M+ (ADD VALUE TO MEMORY) BUTTON IS PRESSED //
void Calculator::AddMemory()
{
    CalculatePressed();
    MemSum += ui->Display->text().toDouble();
}


// WHEN M- (SUBTRACT VALUE TO MEMORY) BUTTON IS PRESSED //
void Calculator::SubtMemory()
{
    CalculatePressed();
    MemSum -= ui->Display->text().toDouble();
}


// WHEN OPERATION ABORTED //
void Calculator::abortOp()
{
    ClearAllPressed();
    ui->Display->setText(tr("#####"));
}


// WHEN = (CALCULATE) BUTTON IS PRESSED //
void Calculator::CalculatePressed()
 {
     double operand = ui->Display->text().toDouble();

     if (!MultOp.isEmpty()) {
         if (!calculate(operand, MultOp)) {
             abortOp();
             return;
         }
         operand = FactorTillNow;
         FactorTillNow = 0.0;
         MultOp.clear();
     }
     if (!AddOp.isEmpty()) {
         if (!calculate(operand, AddOp)) {
             abortOp();
             return;
         }
         AddOp.clear();
     } else {
         SumTillNow = operand;
     }

     ui->Display->setText(QString::number(SumTillNow));
     SumTillNow = 0.0;
     waitingOp = true;
 }


/********************************************************************************/
bool Calculator::calculate(double RVal, const QString &OpVal)
 {
     if (OpVal == tr("+")) {
         SumTillNow += RVal;
     } else if (OpVal == tr("-")) {
         SumTillNow -= RVal;
     } else if (OpVal == tr("×")) {
         FactorTillNow *= RVal;
     } else if (OpVal == tr("÷")) {
         if (RVal == 0.0)
             return false;
         FactorTillNow /= RVal;
     }
     return true;
 }
/********************************************************************************/
