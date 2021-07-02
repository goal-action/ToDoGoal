#ifndef TODOGOAL_H
#define TODOGOAL_H

#include <QMainWindow>
#include <QIcon>

#include "maintaskswidget.h"

class ToDoGoal : public QMainWindow
{
    Q_OBJECT
private:
    MainTasksWidget* m_pMainTasksWidget;
    QToolBar* m_pToolBar;
    QAction* m_pAddAction;
    QAction* m_pRemoveAction;
    QAction* m_pEditAction;
    QAction* m_pMarkAsDoneAction;
    QTranslator m_translator;

    QMenu* m_pSettings;
    QMenu* m_pSettingsLangsMenu;

public:
    ToDoGoal(QWidget *parent = nullptr);
    ~ToDoGoal();

    void retranslateUi();
    void changeEvent(QEvent *event);


signals:
    void changeToEngishSignal();
    void changeToGermanSignal();
    void changeToUkrainianSignal();
    void changeToRussianSignal();

public slots:
    //Слоты для установления языка
    void setEnglishLanguageSlot();
    void setGermanLanguageSlot();
    void setUkrainianLanguageSlot();
    void setRussianLanguageSlot();
};
#endif // TODOGOAL_H
