#pragma once

#include <QToolBar>

#include "qloguru/qabstract_loguru_toolbar.hpp"

class QWidget;
class QAction;
class QCompleter;
class QAbstractItemModel;
class QSettings;

class QLoguruToolBar
    : public QToolBar
    , public QAbstractLoguruToolBar
{
    Q_OBJECT

public:
    struct FilteringSettings {
        QString text;             // The text to filter by.
        bool isRegularExpression; // Whether the text is a regular expression.
        bool isCaseSensitive;     // Whether the filtering is case sensitive.
    };

public:
    QLoguruToolBar(QWidget* parent = nullptr);
    ~QLoguruToolBar();

#pragma region QAbstractLoguruToolBar
    QLineEdit* filter() override;
    QAction* caseSensitive() override;
    QAction* regex() override;
    QAction* clearHistory() override;
    QAction* style() override;
    QComboBox* autoScrollPolicy() override;
#pragma endregion

    FilteringSettings filteringSettings() const;
    void checkInputValidity();
    void clearCompleterHistory();

signals:
    void styleChangeRequested();
    void filterChanged();
    void autoScrollPolicyChanged(int index);

private:
    void loadCompleterHistory();
    void saveCompleterHistory();

private:
    QWidget* _filterWidget;
    QAction* _caseAction;
    QAction* _regexAction;
    QAction* _clearHistory;
    QAction* _styleAction;
    QComboBox* _autoScrollPolicy;
    QAbstractItemModel* _completerData;
    QCompleter* _completer;
};
