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

    m_pDoneTasksPart = new CTasksPart(m_pMainModel);

    m_pFailedTasksPart = new CTasksPart(m_pMainModel);

    m_pAllTasksPart = new CTasksPart(m_pMainModel);

    addTab(m_pPendingTasksPart, tr("Pending tasks"));
    addTab(m_pDoneTasksPart, tr("Done tasks"));
    addTab(m_pFailedTasksPart, tr("Failed tasks"));
    addTab(m_pAllTasksPart, tr("All tasks"));
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
    while (selectQuery.next()){
        task = selectQuery.value(rec.indexOf("task")).toString();
        responsible = selectQuery.value(rec.indexOf("responsible")).toString();
        responsible_email = selectQuery.value(rec.indexOf("responsible_email")).toString();
        deadline = selectQuery.value(rec.indexOf("deadline")).toString();
        m_lslTasks.append(Task(task, responsible, responsible_email, deadline));
    }
}



bool MainTasksWidget::isInDataBase(QString task, QString responsible, QString responsible_email, QString deadline){
    auto res = std::find(m_lslTasks.begin(), m_lslTasks.end(), Task(task, responsible, responsible_email, deadline));
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
        QString insertQueryString = "insert into tasks(task, responsible, responsible_email, deadline)"
                                    "values('%1', '%2', '%3', '%4');";
        bool insertResult = insertQuery.exec(insertQueryString.arg(newTaskDialog.getDescription())
                                                              .arg(newTaskDialog.getResponsible())
                                                              .arg(newTaskDialog.getEmail())
                                                              .arg(newTaskDialog.getDeadline())
                         );
        if(insertResult)
            m_pMainModel->select();
        else
            qDebug() << "Can*t add new Task";
    }
}

void MainTasksWidget::removeTaskSlot(){
    CTasksPart* currentPart = static_cast<CTasksPart*>(currentWidget());
    QTableView* curentView = currentPart->getView();

    QItemSelectionModel *selectionModel = curentView->selectionModel();

    const QModelIndexList indexes = selectionModel->selectedRows();

    for(QModelIndex index : indexes){
        QString task = m_pMainModel->data(index).toString();
        QString responsible = m_pMainModel->data(index.sibling(index.row(), index.column() + 1)).toString();
        QString responsible_email = m_pMainModel->data(index.sibling(index.row(), index.column() + 2)).toString();
        QString deadline = m_pMainModel->data(index.sibling(index.row(), index.column() + 3)).toString();
        QSqlQuery deleteQuery;
        QString deleteQueryString = "delete from tasks where task = \'" +
                                    task + "\' and responsible = \'" +
                                    responsible + "\' and responsible_email = \'" +
                                    responsible_email + "\' and deadline = \'" +
                                    deadline + "\';";
        bool deleteResult = deleteQuery.exec(deleteQueryString);
        if(!deleteResult){
            qDebug() << "Can*t delete task";
            return;
        }
        m_pMainModel->select();
    }
}

void MainTasksWidget::editTaskSlot(){
    CTasksPart* currentPart = static_cast<CTasksPart*>(currentWidget());
    QTableView* curentView = currentPart->getView();
    QSortFilterProxyModel* currentSortModel = currentPart->getSortModel();

    QItemSelectionModel *selectionModel = curentView->selectionModel();

    const QModelIndexList indexes = selectionModel->selectedRows();
    for(QModelIndex index : indexes){
        //int row = index.row();
        QString task = currentSortModel->data(index).toString();
        QString responsible = currentSortModel->data(index.sibling(index.row(), index.column() + 1)).toString();
        QString responsible_email = currentSortModel->data(index.sibling(index.row(), index.column() + 2)).toString();
        QString deadline = currentSortModel->data(index.sibling(index.row(), index.column() + 3)).toString();

        NewTaskDialog newTaskDialog;
        newTaskDialog.setDialog(task, responsible, responsible_email, deadline);
        if(newTaskDialog.exec() == QDialog::Accepted){
            int row = index.row();
            m_lslTasks.replace(row, {newTaskDialog.getDescription(), newTaskDialog.getResponsible(), newTaskDialog.getEmail(), newTaskDialog.getDeadline()});

            QSqlQuery updateQuery;
            QString upfateQueryString = "update tasks set task = \'%1\', "
                                        "responsible = \'%2\', "
                                        "responsible_email = \'%3\', "
                                        "deadline = \'%4\' where "
                                        "task = \'%5\' and "
                                        "responsible = \'%6\' and "
                                        "responsible_email = \'%7\' and "
                                        "deadline = \'%8\';";
            bool updateResult = updateQuery.exec(upfateQueryString.arg(newTaskDialog.getDescription())
                                                                  .arg(newTaskDialog.getResponsible())
                                                                  .arg(newTaskDialog.getEmail())
                                                                  .arg(newTaskDialog.getDeadline())
                                                                  .arg(task)
                                                                  .arg(responsible)
                                                                  .arg(responsible_email)
                                                                  .arg(deadline));
            if(!updateResult){
                qDebug("can*t update database");
                return;
            }
            m_pMainModel->select();
        }
    }
}

