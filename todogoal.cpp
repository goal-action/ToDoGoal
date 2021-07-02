#include "todogoal.h"

ToDoGoal::ToDoGoal(QWidget *parent)
    : QMainWindow(parent)
{
    //Центральный виджет
    m_pMainTasksWidget = new MainTasksWidget(this);
    setCentralWidget(m_pMainTasksWidget);

    //Создание ToolBar'а
    m_pAddAction = new QAction(this);
    m_pAddAction->setIcon(QIcon(":/icons/add.png"));

    m_pRemoveAction = new QAction(this);
    m_pRemoveAction->setIcon(QIcon(":/icons/remove.png"));

    m_pEditAction = new QAction(this);
    m_pEditAction->setIcon(QIcon(":/icons/edit.png"));

    m_pMarkAsDoneAction = new QAction(this);
    m_pMarkAsDoneAction->setIcon(QIcon(":/icons/done.png"));

    m_pToolBar = new QToolBar("title", this);
    m_pToolBar->addAction(m_pAddAction);
    m_pToolBar->addAction(m_pRemoveAction);
    m_pToolBar->addAction(m_pEditAction);
    m_pToolBar->addAction(m_pMarkAsDoneAction);
    addToolBar(m_pToolBar);

    //Создание меню
    QMenuBar* menuBar = new QMenuBar(this);
    m_pSettings = new QMenu(this);
    m_pSettingsLangsMenu = new QMenu(this);
    m_pSettingsLangsMenu->addAction("English", this, &ToDoGoal::changeToEngishSignal);
    m_pSettingsLangsMenu->addAction("Deutsche", this, &ToDoGoal::changeToGermanSignal);
    m_pSettingsLangsMenu->addAction("Українська", this, &ToDoGoal::changeToUkrainianSignal);
    m_pSettingsLangsMenu->addAction("Русский", this, &ToDoGoal::changeToRussianSignal);
    m_pSettings->addMenu(m_pSettingsLangsMenu);
    menuBar->addMenu(m_pSettings);
    setMenuBar(menuBar);

    //Установка всех надписей
    retranslateUi();

    //конекты сигналов и слотов
    connect(m_pAddAction,        &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::addTaskSlot);
    connect(m_pRemoveAction,     &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::removeTaskSlot);
    connect(m_pEditAction,       &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::editTaskSlot);
    connect(m_pMarkAsDoneAction, &QAction::triggered, m_pMainTasksWidget, &MainTasksWidget::markAsDoneSlot);

    connect(this, &ToDoGoal::changeToEngishSignal,    this, &ToDoGoal::setEnglishLanguageSlot);
    connect(this, &ToDoGoal::changeToGermanSignal,    this, &ToDoGoal::setGermanLanguageSlot);
    connect(this, &ToDoGoal::changeToUkrainianSignal, this, &ToDoGoal::setUkrainianLanguageSlot);
    connect(this, &ToDoGoal::changeToRussianSignal,   this, &ToDoGoal::setRussianLanguageSlot);
}


void ToDoGoal::retranslateUi(){
    m_pSettings->setTitle(tr("Settings"));
    m_pSettingsLangsMenu->setTitle(tr("Language"));
    m_pMainTasksWidget->retranslateUi();
}


void ToDoGoal::changeEvent(QEvent *event){
     //В случае получения события изменения языка приложения
     if (event->type() == QEvent::LanguageChange) {
         retranslateUi();    // переведём окно заново
     }
}


void ToDoGoal::setEnglishLanguageSlot(){
    m_translator.load(":/ToDoGoal_en.qm");
    qApp->installTranslator(&m_translator);
}

void ToDoGoal::setGermanLanguageSlot(){
    m_translator.load(":/ToDoGoal_de.qm");
    qApp->installTranslator(&m_translator);
}

void ToDoGoal::setUkrainianLanguageSlot(){
    m_translator.load(":/ToDoGoal_ua.qm");
    qApp->installTranslator(&m_translator);
}

void ToDoGoal::setRussianLanguageSlot(){
    m_translator.load(":/ToDoGoal_ru.qm");
    qApp->installTranslator(&m_translator);
}


ToDoGoal::~ToDoGoal()
{
}

