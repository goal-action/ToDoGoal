#include "todogoal.h"

ToDoGoal::ToDoGoal(QWidget *parent)
    : QMainWindow(parent)
{
    m_pMainTasksWidget = new MainTasksWidget();
    setCentralWidget(m_pMainTasksWidget);

    m_pAddAction = new QAction(this);
    m_pAddAction->setIcon(QIcon(":/icons/add.png"));

    m_pRemoveAction = new QAction(this);
    m_pRemoveAction->setIcon(QIcon(":/icons/remove.png"));

    m_pEditAction = new QAction(this);
    m_pEditAction->setIcon(QIcon(":/icons/edit.png"));

    m_pToolBar = new QToolBar("title", this);
    m_pToolBar->addAction(m_pAddAction);
    m_pToolBar->addAction(m_pRemoveAction);
    m_pToolBar->addAction(m_pEditAction);

    addToolBar(m_pToolBar);

    connect(m_pAddAction,    &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::addTaskSlot);
    connect(m_pRemoveAction, &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::removeTaskSlot);
    connect(m_pEditAction,   &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::editTaskSlot);
}

ToDoGoal::~ToDoGoal()
{
}

