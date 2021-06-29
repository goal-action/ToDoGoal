#include "todogoal.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ToDoGoal w;
    w.show();

    return a.exec();
}
