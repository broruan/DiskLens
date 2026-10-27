#pragma once

#include <QIcon>
#include <QWidget>
#include <functional>

class QLineEdit;

QIcon lineIcon(const QString &name, const QColor &color = QColor("#7a87a0"));
QString applicationStyle();
int scaledUiPixels(int pixels);
// Coalesce typing, but apply clearing and Enter immediately.
void connectSearch(QLineEdit *edit, QObject *context, std::function<void(const QString &)> apply);

class DiskRing : public QWidget {
public:
    explicit DiskRing(QWidget *parent = nullptr);
    void setSpace(quint64 total, quint64 used, quint64 reserved);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    quint64 m_total = 0, m_used = 0, m_reserved = 0;
};

struct RankItem { QString name; quint64 bytes; };
class RankChart : public QWidget {
public:
    explicit RankChart(QWidget *parent = nullptr);
    void setItems(QVector<RankItem> items);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QVector<RankItem> m_items;
};
