#ifndef MAINTASKSWIDGET_H
#define MAINTASKSWIDGET_H

#include <QtWidgets>
#include <QtCore>
#include <QtSql>
#include <algorithm>

#include "ctaskspart.h"

//== Task - структура, которая представляет задачу ===============================
struct Task{
    enum{NONE = -1, FAILED, DONE, PENDING};

    QString task;
    QString responsible;
    QString responsible_email;
    QString deadline;
    int status;
    Task(QString task, QString responsible, QString responsible_email, QString deadline, int status = NONE){
        this->task = task;
        this->responsible = responsible;
        this->responsible_email = responsible_email;
        this->deadline = deadline;
        this->status = status;
    }

    static int generateStatus(Task task){
        QString deadlineString = task.deadline;
        int year = deadlineString.mid(0, deadlineString.indexOf('-')).toInt();
        int month = deadlineString.mid(deadlineString.indexOf('-') + 1, 2).toInt();
        int day = deadlineString.mid(deadlineString.lastIndexOf('-') + 1, 2).toInt();
        if(QDate::currentDate() < QDate(year, month, day))
            return PENDING;
        return FAILED;
    }

    bool operator==(const Task& second){
        return task              == second.task &&
               responsible       == second.responsible &&
               responsible_email == second.responsible_email &&
               deadline          == second.deadline;
    }
    void print() const{
        qDebug() << task << responsible << responsible_email << deadline;
    }
};


class DayValidator : public QValidator{
    Q_OBJECT
public:
    DayValidator(QObject* parent = nullptr) : QValidator{parent}{}
    virtual State validate(QString& str, int& pos) const override{
        if(pos == 0 && str.isEmpty()){
            emit emptyDayEditLineSignal();
            return Intermediate;
        }else if(pos == 1 && str.contains(QRegularExpression("^[1-9]$"))){
            emit validDaySignal();
            return Acceptable;
        }else if(pos == 2 && str.contains(QRegularExpression("^[1-2][0-9]$"))){
            emit validDaySignal();
            return Acceptable;
        }else if(pos == 2 && str.contains(QRegularExpression("^3[0-1]$"))){
            emit validDaySignal();
            return Acceptable;
        }
        emit invalidDaySignal();
        return Intermediate;
    }

signals:
    void invalidDaySignal() const;
    void   validDaySignal() const;
    void emptyDayEditLineSignal() const;

};


class MonthValidator : public QValidator{
    Q_OBJECT
public:
    MonthValidator(QObject* parent = nullptr) : QValidator{parent}{}
    virtual State validate(QString& str, int& pos) const override{
        if(pos == 0 && str.isEmpty()){
            emit emptyMonthEditLineSignal();
            return Intermediate;
        }else if(pos == 1 && str.contains(QRegularExpression("^[1-9]$"))){
            emit validMonthSignal();
            return Acceptable;
        }else if(pos == 2 && str.contains(QRegularExpression("^1[0-2]$"))){
            emit validMonthSignal();
            return Acceptable;
        }
        emit invalidMonthSignal();
        return Intermediate;
    }

signals:
    void invalidMonthSignal() const;
    void   validMonthSignal() const;
    void emptyMonthEditLineSignal() const;

};


class YearValidator : public QValidator{
    Q_OBJECT
public:
    YearValidator(QObject* parent = nullptr) : QValidator{parent}{}
    virtual State validate(QString& str, int& pos) const override{
        if(pos == 0 && str.isEmpty()){
            emit emptyYearEditLineSignal();
            return Intermediate;
        }else if(str.contains(QRegularExpression("^2[0-1][0-9][0-9]$"))){
            emit validYearSignal();
            return Acceptable;
        }
        emit invalidYearSignal();
        return Intermediate;
    }

signals:
    void invalidYearSignal() const;
    void   validYearSignal() const;
    void emptyYearEditLineSignal() const;

};


class EmailValidator : public QValidator{
    Q_OBJECT
public:
    EmailValidator(QObject* parent = nullptr) : QValidator{parent}{}
    virtual State validate(QString& str, int& pos) const override{
        if(pos == 0 && str.isEmpty()){
            emit emptyGmailEditLineSignal();
            return Intermediate;
        }else if(str.contains(QRegularExpression("^[A-Za-z0-9]+([A-Za-z0-9._-]+)*[A-Za-z0-9]@[A-Za-z0-9]+\.[A-Za-z]{2,4}$"))){
            emit validGmailSignal();
            return Acceptable;
        }
        emit invalidGmailSignal();
        return Intermediate;
    }

signals:
    void invalidGmailSignal() const;
    void   validGmailSignal() const;
    void emptyGmailEditLineSignal() const;

};



