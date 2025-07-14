#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QTest>
#include <QTimer>
#include <QTreeView>
#include <QCheckBox>

#include "qloguru/qabstract_loguru_toolbar.hpp"
#include "qloguru/qloguru.hpp"
#include "loguru.hpp"

class QTestToolBar : public QAbstractLoguruToolBar
{
public:
    explicit QTestToolBar()
    {
        _autoScrollPolicy->addItems(
            { "Manual Scroll", "Scroll To Bottom", "Smart Scroll" }
        );
        _regex->setCheckable(true);
        _caseSensitive->setCheckable(true);
    }
    ~QTestToolBar() override = default;

#pragma region QAbstractSpdLogToolBar

public:
    QLineEdit* filter() override { return _filter; }
    QAction* regex() override { return _regex; }
    QAction* caseSensitive() override { return _caseSensitive; }
    QAction* clearHistory() override { return _clearHistory; }
    QAction* style() override { return _style; }
    QComboBox* autoScrollPolicy() override { return _autoScrollPolicy; }

#pragma endregion

public:
    QLineEdit* _filter = new QLineEdit;
    QAction* _regex = new QAction;
    QAction* _caseSensitive = new QAction;
    QAction* _clearHistory = new QAction;
    QAction* _style = new QAction;
    QComboBox* _autoScrollPolicy = new QComboBox;
};

