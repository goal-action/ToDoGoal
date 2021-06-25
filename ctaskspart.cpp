#include "ctaskspart.h"

CTasksPart::CTasksPart(QAbstractItemModel* mainModel ,QWidget *parent)
    : QWidget(parent)
{
    m_pSortModel = new QSortFilterProxyModel(this);
    m_pSortModel->setSourceModel(mainModel);

    m_pView = new QTableView(this);
    m_pView->setModel(m_pSortModel);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pView);
    setLayout(mainLayout);
}
