#include "maintaskswidget.h"

MainTasksWidget::MainTasksWidget(QWidget* parent)
    : QTabWidget(parent)
{
    //== Подключение базы данных ==============================================
    m_dataBase = QSqlDatabase::addDatabase("QPSQL");
    m_dataBase.setHostName("127.0.0.1");
    m_dataBase.setDatabaseName("todogoal");
    m_dataBase.setUserName("postgres");
    m_dataBase.setPassword("goalaction");

    if(!m_dataBase.open()){
        qDebug() << "Can*t open database";
    }

    m_pMainModel = new QSqlTableModel(this);
    m_pMainModel->setTable("tasks");
    m_pMainModel->removeColumn(0);

    m_pMainModel->select();

    readTasks();

    //=====================================================
    m_pPendingTasksPart = new CTasksPart(m_pMainModel);
    m_pPendingTasksPart->setFilter("2", 4);

    m_pDoneTasksPart = new CTasksPart(m_pMainModel);
    m_pDoneTasksPart->setFilter("1", 4);

    m_pFailedTasksPart = new CTasksPart(m_pMainModel);
    m_pFailedTasksPart->setFilter("0", 4);

    m_pAllTasksPart = new CTasksPart(m_pMainModel);

    /*== Test ======================*/
    QWidget* w = new QWidget;
    QHBoxLayout* l = new QHBoxLayout;
    QPushButton* btn = new QPushButton("click");
    l->addWidget(btn);
    w->setLayout(l);
    addTab(w, "tav");
    connect(btn, &QPushButton::clicked, this, &MainTasksWidget::testSlot);
    //===============================

    addTab(m_pAllTasksPart, tr("All tasks"));
    addTab(m_pPendingTasksPart, tr("Pending tasks"));
    addTab(m_pDoneTasksPart, tr("Done tasks"));
    addTab(m_pFailedTasksPart, tr("Failed tasks"));
}


void MainTasksWidget::readTasks(){
    QSqlQuery selectQuery;
    QString selectQueryString = "select * from tasks;";
    selectQuery.exec(selectQueryString);

    QSqlRecord rec = selectQuery.record();
    QString task;
    QString responsible;
    QString responsible_email;
    QString deadline;
    int status;
    while (selectQuery.next()){
        task = selectQuery.value(rec.indexOf("task")).toString();
        responsible = selectQuery.value(rec.indexOf("responsible")).toString();
        responsible_email = selectQuery.value(rec.indexOf("responsible_email")).toString();
        deadline = selectQuery.value(rec.indexOf("deadline")).toString();

        Task oneTask(task, responsible, responsible_email, deadline);
        // Генерация нового статуса
        status = Task::generateStatus(oneTask);
        oneTask.status = status;
        m_lslTasks.append(oneTask);

        // Если новый статус отличается от того, что в базе данных
        // и статус в базе данных не "DONE",
        // то делаем перезапись статуса в базе данных
        int statusInDataBase = selectQuery.value(rec.indexOf("status")).toInt();
        if(statusInDataBase != Task::DONE && statusInDataBase != status){
            QSqlQuery updateQuery;
            QString updateQueryString = "update tasks set status = \'%1\' where "
                                        "task = \'%2\' and "
                                        "responsible = \'%3\' and "
                                        "responsible_email = \'%4\' and "
                                        "deadline = \'%5\';";
            bool updateResult = updateQuery.exec(updateQueryString.arg(status)
                                                 .arg(task)
                                                 .arg(responsible)
                                                 .arg(responsible_email)
                                                 .arg(deadline));
            if(!updateResult){
                QMessageBox::critical(this, "Error", "Can*t read tasks!");
                m_lslTasks.clear();
                return;
            }
            m_pMainModel->select();
        }
    }
}


bool MainTasksWidget::isInDataBase(QString task, QString responsible, QString responsible_email, QString deadline){
    auto res = std::find_if(m_lslTasks.begin(), m_lslTasks.end(), [&](const Task& oneTask){
        return task == oneTask.task &&
               responsible == oneTask.responsible &&
               responsible_email == oneTask.responsible_email &&
               deadline == oneTask.deadline;
    });
    if(res != m_lslTasks.end())
        return true;
    return false;
}


void MainTasksWidget::addTaskSlot(){
    NewTaskDialog newTaskDialog;
    if(newTaskDialog.exec() == QDialog::Accepted){
        if(isInDataBase(newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline())){
            QMessageBox::information(this, tr("Dublicate"), tr("The task is already exists"));
            return;
        }
        QSqlQuery insertQuery;
        QString insertQueryString = "insert into tasks(task, responsible, responsible_email, deadline, status)"
                                    "values('%1', '%2', '%3', '%4', '%5');";
        bool insertResult = insertQuery.exec(insertQueryString.arg(newTaskDialog.getDescription())
                                                              .arg(newTaskDialog.getResponsible())
                                                              .arg(newTaskDialog.getEmail())
                                                              .arg(newTaskDialog.getDeadline())
                                                              .arg(Task::generateStatus(Task(newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline())))
                         );
        if(!insertResult){
            QMessageBox::critical(this, tr("Error"), tr("Can*t add new task!"));
            return;
        }
        m_pMainModel->select();
        m_lslTasks.append(Task(newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline()));
    }
}