class QLoguruTest : public QObject
{
    Q_OBJECT

public:
    QLoguruTest() { }

private slots:
    void checkMessageCountAllLevelsEnabled()
    {
        QLoguru widget;

        LOG_F(INFO, "test");
        LOG_F(WARNING,"test");
        LOG_F(ERROR, "test");
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 3);
    }


    void clearLogHistory()
    {
        QLoguru widget;

        LOG_F(INFO, "test");
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 1);
        widget.clear();
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 0);
    }

    void limitLogTest()
    {
        QLoguru widget;
        QCOMPARE(widget.getMaxEntries(), std::nullopt);
        widget.setMaxEntries(20);
        QCOMPARE(widget.getMaxEntries(), 20);
        LOG_F(INFO, "test");
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 1);
        for (int i = 0; i < 100; i++)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 20);
        widget.setMaxEntries(std::nullopt);
        QCOMPARE(widget.getMaxEntries(), std::nullopt);
        for (int i = 0; i < 50; i++)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 70);
        widget.setMaxEntries(20);
        QCOMPARE(widget.getMaxEntries(), 20);
        QCOMPARE(widget.itemsCount(), 20);
    }

    void backgroundForegroundColorTest()
    {
        QLoguru widget;
        QCOMPARE(widget.getLoggerBackground("test"), std::nullopt);
        widget.setLoggerBackground("test", QBrush(Qt::red));
        QCOMPARE(widget.getLoggerBackground("test"), QBrush(Qt::red));
        QCOMPARE(widget.getLoggerBackground("test2"), std::nullopt);
        widget.setLoggerBackground("test", std::nullopt);
        QCOMPARE(widget.getLoggerBackground("test"), std::nullopt);

        QCOMPARE(widget.getLoggerForeground("test"), std::nullopt);
        widget.setLoggerForeground("test", Qt::white);
        QCOMPARE(widget.getLoggerForeground("test"), Qt::white);
        QCOMPARE(widget.getLoggerForeground("test2"), std::nullopt);
        widget.setLoggerForeground("test", std::nullopt);
        QCOMPARE(widget.getLoggerForeground("test"), std::nullopt);
    }

    void fontTest()
    {
        QLoguru widget;
        QFont testFont;
        testFont.setBold(true);
        QCOMPARE(widget.getLoggerBackground("test"), std::nullopt);
        widget.setLoggerFont("test", testFont);
        QCOMPARE(widget.getLoggerFont("test"), testFont);
        QCOMPARE(widget.getLoggerFont("test2"), std::nullopt);
        widget.setLoggerFont("test", std::nullopt);
        widget.setLoggerFont("test2", testFont);
        QCOMPARE(widget.getLoggerFont("test"), std::nullopt);
        QCOMPARE(widget.getLoggerFont("test2"), testFont);
    }

    void runToolbarTests()
    {
        std::vector<std::unique_ptr<QAbstractLoguruToolBar>> toolbars;
        toolbars.push_back(std::make_unique<QTestToolBar>());
        toolbars.push_back(
            std::unique_ptr<QAbstractLoguruToolBar>(createToolBar())
        );

        for (auto& toolbar : toolbars) {
            filterMessageAndCompletionHistory(toolbar.get());
            filterCaseDependant(toolbar.get());
            filterRegularExpressions(toolbar.get());
            autoScrollPolicyDefault(toolbar.get());
            autoScrollPolicyAutoScroll(toolbar.get());
            autoScrollPolicySmartScroll(toolbar.get());
            toolbar->setParent(nullptr);
        }
    }

    void filterMessageAndCompletionHistory(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;
        LOG_F(INFO,"Lorem ipsum dolor sit amet, consectetur adipiscing elit");
        LOG_F(INFO,"Another message");
        widget.registerToolbar(toolbar);
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 2);
        QTest::keyClicks(toolbar->filter(), "ipsum");
        QTest::keyClick(toolbar->filter(), Qt::Key_Enter);
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 1);
        toolbar->filter()->setText("Another");
        QCOMPARE(widget.itemsCount(), 1);
        toolbar->filter()->setText("");
        QTest::keyClick(toolbar->filter(), Qt::Key_Enter);
        QCOMPARE(widget.itemsCount(), 2);
    }

    void filterCaseDependant(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;

        LOG_F(INFO,"Lorem ipsum dolor sit amet, consectetur adipiscing elit");
        LOG_F(INFO,"Another message");
        widget.registerToolbar(toolbar);

        QLineEdit* filter = toolbar->filter();
        QAction* caseSensitive = toolbar->caseSensitive();
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 2);
        filter->setText("Ipsum");
        QCOMPARE(widget.itemsCount(), 1);
        caseSensitive->trigger();
        QCOMPARE(widget.itemsCount(), 0);
        filter->setText("ipsum");
        QCOMPARE(widget.itemsCount(), 1);
        filter->setText("nonexistent");
        QCOMPARE(widget.itemsCount(), 0);
        filter->setText("Ipsum");
        QCOMPARE(widget.itemsCount(), 0);
        caseSensitive->trigger();
        QCOMPARE(widget.itemsCount(), 1);
        filter->setText("");
        QCOMPARE(widget.itemsCount(), 2);
        caseSensitive->trigger();
        QCOMPARE(widget.itemsCount(), 2);
    }

    void filterRegularExpressions(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;
        LOG_F(INFO,"Lorem ipsum dolor sit amet, consectetur adipiscing elit");
        LOG_F(INFO,"Another message");
        widget.registerToolbar(toolbar);

        QLineEdit* filter = toolbar->filter();
        QAction* regex = toolbar->regex();
        QTest::qWait(100);
        QCOMPARE(widget.itemsCount(), 2);
        filter->setText("ipsum");
        QCOMPARE(widget.itemsCount(), 1);
        regex->trigger();
        QCOMPARE(widget.itemsCount(), 1);
        filter->setText(".*");
        QCOMPARE(widget.itemsCount(), 2);
        filter->setText(".*amet");
        QCOMPARE(widget.itemsCount(), 1);
        regex->trigger();
        QCOMPARE(widget.itemsCount(), 0);
        filter->setText(".*");
        QCOMPARE(widget.itemsCount(), 0);
        regex->trigger();
        QCOMPARE(widget.itemsCount(), 2);
        // TODO: base implementation of the toolbar should change the color of
        // the invalid regex text filter->setText("\(.*"); QColor color =
        // filter->palette().color(QPalette::Text); QCOMPARE(color, Qt::red);
        // QRegularExpression re("\(.*");
        // QCOMPARE(filter->toolTip(), re.errorString());
    }

    void autoScrollPolicyDefault(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;
        widget.registerToolbar(toolbar);

        QComboBox* autoScrollPolicy = toolbar->autoScrollPolicy();
        QTreeView* treeView = widget.findChild<QTreeView*>("qloguruTreeView");
        QScrollBar* scrollBar = treeView->verticalScrollBar();

        treeView->resize(100, 100);

        autoScrollPolicy->setCurrentIndex(
            static_cast<int>(AutoScrollPolicy::AutoScrollPolicyDisabled)
        );

        QCOMPARE(scrollBar->value(), scrollBar->maximum());

        for (int i = 0; i < 10; ++i)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        auto actualValue = scrollBar->value();
        treeView->scrollToBottom();
        auto maximumValue = scrollBar->value();

        QVERIFY(actualValue != maximumValue);
    }

    void autoScrollPolicyAutoScroll(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;

        widget.registerToolbar(toolbar);

        QComboBox* autoScrollPolicy = toolbar->autoScrollPolicy();
        QTreeView* treeView = widget.findChild<QTreeView*>("qloguruTreeView");
        QScrollBar* scrollBar = treeView->verticalScrollBar();

        treeView->resize(100, 100);

        autoScrollPolicy->setCurrentIndex(
            static_cast<int>(AutoScrollPolicy::AutoScrollPolicyEnabled)
        );

        // fill the visible area
        for (int i = 0; i < 5; ++i)
            LOG_F(INFO, "test %d", i);

        QCOMPARE(scrollBar->value(), scrollBar->maximum());

        for (int i = 0; i < 3; ++i)
            LOG_F(INFO, "test %d", i);

        QCOMPARE(scrollBar->value(), scrollBar->maximum());

        widget.setAutoScrollPolicy(AutoScrollPolicy::AutoScrollPolicyDisabled);
        QCOMPARE(
            autoScrollPolicy->currentIndex(),
            static_cast<int>(AutoScrollPolicy::AutoScrollPolicyDisabled)
        );
    }

    void autoScrollPolicySmartScroll(QAbstractLoguruToolBar* toolbar)
    {
        QLoguru widget;

        widget.registerToolbar(toolbar);

        QComboBox* autoScrollPolicy = toolbar->autoScrollPolicy();
        QTreeView* treeView = widget.findChild<QTreeView*>("qloguruTreeView");
        QScrollBar* scrollBar = treeView->verticalScrollBar();

        treeView->resize(100, 100);

        autoScrollPolicy->setCurrentIndex(
            static_cast<int>(AutoScrollPolicy::AutoScrollPolicyEnabledIfBottom)
        );

        // fill the visible area
        for (int i = 0; i < 5; ++i)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QCOMPARE(scrollBar->value(), scrollBar->maximum());

        for (int i = 0; i < 3; ++i)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QCOMPARE(scrollBar->value(), scrollBar->maximum());

        scrollBar->setValue(0);
        for (int i = 0; i < 3; ++i)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QVERIFY(scrollBar->value() != scrollBar->maximum());

        scrollBar->setValue(scrollBar->maximum());
        for (int i = 0; i < 3; ++i)
            LOG_F(INFO, "test %d", i);
        QTest::qWait(100);
        QCOMPARE(scrollBar->value(), scrollBar->maximum());
    }

    void headerColumnShowHide()
    {
        QLoguru widget;
        QTreeView* treeView = widget.findChild<QTreeView*>("qloguruTreeView");
        QHeaderView* headerView = treeView->header();
        QCOMPARE(headerView->count(), 5);
        QMetaObject::invokeMethod(
            headerView,
            [] {
            QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
            auto widgetsIt = std::find_if(
                topLevelWidgets.begin(),
                topLevelWidgets.end(),
                [](QWidget* widget) {
                return widget->objectName() == "qloguruHeaderContextMenu";
                });
            QVERIFY(widgetsIt != topLevelWidgets.end());
            QMenu* menu = qobject_cast<QMenu*>(*widgetsIt);
            QTest::mouseClick(
                menu, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5)
            );
            },
            Qt::QueuedConnection);
        headerView->customContextMenuRequested(QPoint(5, 5));
        QCOMPARE(headerView->count(), 5);
        QCOMPARE(headerView->hiddenSectionCount(), 1);
        QMetaObject::invokeMethod(
            headerView,
            [] {
            QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
            auto widgetsIt = std::find_if(
                topLevelWidgets.begin(),
                topLevelWidgets.end(),
                [](QWidget* widget) {
                return widget->objectName() == "qloguruHeaderContextMenu";
                });
            QVERIFY(widgetsIt != topLevelWidgets.end());
            QMenu* menu = qobject_cast<QMenu*>(*widgetsIt);
            QTest::mouseClick(
                menu, Qt::LeftButton, Qt::NoModifier, QPoint(5, 25)
            );
            },
            Qt::QueuedConnection);
        headerView->customContextMenuRequested(QPoint(5, 5));
        QCOMPARE(headerView->count(), 5);
        QCOMPARE(headerView->hiddenSectionCount(), 2);
        QMetaObject::invokeMethod(
            headerView,
            [] {
            QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
            auto widgetsIt = std::find_if(
                topLevelWidgets.begin(),
                topLevelWidgets.end(),
                [](QWidget* widget) {
                return widget->objectName() == "qloguruHeaderContextMenu";
                });
            QVERIFY(widgetsIt != topLevelWidgets.end());
            QMenu* menu = qobject_cast<QMenu*>(*widgetsIt);
            QTest::mouseClick(
                menu, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5)
            );
            },
            Qt::QueuedConnection);
        headerView->customContextMenuRequested(QPoint(5, 5));
        QCOMPARE(headerView->count(), 5);
        QCOMPARE(headerView->hiddenSectionCount(), 1);
    }

    void setStyleFromToolbar()
    {
        QLoguru widget;

        std::thread t([]() {
            loguru::set_thread_name("test");
            LOG_F(INFO, "test");
        });
        QTest::qWait(100);
        std::thread t1([]() {
          loguru::set_thread_name("test1");
          LOG_F(INFO, "test1");
        });
        t.join();
        t1.join();
        QTest::qWait(100);

        std::unique_ptr<QAbstractLoguruToolBar> toolbar(createToolBar());
        widget.registerToolbar(toolbar.get());
        QAction* style = toolbar->style();

        QFont f;
        f.setBold(true);
        auto dialogManipThread =
            manipulateStyleDialog("test", "#ff0000", "#00ff00", f, true);
        style->trigger();
        dialogManipThread.join();



        const QTreeView* treeView =
            widget.findChild<const QTreeView*>("qloguruTreeView");
        const QAbstractItemModel* model = treeView->model();
        QModelIndex index = model->index(0, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(),
            QColor("#ff0000")
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(),
            QColor("#00ff00")
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(),
            f
        );

        index = model->index(1, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test1")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(), QFont {}
        );

        dialogManipThread =
            manipulateStyleDialog("test", std::nullopt, std::nullopt, std::nullopt, true);
        style->trigger();
        dialogManipThread.join();

        index = model->index(0, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(),
            QColor("#ff0000")
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(),
            QColor("#00ff00")
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(),
            f
        );

        dialogManipThread = manipulateStyleDialog("test", "", "", QFont(), true);
        style->trigger();
        dialogManipThread.join();

        index = model->index(0, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(), QFont {}
        );

        dialogManipThread = manipulateStyleDialog("test", "", "", QFont(), false);
        style->trigger();
        dialogManipThread.join();

        index = model->index(0, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(), QFont {}
        );
        index = model->index(1, 4);
        QCOMPARE(
            model->data(index, Qt::DisplayRole).value<QString>(),
            QString("test1")
        );
        QCOMPARE(
            model->data(index, Qt::BackgroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::ForegroundRole).value<QColor>(), QColor {}
        );
        QCOMPARE(
            model->data(index, Qt::FontRole).value<QFont>(), QFont {}
        );
    }

