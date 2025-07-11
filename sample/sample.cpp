#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QStyleFactory>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <thread>
#include <QDebug>

#include "qspdlog/qabstract_spdlog_toolbar.hpp"
#include "qspdlog/qspdlog.hpp"
#include <loguru.hpp>

void configureColorScheme()
{
#ifdef Q_OS_WIN
    QSettings settings(
        "HKEY_CURRENT_"
        "USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personaliz"
        "e",
        QSettings::NativeFormat
    );
    if (settings.value("AppsUseLightTheme") == 0) {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        QColor darkColor = QColor(45, 45, 45);
        QColor disabledColor = QColor(127, 127, 127);
        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(
            QPalette::Disabled, QPalette::ButtonText, disabledColor
        );
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(
            QPalette::Disabled, QPalette::HighlightedText, disabledColor
        );

        qApp->setPalette(darkPalette);

        qApp->setStyleSheet(
            "QToolTip { color: #ffffff; background-color: #2a82da; "
            "border: 1px solid white; }"
        );
    }
#endif
}

void configureToolbar(
    QToolBar& toolbar, QSpdLog& logView
)
{
    QAction* clearAction = toolbar.addAction("Clear");
    QAction* generateAction = toolbar.addAction("Generate");
    QAction* generateMultipleAction = toolbar.addAction("GenerateMultiple");

    generateAction->connect(
        generateAction,
        &QAction::triggered,
        [] (bool) {
            // generate 10 messages with random levels
            for (int i = 0; i < 10; ++i) {
                int level = -(rand() % 4);
                switch (level) {
                    // case 0: LOG_F(TRACE, "Message %d", i); break;
                    // case 1: LOG_F(DEBUG, "Message %d", i); break;
                    case 0: LOG_F(INFO,  "Message %d", i); break;
                    case -1: LOG_F(WARNING, "Message %d", i); break;
                    case -2: LOG_F(ERROR, "Message %d", i); break;
                }
            }
        });

    generateMultipleAction->connect(
        generateMultipleAction,
        &QAction::triggered,
        [] (bool) {
            // create 10 threads and generate 10 messages with random levels
            std::vector<std::thread> threads;
            for (int i = 0; i < 10; ++i) {
                threads.emplace_back([i]() {
                    LOG_F(INFO, "Thread %d started", i);
                    for (int j = 0; j < 10; ++j) {
                        int level = rand() % 5;
                        switch (level) {
                            // case 0: LOG_F(TRACE, "[thread %d] Message %d", i, j); break;
                            // case 1: LOG_F(DEBUG, "[thread %d] Message %d", i, j); break;
                            case 2: LOG_F(INFO,  "[thread %d] Message %d", i, j); break;
                            case 3: LOG_F(WARNING, "[thread %d] Message %d", i, j); break;
                            case 4: LOG_F(ERROR, "[thread %d] Message %d", i, j); break;
                        }
                    }
                    LOG_F(INFO, "Thread %d finished", i);
                });
            }
            for (auto& thread : threads)
                thread.join();
        });

    clearAction->connect(clearAction, &QAction::triggered, [ &logView ](bool) {
        logView.clear();
    });
}

int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(qspdlog_resources);

    loguru::init(argc, argv);

    QApplication app(argc, argv);

    configureColorScheme();

    QToolBar toolbar("Manipulation toolbar");
    toolbar.show();

    QSpdLog log;
    log.show();
    log.move(toolbar.pos() + QPoint(0, toolbar.height() + 50));

    QAbstractSpdLogToolBar* logToolbar = createToolBar();
    log.registerToolbar(logToolbar);
    dynamic_cast<QWidget*>(logToolbar)->show();

    configureToolbar(toolbar, log);
    log.setMaxEntries(10);
    qDebug() << *log.getMaxEntries();

    int result = app.exec();
    return result;
}
