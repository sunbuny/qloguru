#pragma once
#include "qloguru_model.hpp"
#include <regex>
#include <string>
#include <loguru.hpp>
#include <QObject>
#include <QString>
#include <QMetaObject>

class QtLoggerSink : public QObject {
    Q_OBJECT
public:
    explicit QtLoggerSink(QLoguruModel* model, QObject* parent = nullptr)
        : QObject(parent), _model(model)
    {
        loguru::add_callback("qt_logger_sink", QtLoggerSink::callback, _model, loguru::Verbosity_INFO);
    }

    static void callback(void* user_data, const loguru::Message& message)
    {
        if (!user_data) return;
        QLoguruModel::entry_t entry;
        std::regex pattern(R"(.*?(\d{2}:\d{2}:\d{2}\.\d{3})\s+\(\s*([0-9.]+s)\)\s+\[\s*(.*?)\s*\])");
        std::smatch match;
        std::string preamble(message.preamble);
        if (std::regex_search(preamble, match, pattern)) {
            std::string time = match[1];
            std::string elapsed = match[2];
            std::string thread_id = match[3];
            entry.time = time;
            entry.elapsed = elapsed;
            entry.loggerName = thread_id;
        }else {
            return;
        }
        entry.level = static_cast<int>(message.verbosity);
        entry.message = message.message;
        // make sure addEntry is call by QT GUI thread
        auto model = (QLoguruModel*)user_data;
        QMetaObject::invokeMethod(model, [model, entry]() {
            model->addEntry(entry);
        }, Qt::QueuedConnection);
    }

    ~QtLoggerSink() override {
        loguru::remove_callback("qt_logger_sink");
    }

    void invalidate() { _model = nullptr; }

private:
    QLoguruModel* _model;
};

