#include <QBrush>
#include <QDateTime>
#include <QFile>
#include <QFont>
#include <QIcon>
#include <map>
#include <array>

#include "qloguru_model.hpp"

namespace
{

static std::map<int, const char*> icon_names = {
    {0, ":/res/info.png"},
    {-1, ":/res/warn.png"},
    {-2, ":/res/error.png"},
    {-3, ":/res/critical.png"}
};

static  std::map<int, const char*> level_names = {
     {0, "Info"},
    {-1, "Warning"},
    {-2, "Error"},
    {-3,"Critical"}
};

enum class Column { Level = 0, Logger, Time, Elapsed, Message, Last };

static constexpr std::array<const char*, 5> column_names = {
    "Level", "Logger","Elapsed", "Time", "Message"
};

} // namespace

QLoguruModel::QLoguruModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

void QLoguruModel::addEntry(entry_t entry)
{
    if (_maxEntries > 0 && _items.size() == _maxEntries) {
        beginRemoveRows(QModelIndex(), 0, 0);
        _items.pop_front();
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    _items.push_back(std::move(entry));

    endInsertRows();
}

void QLoguruModel::setMaxEntries(std::optional<std::size_t> maxEntries)
{
    _maxEntries = maxEntries;
    // Incase the new maximum is below the current amount of items.
    if (_maxEntries > 0 && _items.size() > _maxEntries) {
        std::size_t offset = _items.size() - _maxEntries.value();
        beginRemoveRows(QModelIndex(), 0, offset - 1);
        _items.erase(_items.begin(), _items.begin() + offset);
        endRemoveRows();
    }
}

std::optional<std::size_t> QLoguruModel::getMaxEntries() const
{
    return _maxEntries;
}

void QLoguruModel::clear()
{
    beginResetModel();
    _items.clear();
    endResetModel();
}

int QLoguruModel::rowCount(const QModelIndex& parent) const
{
    return _items.size();
}

int QLoguruModel::columnCount(const QModelIndex& parent) const
{
    return static_cast<std::size_t>(Column::Last);
}

QVariant QLoguruModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= _items.size())
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: {
            auto const& item = _items[ index.row() ];
            switch (static_cast<Column>(index.column())) {
                case Column::Level: {
                    return QString(level_names[ item.level ]);
                }

                case Column::Logger: {
                    return QString::fromStdString(item.loggerName);
                }
                case Column::Elapsed: {
                    return QString::fromStdString(item.elapsed);
                }

                case Column::Time: {
                    return QString::fromStdString(item.time);
                }

                case Column::Message: {
                    return QString::fromStdString(item.message);
                }

                default: {
                    break;
                }
            }

            break;
        }

        case Qt::DecorationRole: {
            if (index.column() == 0) {
                const auto& item = _items[ index.row() ];
                if (icon_names.count(_items[ index.row() ].level )) {
                    return QIcon(
                        QString(icon_names[ _items[ index.row() ].level ])
                    );
                }
            }

            break;
        }

        case Qt::BackgroundRole: {
            std::string loggerName = _items[ index.row() ].loggerName;
            if (_backgroundMappings.contains(loggerName))
                return _backgroundMappings.at(loggerName);

            break;
        }

        case Qt::ForegroundRole: {
            std::string loggerName = _items[ index.row() ].loggerName;
            if (_foregroundMappings.contains(loggerName))
                return _foregroundMappings.at(loggerName);

            break;
        }

        case Qt::FontRole: {
            std::string loggerName = _items[ index.row() ].loggerName;
            if (_fontMappings.contains(loggerName))
                return _fontMappings.at(loggerName);

            break;
        }

        default: {
            break;
        }
    }

    return QVariant();
}

QVariant QLoguruModel::headerData(
    int section, Qt::Orientation orientation, int role
) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QString(column_names[ section ]);

    return QVariant();
}

void QLoguruModel::setLoggerForeground(
    std::string_view loggerName, std::optional<QColor> color
)
{
    int lastRow = this->rowCount() - 1;
    if (lastRow < 0)
        lastRow = 0;
    int lastColumn = this->columnCount() - 1;
    if (lastColumn < 0)
        lastColumn = 0;
    if (color.has_value()) {
        _foregroundMappings[ std::string(loggerName) ] = color.value();
        emit dataChanged(
            this->index(0),
            this->index(lastRow, lastColumn),
            { Qt::ForegroundRole }
        );
    } else if (_foregroundMappings.contains(std::string(loggerName))) {
        _foregroundMappings.erase(std::string(loggerName));
        emit dataChanged(
            this->index(0),
            this->index(lastRow, lastColumn),
            { Qt::ForegroundRole }
        );
    }
}

std::optional<QColor> QLoguruModel::getLoggerForeground(
    std::string_view loggerName
) const
{
    if (_foregroundMappings.contains(std::string(loggerName)))
        return _foregroundMappings.at(std::string(loggerName));

    return std::nullopt;
}

void QLoguruModel::setLoggerBackground(
    std::string_view loggerName, std::optional<QBrush> brush
)
{
    int lastRow = this->rowCount() - 1;
    if (lastRow < 0)
        lastRow = 0;
    int lastColumn = this->columnCount() - 1;
    if (lastColumn < 0)
        lastColumn = 0;
    if (brush.has_value()) {
        _backgroundMappings[ std::string(loggerName) ] = brush.value();
        emit dataChanged(
            this->index(0),
            this->index(lastRow, lastColumn),
            { Qt::BackgroundRole }
        );
    } else if (_backgroundMappings.contains(std::string(loggerName))) {
        _backgroundMappings.erase(std::string(loggerName));
        emit dataChanged(
            this->index(0),
            this->index(lastRow, lastColumn),
            { Qt::BackgroundRole }
        );
    }
}

std::optional<QBrush> QLoguruModel::getLoggerBackground(
    std::string_view loggerName
) const
{
    if (_backgroundMappings.contains(std::string(loggerName)))
        return _backgroundMappings.at(std::string(loggerName));

    return std::nullopt;
}

void QLoguruModel::setLoggerFont(
    std::string_view loggerName, std::optional<QFont> font
)
{
    int lastRow = this->rowCount() - 1;
    if (lastRow < 0)
        lastRow = 0;
    int lastColumn = this->columnCount() - 1;
    if (lastColumn < 0)
        lastColumn = 0;
    if (font.has_value()) {
        _fontMappings[ std::string(loggerName) ] = font.value();
        emit dataChanged(
            this->index(0), this->index(lastRow, lastColumn), { Qt::FontRole }
        );
    } else if (_fontMappings.contains(std::string(loggerName))) {
        _fontMappings.erase(std::string(loggerName));
        emit dataChanged(
            this->index(0), this->index(lastRow, lastColumn), { Qt::FontRole }
        );
    }
}

std::optional<QFont> QLoguruModel::getLoggerFont(std::string_view loggerName
) const
{
    if (_fontMappings.contains(std::string(loggerName)))
        return _fontMappings.at(std::string(loggerName));

    return std::nullopt;
}