#include "qloguru_model.hpp"
#include "qloguru_proxy_model.hpp"

QLoguruProxyModel::QLoguruProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterKeyColumn(-1);
}