private:
    std::thread manipulateStyleDialog(
        std::optional<QString> name,
        std::optional<QString> background,
        std::optional<QString> foreground,
        std::optional<QFont> font,
        bool accept
    ) const
    {
        return std::thread([ n = std::move(name),
                             bg = std::move(background),
                             fg = std::move(foreground),
                             fnt = std::move(font),
                             accept ] {
            QDialog* dialog;
            bool success = QTest::qWaitFor(
                [ &dialog ]() -> bool {
                    auto widgets = qApp->topLevelWidgets();
                    auto it = std::find_if(
                        widgets.begin(),
                        widgets.end(),
                        [](QWidget* widget) {
                    return widget->objectName() == "qloguruStyleDialog";
                        });

                    if (it == widgets.end())
                        return false;

                    dialog = qobject_cast<QDialog*>(*it);
                    return true;
                },
                1000
            );

            QVERIFY(success);

            QMetaObject::invokeMethod(
                dialog,
                [ name = std::move(n),
                  background = std::move(bg),
                  foreground = std::move(fg),
                  font = std::move(fnt),
                  accept,
                  dialog ] {
                QVERIFY(dialog);
                QLineEdit* loggerNameEdit =
                    dialog->findChild<QLineEdit*>("loggerNameEdit");
                QVERIFY(loggerNameEdit);
                QLineEdit* backgroundColorEdit =
                    dialog->findChild<QLineEdit*>("backgroundColorEdit");
                QVERIFY(backgroundColorEdit);
                QLineEdit* textColorEdit =
                    dialog->findChild<QLineEdit*>("textColorEdit");
                QVERIFY(textColorEdit);
                QCheckBox* checkBoxBold = 
                    dialog->findChild<QCheckBox*>("checkBoxBold");
                QVERIFY(checkBoxBold);

                if (name)
                    loggerNameEdit->setText(name.value());

                if (background)
                    backgroundColorEdit->setText(background.value());

                if (foreground)
                    textColorEdit->setText(foreground.value());
                
                if (font)
                    checkBoxBold->setChecked(font.value().bold());

                QDialogButtonBox* buttonBox =
                    dialog->findChild<QDialogButtonBox*>("buttonBox");
                QVERIFY(buttonBox);
                if (accept)
                    buttonBox->button(QDialogButtonBox::Ok)->click();
                else
                    buttonBox->button(QDialogButtonBox::Cancel)->click();
                },
                Qt::QueuedConnection);
        });
    }
};

QTEST_MAIN(QLoguruTest);
#include "test_qloguru.moc"
