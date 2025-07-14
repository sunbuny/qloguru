#pragma once

class QLineEdit;
class QAction;
class QComboBox;
class QLoguru;

class QAbstractLoguruToolBar
{
public:
    /**
     * @brief Constructor
     */
    explicit QAbstractLoguruToolBar() = default;

    /**
     * @brief Destructor
     */
    virtual ~QAbstractLoguruToolBar();

public:
    /**
     * @brief Set the parent.
     *
     * @param parent the parent
     */
    void setParent(QLoguru* parent);

    /**
     * @brief Get the filter text editing widget.
     *
     * The widget is used to filter the messages in the widget.
     *
     * @return QLineEdit* the filter text editing widget
     */
    virtual QLineEdit* filter() = 0;

    /**
     * @brief Get the case sensitive action.
     *
     * The action is used to toggle the case sensitivity of the filtering.
     *
     * @return QAction* the case sensitive action
     */
    virtual QAction* caseSensitive() = 0;

    /**
     * @brief Get the regular expression action.
     *
     * The action is used to toggle the regular expression mode of the
     * filtering.
     *
     * @return QAction* the regular expression action
     */
    virtual QAction* regex() = 0;

    /**
     * @brief Get the clear history action.
     *
     * The action is used to clear the history of the filter.
     *
     * @return QAction* the clear history action
     */
    virtual QAction* clearHistory() = 0;

    /**
     * @brief Get the style action.
     *
     * The action is used to toggle the style of the widget.
     *
     * @return QAction* the style action
     */
    virtual QAction* style() = 0;

    /**
     * @brief Get the auto scroll policy combo box.
     *
     * The combo box is used to select the auto scroll policy.
     *
     * @return QComboBox* the auto scroll policy combo box
     */
    virtual QComboBox* autoScrollPolicy() = 0;

private:
    QLoguru* _parent;
};

extern QAbstractLoguruToolBar* createToolBar();
