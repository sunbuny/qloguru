#pragma once

#include <QSortFilterProxyModel>

class QLoguruProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    QLoguruProxyModel(QObject* parent = nullptr);
};
