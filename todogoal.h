#ifndef TODOGOAL_H
#define TODOGOAL_H

#include <QMainWindow>
#include <QIcon>

#include "maintaskswidget.h"

class ToDoGoal : public QMainWindow
{
    Q_OBJECT

public:
    ToDoGoal(QWidget *parent = nullptr);
    ~ToDoGoal();

private:
    MainTasksWidget* m_pMainTasksWidget;
    QToolBar* m_pToolBar;
    QAction* m_pAddAction;
    QAction* m_pRemoveAction;
    QAction* m_pEditAction;
    QAction* m_pMarkAsDoneAction;

    QMenu* m_pSettings;
    QMenu* m_pSettingsLangsMenu;

signals:
    void changeToEngishSignal();
    void changeToGermanSignal();
    void changeToUkrainianSignal();
    void changeToRussianSignal();
};
#endif // TODOGOAL_H