//== NewTaskDialog - диалоговое окно для добавления новой задачи =================
class NewTaskDialog : public QDialog{
    Q_OBJECT
protected:
    QTextEdit* description;
    QLineEdit* responsibleLine;
    QLineEdit* emailLine;

    //QLineEdit* deadlineLine;
    QLineEdit* deadlineDayLine;
    QLineEdit* deadlineMonthLine;
    QLineEdit* deadlineYearLine;

    //Надписm для установки сообщений о некоректности ввода
    QLabel* dateStateLabel;
    QLabel* emailStateLabel;

    //Buttons
    QPushButton* okBtn;
    QPushButton* cancelBtn;

public:
    NewTaskDialog(QWidget* parent = nullptr)
        :QDialog(parent)
    {
        QLabel* descriptionLabel = new QLabel(tr("Description:"), this);
        description = new QTextEdit(this);

        QLabel* responsibleLabel = new QLabel(tr("Responsible (Name & Surname):"), this);
        responsibleLine = new QLineEdit(this);

        QLabel* emailLabel = new QLabel(tr("Email:"), this);
        emailLine = new QLineEdit(this);
        EmailValidator* emailValidator = new EmailValidator;
        emailLine->setValidator(emailValidator);

        QLabel* deadlineLabel = new QLabel(tr("Deadline (dd-mm-yyyy):"), this);
        deadlineDayLine   = new QLineEdit(this);
        deadlineMonthLine = new QLineEdit(this);
        deadlineYearLine  = new QLineEdit(this);
        DayValidator*   dayValidator   = new DayValidator;
        MonthValidator* monthValidator = new MonthValidator;
        YearValidator*  yearValidator  = new YearValidator;
        deadlineDayLine->setValidator(dayValidator);
        deadlineMonthLine->setValidator(monthValidator);
        deadlineYearLine->setValidator(yearValidator);

        QLabel* lbl1    = new QLabel("-", this);
        QLabel* lbl2    = new QLabel("-", this);
        dateStateLabel  = new QLabel(this);
        emailStateLabel = new QLabel(this);

        okBtn     = new QPushButton(tr("ok"));
        cancelBtn = new QPushButton(tr("cancel"));

        //Layout
        QVBoxLayout* mainLayout = new QVBoxLayout;

        mainLayout->addWidget(descriptionLabel);
        mainLayout->addWidget(description);
        mainLayout->addSpacing(10);

        mainLayout->addWidget(responsibleLabel);
        mainLayout->addWidget(responsibleLine);
        mainLayout->addSpacing(10);

        QHBoxLayout* emailLabelsLayout = new QHBoxLayout;
        emailLabelsLayout->addWidget(emailLabel);
        emailLabelsLayout->addWidget(emailStateLabel);
        mainLayout->addLayout(emailLabelsLayout);
        mainLayout->addWidget(emailLine);
        mainLayout->addSpacing(10);

        QHBoxLayout* deadlineLabelsLayout = new QHBoxLayout;
        deadlineLabelsLayout->addWidget(deadlineLabel);
        deadlineLabelsLayout->addWidget(dateStateLabel);
        QHBoxLayout* deadLineLayout = new QHBoxLayout;
        deadLineLayout->addWidget(deadlineDayLine);
        deadLineLayout->addWidget(lbl1);
        deadLineLayout->addWidget(deadlineMonthLine);
        deadLineLayout->addWidget(lbl2);
        deadLineLayout->addWidget(deadlineYearLine);
        mainLayout->addLayout(deadlineLabelsLayout);
        mainLayout->addLayout(deadLineLayout);

        QHBoxLayout* buttonsLayout = new QHBoxLayout;
        buttonsLayout->addWidget(cancelBtn);
        buttonsLayout->addWidget(okBtn);
        mainLayout->addLayout(buttonsLayout);

        setLayout(mainLayout);

        //connect*ы
        connect(emailValidator,   SIGNAL(invalidGmailSignal()),        SLOT(invalidGmailSlot()));
        connect(emailValidator,   SIGNAL(validGmailSignal()),          SLOT(validGmailSlot()));
        connect(emailValidator,   SIGNAL(emptyGmailEditLineSignal()),  SLOT(invalidGmailSlot()));
        connect(dayValidator,     SIGNAL(invalidDaySignal()),          SLOT(invalidDateSlot()));
        connect(dayValidator,     SIGNAL(validDaySignal()),            SLOT(validDateSlot()));
        connect(dayValidator,     SIGNAL(emptyDayEditLineSignal()),    SLOT(invalidDateSlot()));
        connect(monthValidator,   SIGNAL(invalidMonthSignal()),        SLOT(invalidDateSlot()));
        connect(monthValidator,   SIGNAL(validMonthSignal()),          SLOT(validDateSlot()));
        connect(monthValidator,   SIGNAL(emptyMonthEditLineSignal()),  SLOT(invalidDateSlot()));
        connect(yearValidator,    SIGNAL(invalidYearSignal()),         SLOT(invalidDateSlot()));
        connect(yearValidator,    SIGNAL(validYearSignal()),           SLOT(validDateSlot()));
        connect(yearValidator,    SIGNAL(emptyYearEditLineSignal()),   SLOT(invalidDateSlot()));

        connect(okBtn,     SIGNAL(clicked()), SLOT(accept()));
        connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

        setWindowTitle(tr("New task"));
    }

