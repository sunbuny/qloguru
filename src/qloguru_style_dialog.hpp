#pragma once

#include <QDialog>
#include <optional>

class QLoguruModel;

class QLoguruStyleDialog : public QDialog
{
    Q_OBJECT

public:
    struct Style {
        std::string loggerName;
        std::optional<QColor> backgroundColor;
        std::optional<QColor> textColor;
        bool fontBold;
    };

public:
    explicit QLoguruStyleDialog(QWidget* parent = nullptr);
    ~QLoguruStyleDialog() override;

    Style result() const;
    void setModel(const QLoguruModel* model);

private:
    Style _result;
    const QLoguruModel* _model;
};
