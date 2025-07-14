#include <qloguru/qabstract_loguru_toolbar.hpp>
#include <qloguru/qloguru.hpp>

QAbstractLoguruToolBar::~QAbstractLoguruToolBar()
{
    if (_parent) {
        _parent->removeToolbar(this);
        _parent = nullptr;
    }
}

void QAbstractLoguruToolBar::setParent(QLoguru* parent) { _parent = parent; }
