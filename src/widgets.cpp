#include "localization.h"
#include "widgets.h"
#include "models.h"
#include <QPainter>
#include <QApplication>
#include <QtMath>
#include <QPainterPath>
#include <QLineEdit>
#include <QPixmapCache>
#include <QTimer>

void connectSearch(QLineEdit *edit, QObject *context, std::function<void(const QString &)> apply)
{
    auto *timer = new QTimer(edit);
    timer->setSingleShot(true);
    timer->setInterval(140);
    const auto commit = [edit, timer, apply] {
        timer->stop();
        apply(edit->text());
    };
    QObject::connect(timer, &QTimer::timeout, context, commit);
    QObject::connect(edit, &QLineEdit::textChanged, context, [timer, commit](const QString &text) {
        if (text.isEmpty()) commit();
        else timer->start();
    });
    QObject::connect(edit, &QLineEdit::returnPressed, context, commit);
}

int scaledUiPixels(int pixels)
{
    const int fontSize = qApp->property("disklensFontSize").toInt();
    return qRound(pixels * (fontSize > 0 ? fontSize / 13.0 : 1.0));
}

QIcon lineIcon(const QString &name, const QColor &color)
{
    const QString key = QStringLiteral("disklens-line:%1:%2").arg(name, color.name(QColor::HexArgb));
    QPixmap cached;
    if (QPixmapCache::find(key, &cached)) return QIcon(cached);
    QPixmap image(48, 48); image.fill(Qt::transparent);
    QPainter p(&image); p.setRenderHint(QPainter::Antialiasing); p.scale(2, 2);
    p.setPen(QPen(color, 1.7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (name == "grid") {
        for (int x : {4, 14}) for (int y : {4, 14}) p.drawRoundedRect(QRectF(x,y,6,6),1.5,1.5);
    } else if (name == "apps") {
        p.drawRoundedRect(QRectF(3,4,18,16),2,2); p.drawLine(3,9,21,9); p.drawPoint(6,6); p.drawLine(9,13,9,17); p.drawLine(13,13,17,13); p.drawLine(13,17,17,17);
    } else if (name == "files") {
        QPainterPath path; path.moveTo(6,3); path.lineTo(14,3); path.lineTo(19,8); path.lineTo(19,21); path.lineTo(6,21); path.closeSubpath();
        p.drawPath(path); p.drawLine(14,3,14,9); p.drawLine(14,9,19,9); p.drawLine(9,13,16,13); p.drawLine(9,17,14,17);
    } else if (name == "disk") {
        p.drawRoundedRect(QRectF(3,5,18,14),2,2); p.drawLine(3,14,21,14); p.drawPoint(17,17); p.drawPoint(14,17);
    } else if (name == "refresh") {
        p.drawArc(QRectF(5,5,14,14),35*16,285*16); p.drawLine(19,4,19,9); p.drawLine(19,9,14,9);
    } else if (name == "search") {
        p.drawEllipse(QRectF(4,3,12,12)); p.drawLine(15,14,21,20);
    } else if (name == "folder") {
        QPainterPath path; path.moveTo(3,8); path.lineTo(3,5); path.lineTo(10,5); path.lineTo(12,8); path.lineTo(21,8); path.lineTo(21,20); path.lineTo(3,20); path.closeSubpath(); p.drawPath(path);
    } else if (name == "trash") {
        p.drawLine(4,6,20,6); p.drawLine(9,3,15,3); p.drawLine(6,6,7,21); p.drawLine(7,21,17,21); p.drawLine(17,21,18,6); p.drawLine(10,10,10,17); p.drawLine(14,10,14,17);
    } else if (name == "copy") {
        p.drawRoundedRect(QRectF(8,8,12,13),1.5,1.5); p.drawLine(4,16,4,3); p.drawLine(4,3,16,3);
    } else if (name == "arrow") {
        p.drawLine(4,12,20,12); p.drawLine(15,7,20,12); p.drawLine(20,12,15,17);
    } else if (name == "stop") {
        p.drawRoundedRect(QRectF(6,6,12,12),2,2);
    } else if (name == "chart") {
        p.drawLine(4,20,21,20); p.drawLine(6,16,6,11); p.drawLine(12,16,12,4); p.drawLine(18,16,18,8);
    } else if (name == "code") {
        p.drawLine(8,6,3,12); p.drawLine(3,12,8,18); p.drawLine(16,6,21,12); p.drawLine(21,12,16,18); p.drawLine(14,3,10,21);
    }
    p.end();
    QPixmapCache::insert(key, image);
    return QIcon(image);
}

DiskRing::DiskRing(QWidget *parent) : QWidget(parent) { setFixedSize(scaledUiPixels(148),scaledUiPixels(148)); }
void DiskRing::setSpace(quint64 total, quint64 used, quint64 reserved) { m_total=total; m_used=used; m_reserved=reserved; update(); }
void DiskRing::paintEvent(QPaintEvent *)
{
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    const double side = qMin(width(),height())-24;
    const QRectF ring((width()-side)/2, (height()-side)/2, side, side);
    p.setPen(QPen(QColor("#3d4d77"), 11, Qt::SolidLine, Qt::RoundCap)); p.drawEllipse(ring);
    if (m_total) {
        double fraction = qBound(0.0, double(m_used)/m_total, 1.0);
        if (m_reserved) {
            p.setPen(QPen(QColor("#8796c0"),11,Qt::SolidLine,Qt::FlatCap));
            p.drawArc(ring, 90*16, -int((double(m_used+m_reserved)/m_total)*360*16));
        }
        if (fraction > 0) {
            p.setPen(QPen(QColor("#8fa7ff"),11,Qt::SolidLine,Qt::RoundCap));
            p.drawArc(ring,90*16,-qMax(1,int(fraction*360*16)));
        }
    }
    QFont f = font(); f.setPixelSize(scaledUiPixels(29)); f.setBold(true); p.setFont(f); p.setPen(QColor("#ffffff"));
    p.drawText(rect().adjusted(0,-8,0,-8),Qt::AlignCenter,m_total ? QString::number(qRound(double(m_used)*100/m_total))+"%" : QStringLiteral("—"));
    f.setPixelSize(scaledUiPixels(10)); f.setBold(false); p.setFont(f); p.setPen(QColor("#c1cdee"));
    p.drawText(rect().adjusted(0,scaledUiPixels(46),0,0),Qt::AlignCenter,dlTr("磁盘已使用"));
}

RankChart::RankChart(QWidget *parent) : QWidget(parent) { setMinimumHeight(scaledUiPixels(250)); setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding); }
void RankChart::setItems(QVector<RankItem> items) { m_items=std::move(items); update(); }
void RankChart::paintEvent(QPaintEvent *)
{
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    if (m_items.isEmpty()) { p.setPen(QColor("#8b96ac")); p.drawText(rect(),Qt::AlignCenter,dlTr("正在读取已安装的软件…")); return; }
    const QColor colors[] = {QColor("#536bed"),QColor("#7586ef"),QColor("#96a3f4"),QColor("#40bba5"),QColor("#79cfbf"),QColor("#aadfd5")};
    const double maxBytes = qMax<quint64>(1,m_items.first().bytes);
    const int rowHeight = qMin(scaledUiPixels(52),height()/qMax(1,int(m_items.size())));
    const int textHeight = fontMetrics().height() + 4;
    const int valueWidth = fontMetrics().horizontalAdvance(QStringLiteral("999.9 GiB")) + 12;
    for (int i=0;i<m_items.size() && i<6;++i) {
        int y=i*rowHeight+3;
        QFont f=font(); f.setBold(false); p.setFont(f); p.setPen(QColor("#3c4963"));
        QString label = QFontMetrics(f).elidedText(m_items[i].name,Qt::ElideRight,qMax(0,width()-valueWidth-8));
        p.drawText(QRect(0,y,width()-valueWidth-8,textHeight),Qt::AlignVCenter,label);
        p.setPen(QColor("#6f7b94")); p.drawText(QRect(width()-valueWidth,y,valueWidth,textHeight),Qt::AlignRight|Qt::AlignVCenter,formatBytes(m_items[i].bytes));
        QRectF bar(0,y+textHeight+6,width(),5); p.setPen(Qt::NoPen); p.setBrush(QColor("#eef1f7")); p.drawRoundedRect(bar,2.5,2.5);
        bar.setWidth(qMax(5.0,width()*m_items[i].bytes/maxBytes)); p.setBrush(colors[i]); p.drawRoundedRect(bar,2.5,2.5);
    }
}

QString applicationStyle()
{
    return QStringLiteral(R"(
    QWidget { color: #24324b; font-size: 13px; }
    QMainWindow, QWidget#workspace { background: #f3f5fa; }
    QFrame#sidebar { background: #ffffff; border: 0; border-right: 1px solid #e5e9f2; }
    QLabel#brand { color: #233254; font-size: 22px; font-weight: 700; }
    QLabel#brandSub { color: #6b7891; font-size: 11px; }
    QLabel#sideCaption { color: #75819a; font-size: 10px; font-weight: 600; }
    QPushButton#navButton { text-align: left; background: transparent; color: #62708a; border: 1px solid transparent; padding: 12px 14px; border-radius: 10px; font-size: 13px; }
    QPushButton#navButton:hover { background: #f4f6fc; color: #364d9c; }
    QPushButton#navButton:checked { background: #eaf0ff; border-color: #dce5ff; color: #3c54cb; font-weight: 600; }
    QPushButton#navButton:focus { border-color: #8699eb; }
    QPushButton#appearanceButton { text-align: left; color: #55647f; background: #f6f8fc; border-color: #e6ebf4; }
    QPushButton#appearanceButton:hover { background: #eaf0ff; border-color: #c7d3fa; }
    QLabel#pageTitle { font-size: 25px; font-weight: 700; color: #1f2e49; }
    QLabel#subtitle, QLabel#muted { color: #6f7d95; font-size: 12px; }
    QFrame#card { background: #ffffff; border: 1px solid #e4e9f3; border-radius: 16px; }
    QFrame#diskCard { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #263863,stop:1 #182748); border: 1px solid #273a64; border-radius: 16px; }
    QFrame#diskCard QLabel { color: #ffffff; }
    QFrame#diskCard QLabel#muted { color: #bfcae2; }
    QFrame#diskCard QComboBox { background: #35476b; color: #eef3ff; border-color: #536584; padding: 5px 10px; }
    QFrame#diskCard QComboBox QAbstractItemView { color: #24324b; background: white; }
    QLabel#cardTitle { color: #34415c; font-size: 14px; font-weight: 600; }
    QLabel#metricValue { font-size: 32px; font-weight: 700; color: #233455; }
    QLabel#smallValue { font-size: 22px; font-weight: 700; color: #263551; }
    QLabel#metricIcon { background: #edf1ff; border-radius: 10px; padding: 8px; }
    QLabel#fileMetricIcon { background: #e9f7f2; border-radius: 10px; padding: 8px; }
    QLabel#tag { background: #eaf7f0; border-radius: 6px; color: #237c60; padding: 5px 9px; font-size: 11px; }
    QPushButton { background: white; color: #53627c; border: 1px solid #dce3ef; border-radius: 9px; padding: 9px 14px; font-size: 12px; }
    QPushButton:hover { background: #f0f4ff; border-color: #b8c7ee; color: #3e57c6; }
    QPushButton:pressed { background: #e4ebff; border-color: #8fa4e8; }
    QPushButton:focus { border-color: #728be5; }
    QPushButton:disabled { color: #9ca7ba; background: #f7f8fb; border-color: #e8ecf3; }
    QPushButton#primary { background: #5065df; border-color: #5065df; color: white; font-weight: 600; }
    QPushButton#primary:hover { background: #4054ce; border-color: #4054ce; }
    QPushButton#primary:pressed { background: #3347b7; }
    QPushButton#primary:focus { border-color: #263d9e; }
    QPushButton#primary:disabled { background: #b5c0ed; border-color: #b5c0ed; color: #ffffff; }
    QPushButton#danger { color: #b84155; border-color: #ebcbd3; background: #fff6f8; }
    QPushButton#danger:hover { background: #ffe9ee; }
    QPushButton#danger:disabled { color: #bba9af; background: #faf8f9; border-color: #eee6e9; }
    QPushButton#link { border: 1px solid transparent; background: transparent; color: #4a61cf; padding: 4px 2px; }
    QPushButton#link:hover { color: #2f46b3; background: #f0f4ff; }
    QPushButton#link:focus { border-color: #8699eb; }
    QLineEdit { background: #ffffff; border: 1px solid #dce3ef; border-radius: 9px; padding: 9px 12px; selection-background-color: #dfe7ff; selection-color: #24324b; }
    QLineEdit:hover { border-color: #bcc9e1; }
    QLineEdit:focus { border-color: #728be5; background: #fcfdff; }
    QLineEdit:disabled { background: #f5f7fb; color: #8793a8; }
    QComboBox { background: #fff; border: 1px solid #dce3ef; border-radius: 9px; padding: 8px 12px; min-height: 18px; }
    QComboBox:hover, QComboBox:focus { border-color: #8699df; }
    QComboBox::drop-down { border: 0; width: 24px; }
    QComboBox QAbstractItemView { background: white; border: 1px solid #dce2ef; selection-background-color: #eaf0ff; selection-color: #35435c; color: #35435c; padding: 5px; }
    QTableView { background: #fff; border: 0; outline: 0; gridline-color: #eef2f8; selection-background-color: #edf2ff; selection-color: #29364f; }
    QHeaderView::section { background: #f6f8fc; color: #697891; border: 0; border-bottom: 1px solid #e9edf5; padding: 12px 16px; font-size: 11px; text-align: left; }
    QHeaderView::section:hover { background: #edf2fc; color: #435b9e; }
    QTableView::item { padding: 8px 16px; }
    QTableView::item:selected { color: #29364f; }
    QTreeView { background: white; border: 0; outline: 0; selection-background-color: #edf2ff; selection-color: #29364f; }
    QTreeView::item { padding: 7px 5px; }
    QTreeView::item:hover { background: #f5f8ff; }
    QTreeView::item:selected { color: #29364f; background: #edf2ff; }
    QTabWidget::pane { border: 1px solid #e4e9f3; border-radius: 10px; background: white; }
    QTabBar::tab { color: #62718b; background: #eef2f9; padding: 10px 22px; border: 0; margin-right: 4px; }
    QTabBar::tab:selected { color: #4058c6; background: white; font-weight: 600; }
    QScrollBar:vertical { background: transparent; width: 10px; margin: 3px 2px; }
    QScrollBar::handle:vertical { background: #cbd5e5; min-height: 36px; border-radius: 3px; }
    QScrollBar::handle:vertical:hover { background: #9eafcf; }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
    QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px 3px; }
    QScrollBar::handle:horizontal { background: #cbd5e5; min-width: 36px; border-radius: 3px; }
    QScrollBar::handle:horizontal:hover { background: #9eafcf; }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }
    QProgressBar { border: 0; background: #e5eaf6; border-radius: 2px; max-height: 4px; }
    QProgressBar::chunk { background: #6077e8; border-radius: 2px; }
    QCheckBox { spacing: 7px; color: #53627c; }
    QCheckBox:focus { color: #354fbc; }
    QLabel#notice { background: #fff6e5; color: #8b6829; border-radius: 9px; padding: 10px 12px; font-size: 12px; }
    QTextEdit, QPlainTextEdit { border: 1px solid #dce3ef; border-radius: 9px; background: #f8fafd; color: #53627c; padding: 10px; }
    QToolTip { background: #243455; color: white; border: 0; padding: 8px; }
    QMessageBox, QDialog { background: #f5f7fc; }
    )");
}