    QString getDescription(){return description->toPlainText();}
    QString getResponsible(){return responsibleLine->text();}
    QString getEmail(){return emailLine->text();}
    QString getDeadline() {
        QString day = deadlineDayLine->text();
        if(day.count() == 1)
            day.push_front('0');
        QString month = deadlineMonthLine->text();
        if(month.count() == 1)
            month.push_front('0');
        QString year = deadlineYearLine->text();

        return year + "-" + month + "-" + day;
    }

    void setDialog(QString task, QString responsible, QString responsible_email, QString deadline){
        description->setText(task);
        responsibleLine->setText(responsible);
        emailLine->setText(responsible_email);
        //deadline
        deadlineYearLine->setText(deadline.mid(0, deadline.indexOf('-')));
        deadlineMonthLine->setText(deadline.mid(deadline.indexOf('-') + 1, 2));
        deadlineDayLine->setText(deadline.mid(deadline.lastIndexOf('-') + 1));
    }

protected slots:
    void invalidGmailSlot(){
        emailStateLabel->setText(tr("Invalid gmail"));
        emailStateLabel->setStyleSheet("color: red; font-weight: bold;");
        okBtn->setDisabled(true);
    }
    void validGmailSlot(){
        emailStateLabel->setText(tr("valid gmail"));
        emailStateLabel->setStyleSheet("color: green; font-weight: bold;");
        okBtn->setDisabled(false);
    }
    void invalidDateSlot(){
        dateStateLabel->setText(tr("Invalid date"));
        dateStateLabel->setStyleSheet("color: red; font-weight: bold;");
        okBtn->setDisabled(true);
    }
    void validDateSlot(){
        dateStateLabel->setText(tr("Valid date"));
        dateStateLabel->setStyleSheet("color: green; font-weight: bold;");
        okBtn->setDisabled(false);
    }
};


//== MainTasksWidget - центральный виджет программы ==============================
class MainTasksWidget : public QTabWidget
{
    Q_OBJECT
public:
    MainTasksWidget(QWidget* parent = nullptr);

    QList<Task>& getTasks(){return m_lslTasks;}

private:
    void readTasks();

protected:
    QList<Task> m_lslTasks;
    QSqlDatabase m_dataBase;
    QSqlTableModel* m_pMainModel;
    CTasksPart* m_pPendingTasksPart;
    CTasksPart* m_pDoneTasksPart;
    CTasksPart* m_pFailedTasksPart;
    CTasksPart* m_pAllTasksPart;

    bool isInDataBase(QString task, QString responsible, QString responsible_email, QString date);

public:
    void retranslateUi();

public slots:
    void addTaskSlot();
    void removeTaskSlot();
    void editTaskSlot();
    void markAsDoneSlot();
};

#endif // MAINTASKSWIDGET_H
