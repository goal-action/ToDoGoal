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

//== NewTaskDialog - диалоговое окно для добавления новой задачи =================
class NewTaskDialog : public QDialog{
    Q_OBJECT
protected:
    QTextEdit* description;
    QLineEdit* responsibleLine;
    QLineEdit* emailLine;
    QLineEdit* deadlineLine;
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

        QLabel* deadlineLabel = new QLabel(tr("Deadline:"), this);
        deadlineLine = new QLineEdit(this);

        okBtn = new QPushButton(tr("ok"));
        cancelBtn = new QPushButton(tr("cancel"));


        QHBoxLayout* buttonsLayout = new QHBoxLayout;
        buttonsLayout->addWidget(cancelBtn);
        buttonsLayout->addWidget(okBtn);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(descriptionLabel);
        mainLayout->addWidget(description);
        mainLayout->addWidget(responsibleLabel);
        mainLayout->addWidget(responsibleLine);
        mainLayout->addWidget(emailLabel);
        mainLayout->addWidget(emailLine);
        mainLayout->addWidget(deadlineLabel);
        mainLayout->addWidget(deadlineLine);
        mainLayout->addLayout(buttonsLayout);

        setLayout(mainLayout);

        connect(okBtn, SIGNAL(clicked()), SLOT(accept()));
        connect(cancelBtn, SIGNAL(clicked()), SLOT(reject()));

        setWindowTitle(tr("New task"));
    }

    QString getDescription(){return description->toPlainText();}
    QString getResponsible(){return responsibleLine->text();}
    QString getEmail(){return emailLine->text();}
    QString getDeadline() {return deadlineLine->text();}

    void setDialog(QString task, QString responsible, QString responsible_email, QString deadline){
        description->setText(task);
        responsibleLine->setText(responsible);
        emailLine->setText(responsible_email);
        deadlineLine->setText(deadline);
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
    QTranslator m_translator;

    bool isInDataBase(QString task, QString responsible, QString responsible_email, QString date);

public:
    void retranslateUi();
    void changeEvent(QEvent *event);

public slots:
    void addTaskSlot();
    void removeTaskSlot();
    void editTaskSlot();
    void markAsDoneSlot();

    //Слоты для установления языка
    void setEnglishLanguageSlot();
    void setGermanLanguageSlot();
    void setUkrainianLanguageSlot();
    void setRussianLanguageSlot();
};

#endif // MAINTASKSWIDGET_H
