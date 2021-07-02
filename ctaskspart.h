#ifndef CTASKSPART_H
#define CTASKSPART_H

#include <QWidget>
#include <QtCore>
#include <QtWidgets>

class CTasksPart : public QWidget
{
    Q_OBJECT
public:
    explicit CTasksPart(QAbstractItemModel* mainModel, QWidget *parent = nullptr);

protected:
    QSortFilterProxyModel* m_pSortModel;
    QTableView* m_pView;

public:
    QSortFilterProxyModel* getSortModel(){return m_pSortModel;}
    QTableView* getView(){return m_pView;}

    void setFilter(const QString pattern, int column);
    void deleteRow(const int row);
    void setHeaderData();

signals:

};

#endif // CTASKSPART_H