void MainTasksWidget::removeTaskSlot(){
    CTasksPart* currentPart = static_cast<CTasksPart*>(currentWidget());
    QTableView* curentView = currentPart->getView();
    QSortFilterProxyModel* currentSortModel = currentPart->getSortModel();

    QItemSelectionModel *selectionModel = curentView->selectionModel();

    const QModelIndexList indexes = selectionModel->selectedRows();

    for(QModelIndex index : indexes){
        QString task = currentSortModel->data(index).toString();
        QString responsible = currentSortModel->data(index.sibling(index.row(), index.column() + 1)).toString();
        QString responsible_email = currentSortModel->data(index.sibling(index.row(), index.column() + 2)).toString();
        QString deadline = currentSortModel->data(index.sibling(index.row(), index.column() + 3)).toString();
        QSqlQuery deleteQuery;
        QString deleteQueryString = "delete from tasks where task = \'" +
                                    task + "\' and responsible = \'" +
                                    responsible + "\' and responsible_email = \'" +
                                    responsible_email + "\' and deadline = \'" +
                                    deadline + "\';";
        bool deleteResult = deleteQuery.exec(deleteQueryString);
        if(!deleteResult){
            QMessageBox::critical(this, tr("Error"), tr("Can*t delete a task!"));
            return;
        }
        m_pMainModel->select();

        auto res = std::find_if(m_lslTasks.begin(), m_lslTasks.end(), [&](const Task& second){
             return task == second.task &&
                    responsible == second.responsible &&
                    responsible_email == second.responsible_email &&
                    deadline == second.deadline;
        });
        if(res != m_lslTasks.end())
            m_lslTasks.erase(res);
        else{
            QMessageBox::critical(this, tr("Error"), tr("No such task!"));
            return;
        }
    }
}


void MainTasksWidget::editTaskSlot(){
    CTasksPart* currentPart = static_cast<CTasksPart*>(currentWidget());
    QTableView* curentView = currentPart->getView();
    QSortFilterProxyModel* currentSortModel = currentPart->getSortModel();

    QItemSelectionModel *selectionModel = curentView->selectionModel();

    const QModelIndexList indexes = selectionModel->selectedRows();
    for(QModelIndex index : indexes){
        QString task = currentSortModel->data(index).toString();
        QString responsible = currentSortModel->data(index.sibling(index.row(), index.column() + 1)).toString();
        QString responsible_email = currentSortModel->data(index.sibling(index.row(), index.column() + 2)).toString();
        QString deadline = currentSortModel->data(index.sibling(index.row(), index.column() + 3)).toString();

        NewTaskDialog newTaskDialog;
        newTaskDialog.setDialog(task, responsible, responsible_email, deadline);
        if(newTaskDialog.exec() == QDialog::Accepted){
            if(!isInDataBase(newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline())){
                int row = index.row();
                m_lslTasks.replace(row, {newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline()});

                //Проверка новой даты и присвоение значения для newStatus
                int year = deadline.mid(0, deadline.indexOf('-')).toInt();
                int month = deadline.mid(deadline.indexOf('-'), 2).toInt();
                int day = deadline.mid(deadline.lastIndexOf('-') + 1, 2).toInt();
                int newStatus;
                if(QDate(year, month, day) < QDate::currentDate()){
                    newStatus = Task::FAILED;
                }else{
                    newStatus = Task::PENDING;
                }

                QSqlQuery updateQuery;
                QString upfateQueryString = "update tasks set "
                                            "status = \'%1\', "
                                            "task = \'%2\', "
                                            "responsible = \'%3\', "
                                            "responsible_email = \'%4\', "
                                            "deadline = \'%5\' where "
                                            "task = \'%6\' and "
                                            "responsible = \'%7\' and "
                                            "responsible_email = \'%8\' and "
                                            "deadline = \'%9\';";
                bool updateResult = updateQuery.exec(upfateQueryString.arg(QString().number(newStatus))
                                                                      .arg(newTaskDialog.getDescription())
                                                                      .arg(newTaskDialog.getResponsible())
                                                                      .arg(newTaskDialog.getEmail())
                                                                      .arg(newTaskDialog.getDeadline())
                                                                      .arg(task)
                                                                      .arg(responsible)
                                                                      .arg(responsible_email)
                                                                      .arg(deadline));
                if(!updateResult){
                    QMessageBox::critical(this, tr("Error"), tr("Can*t update database!"));
                    return;
                }
                m_pMainModel->select();
            }else{
                QMessageBox::information(this, tr("Dublicate"), tr("The task is already exists"));
            }
        }
    }
}


void MainTasksWidget::markAsDoneSlot(){
    CTasksPart* currentPart = static_cast<CTasksPart*>(currentWidget());
    QTableView* curentView = currentPart->getView();
    QSortFilterProxyModel* currentSortModel = currentPart->getSortModel();

    QItemSelectionModel *selectionModel = curentView->selectionModel();

    const QModelIndexList indexes = selectionModel->selectedRows();

    for(QModelIndex index : indexes){
        QString task = currentSortModel->data(index).toString();
        QString responsible = currentSortModel->data(index.sibling(index.row(), index.column() + 1)).toString();
        QString responsible_email = currentSortModel->data(index.sibling(index.row(), index.column() + 2)).toString();
        QString deadline = currentSortModel->data(index.sibling(index.row(), index.column() + 3)).toString();
        QSqlQuery updateQuery;
        QString updateQueryString = "update tasks set status = " +
                                    QString().number(Task::DONE) + " where " +
                                    "task = '%1' and "
                                    "responsible = '%2' and "
                                    "responsible_email = '%3' and "
                                    "deadline = '%4';";
        bool updateResult = updateQuery.exec(updateQueryString.arg(task)
                                                              .arg(responsible)
                                                              .arg(responsible_email)
                                                              .arg(deadline)
                                             );
        if(!updateResult){
            QMessageBox::critical(this, tr("Error"), tr("Can*t update a task!"));
            return;
        }
        int row = index.row();
        m_lslTasks.replace(row, {task, responsible, responsible_email, deadline, Task::DONE});
        m_pMainModel->select();
    }
}


void MainTasksWidget::testSlot(){
    for(const auto& val : m_lslTasks){
        val.print();
    }
}
