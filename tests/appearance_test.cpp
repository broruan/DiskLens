#include "appearance.h"
#include "mainwindow.h"
#include "explorerpage.h"
#include "localization.h"
#include "uninstallcontroller.h"
#include "widgets.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QScrollArea>
#include <QSignalSpy>
#include <QStackedWidget>
#include <QTableView>
#include <QTabBar>
#include <QImage>
#include <QTemporaryDir>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTimer>
#include <QtTest>
#include <memory>

class AppearanceTest : public QObject {
    Q_OBJECT
private slots:
    void settingsRoundTripAndInvalidFallback();
    void actualLanguageAndFontWithViewRestore();
    void duplicateRequestRestored_data();
    void duplicateRequestRestored();
    void dialogApplyAndCancel();
    void dialogPreview();
    void explorerLargeFontLayout();
    void modalAndPackagePreviewBlockChanges();
    void searchCoalescesTypingAndFlushes();
    void keyboardNavigationSearchAndRefresh();
    void overviewResponsiveLayout_data();
    void overviewResponsiveLayout();
};

void AppearanceTest::searchCoalescesTypingAndFlushes()
{
    QLineEdit edit;
    QStringList applied;
    connectSearch(&edit,&edit,[&](const QString &text){applied.append(text);});
    edit.show(); edit.setFocus();
    QTest::keyClicks(&edit,"package");
    QVERIFY(applied.isEmpty());
    QTRY_COMPARE(applied.size(),1);
    QCOMPARE(applied.last(),QStringLiteral("package"));
    QTest::keyClicks(&edit,"-name");
    QTest::keyClick(&edit,Qt::Key_Return);
    QCOMPARE(applied.size(),2);
    QCOMPARE(applied.last(),QStringLiteral("package-name"));
    edit.setText(QStringLiteral("pending"));
    edit.clear();
    QCOMPARE(applied.size(),3);
    QCOMPARE(applied.last(),QString());
    QTest::qWait(220);
    QCOMPARE(applied.size(),3); // Enter and clear cancel stale callbacks.
}

void AppearanceTest::keyboardNavigationSearchAndRefresh()
{
    AppearanceManager manager;
    QVERIFY(manager.apply({QStringLiteral("en"),13}));
    QTemporaryDir root;
    MainWindow window; window.setScanRoot(root.path()); window.show(); window.activateWindow();
    QTRY_VERIFY_WITH_TIMEOUT(window.isSettled(),10000);
    QTest::keyClick(&window,Qt::Key_F,Qt::ControlModifier);
    QCOMPARE(window.viewState().page,1);
    auto *search=window.findChild<QLineEdit*>("appsSearch");
    QVERIFY(search && search->hasFocus());
    auto *apps=window.findChild<QTableView*>("appsTable");
    const int count=apps->model()->rowCount();
    QVERIFY(count>0);
    QTest::keyClicks(search,"DISKLENS_NO_SUCH_PACKAGE_09324");
    QTest::keyClick(search,Qt::Key_Return);
    QCOMPARE(apps->model()->rowCount(),0);
    search->clear(); QCOMPARE(apps->model()->rowCount(),count);
    QTest::keyClick(&window,Qt::Key_3,Qt::AltModifier);
    QCOMPARE(window.viewState().page,2);
    auto *files=window.findChild<QTableView*>("filesTable");
    QCOMPARE(files->model()->rowCount(),0);
    QFile file(root.filePath("large.bin"));
    QVERIFY(file.open(QIODevice::WriteOnly)); QVERIFY(file.resize(128*1024*1024)); file.close();
    QTest::keyClick(&window,Qt::Key_F5);
    QTRY_COMPARE_WITH_TIMEOUT(files->model()->rowCount(),1,10000);
    QTest::keyClick(&window,Qt::Key_F,Qt::ControlModifier);
    QVERIFY(window.findChild<QLineEdit*>("filesSearch")->hasFocus());
    QTest::keyClick(&window,Qt::Key_1,Qt::AltModifier);
    QCOMPARE(window.viewState().page,0);
}

void AppearanceTest::overviewResponsiveLayout_data()
{
    QTest::addColumn<QString>("language"); QTest::addColumn<int>("size"); QTest::addColumn<int>("width");
    for(const QString &language : {QStringLiteral("en"),QStringLiteral("zh_CN")})
        for(int size : {13,18}) for(int width : {1080,1280})
            QTest::newRow(qPrintable(QStringLiteral("%1-%2-%3").arg(language).arg(size).arg(width))) << language << size << width;
}

void AppearanceTest::overviewResponsiveLayout()
{
    QFETCH(QString,language); QFETCH(int,size); QFETCH(int,width);
    AppearanceManager manager; QVERIFY(manager.apply({language,size}));
    QTemporaryDir root;
    MainWindow window; window.setScanRoot(root.path()); window.resize(width,860); window.show();
    QTRY_VERIFY_WITH_TIMEOUT(window.isSettled(),10000);
    QTest::qWait(80);
    auto *stack=window.findChild<QStackedWidget*>();
    auto *scroll=qobject_cast<QScrollArea*>(stack->currentWidget());
    QVERIFY(scroll);
    QVERIFY2(scroll->widget()->width()<=scroll->viewport()->width(),"Overview must fit without horizontal scrolling");
    auto *disk=window.findChild<QFrame*>("diskCard"); QVERIFY(disk);
    const auto values=scroll->widget()->findChildren<QLabel*>("metricValue");
    QCOMPARE(values.size(),2);
    for(auto *value:values) {
        QVERIFY(value->width()>=value->fontMetrics().horizontalAdvance(value->text()));
        const QPoint top=value->mapTo(scroll->widget(),QPoint());
        QVERIFY(top.x()>=0 && top.x()+value->width()<=scroll->widget()->width());
    }
    // Large-font cards wrap below the disk panel; wide default text stays alongside.
    const int diskBottom=disk->mapTo(scroll->widget(),disk->rect().bottomLeft()).y();
    const int valueTop=values.first()->mapTo(scroll->widget(),QPoint()).y();
    if(size==18 || width==1080) QVERIFY(valueTop>diskBottom);
    else QVERIFY(valueTop<diskBottom);
}

void AppearanceTest::settingsRoundTripAndInvalidFallback()
{
    QTemporaryDir root;
    QVERIFY(root.isValid());
    const QString path=root.filePath("preferences.ini");
    QSettings settings(path,QSettings::IniFormat);
    QCOMPARE(AppearanceManager::readSettings(settings).language,QStringLiteral("zh_CN"));
    QCOMPARE(AppearanceManager::readSettings(settings).fontSize,13);
    QVERIFY(AppearanceManager::saveSettings({QStringLiteral("en"),17},settings));
    QSettings reloaded(path,QSettings::IniFormat);
    QCOMPARE(AppearanceManager::readSettings(reloaded).language,QStringLiteral("en"));
    QCOMPARE(AppearanceManager::readSettings(reloaded).fontSize,17);
    QVERIFY(!AppearanceManager::saveSettings({QStringLiteral("xx"),100},settings));
    settings.setValue("appearance/language","invalid");
    settings.setValue("appearance/fontSize",999);
    QCOMPARE(AppearanceManager::readSettings(settings).language,QStringLiteral("zh_CN"));
    QCOMPARE(AppearanceManager::readSettings(settings).fontSize,13);
    QCOMPARE(AppearanceManager::scaledStyleSheet("font-size:13px; padding:13px;font-size:11px;",17),
        QStringLiteral("font-size:17px; padding:13px;font-size:14px;"));
}

void AppearanceTest::actualLanguageAndFontWithViewRestore()
{
    QTemporaryDir root;
    QVERIFY(root.isValid());
    QVERIFY(QDir().mkpath(root.filePath("explorer")));
    QVERIFY(QDir().mkpath(root.filePath("developer")));
    AppearanceManager manager;
    QString error;
    QVERIFY2(manager.apply({QStringLiteral("zh_CN"),13},false,&error),qPrintable(error));
    auto window=std::make_unique<MainWindow>();
    window->setScanRoot(root.path());
    window->resize(1080,750);
    auto navigation=window->findChildren<QPushButton*>("navButton");
    QCOMPARE(navigation.size(),6);
    navigation[5]->click();
    window->findChild<QLineEdit*>("explorerRoot")->setText(root.filePath("explorer"));
    window->findChild<QLineEdit*>("developerRoot")->setText(root.filePath("developer"));
    window->show();
    QTest::qWait(50);
    QCOMPARE(navigation[0]->text(),QStringLiteral("空间总览"));
    const int initialFont=navigation[0]->font().pixelSize();
    const int initialRow=window->findChild<QTableView*>("developerTable")->verticalHeader()->defaultSectionSize();
    const int initialColumn=window->findChild<QTableView*>("developerTable")->columnWidth(0);
    const auto state=window->viewState();
    window.reset();
    QVERIFY2(manager.apply({QStringLiteral("en"),17},false,&error),qPrintable(error));
    window=std::make_unique<MainWindow>();
    window->restoreViewState(state);
    window->show();
    QTest::qWait(80);
    navigation=window->findChildren<QPushButton*>("navButton");
    QVERIFY(navigation[0]->text()!=QStringLiteral("空间总览"));
    QCOMPARE(navigation[0]->text(),dlTr("空间总览"));
    QVERIFY(!navigation[0]->text().contains(QRegularExpression(QStringLiteral("[\\x{4e00}-\\x{9fff}]"))));
    QVERIFY(navigation[0]->font().pixelSize()>initialFont);
    QCOMPARE(qApp->property("disklensFontSize").toInt(),17);
    QCOMPARE(window->viewState().page,5);
    QCOMPARE(window->viewState().scanRoot,state.scanRoot);
    QCOMPARE(window->viewState().explorerRoot,state.explorerRoot);
    QCOMPARE(window->viewState().developerRoot,state.developerRoot);
    QCOMPARE(window->size(),QSize(1080,750));
    auto *table=window->findChild<QTableView*>("developerTable");
    QVERIFY(table);
    QVERIFY(table->verticalHeader()->defaultSectionSize()>initialRow);
    QVERIFY(table->columnWidth(0)>initialColumn);
    // A local per-widget font rule must grow too, rather than only the base font.
    auto *status=window->findChild<QLabel*>("developerStatus");
    QVERIFY(status);
    QCOMPARE(status->font().pixelSize(),14);
    QVERIFY(table->viewport()->height()>=50);
    const QString capture=qEnvironmentVariable("DISKLENS_TEST_CAPTURE");
    if(!capture.isEmpty()) QVERIFY(window->grab().save(capture));
    QVERIFY2(manager.apply({QStringLiteral("zh_CN"),13},false,&error),qPrintable(error));
}

void AppearanceTest::duplicateRequestRestored_data()
{
    QTest::addColumn<bool>("requested");
    QTest::addColumn<bool>("duplicateTab");
    QTest::addColumn<int>("page");
    QTest::newRow("tab-only-does-not-opt-in") << false << true << 4;
    QTest::newRow("completed-hash-directory-tab") << true << false << 4;
    QTest::newRow("completed-hash-inactive-page") << true << true << 5;
}
void AppearanceTest::duplicateRequestRestored()
{
    QFETCH(bool,requested);
    QFETCH(bool,duplicateTab);
    QFETCH(int,page);
    QTemporaryDir root;
    QVERIFY(root.isValid());
    for(const QString &name : {QStringLiteral("a.bin"),QStringLiteral("b.bin")}) {
        QFile file(root.filePath(name));
        QVERIFY(file.open(QIODevice::WriteOnly));
        QCOMPARE(file.write(QByteArray(4096,'a')),qint64(4096));
    }
    AppearanceManager manager;
    QVERIFY(manager.apply({QStringLiteral("zh_CN"),13}));
    auto window=std::make_unique<MainWindow>();
    window->setScanRoot(root.path());
    auto *explorer=window->findChild<ExplorerPage*>();
    explorer->setDuplicateMinimum(1024);
    window->findChildren<QPushButton*>("navButton")[4]->click();
    window->show();
    QTRY_VERIFY_WITH_TIMEOUT(explorer->isSettled(),10000);
    if(requested) {
        explorer->startDuplicateScan();
        QTRY_VERIFY_WITH_TIMEOUT(explorer->isSettled(),10000);
        QCOMPARE(explorer->findChild<QTreeWidget*>("explorerDuplicates")->topLevelItemCount(),1);
    }
    explorer->findChild<QTabWidget*>("explorerTabs")->setCurrentIndex(duplicateTab?1:0);
    window->findChildren<QPushButton*>("navButton")[page]->click();
    const auto state=window->viewState();
    QCOMPARE(state.duplicateRequested,requested);
    window.reset();
    QVERIFY(manager.apply({QStringLiteral("en"),15}));
    window=std::make_unique<MainWindow>();
    window->restoreViewState(state);
    window->show();
    explorer=window->findChild<ExplorerPage*>();
    QTRY_VERIFY_WITH_TIMEOUT(explorer->isSettled(),10000);
    QCOMPARE(explorer->duplicateScanRequested(),requested);
    QCOMPARE(explorer->duplicateMinimum(),quint64(1024));
    QCOMPARE(explorer->findChild<QTabWidget*>("explorerTabs")->currentIndex(),duplicateTab?1:0);
    QCOMPARE(window->viewState().page,page);
    QCOMPARE(explorer->findChild<QTreeWidget*>("explorerDuplicates")->topLevelItemCount(),requested?1:0);
}

void AppearanceTest::dialogApplyAndCancel()
{
    AppearanceManager manager;
    QVERIFY(manager.apply({QStringLiteral("zh_CN"),13}));
    MainWindow window;
    QTemporaryDir root;
    window.setScanRoot(root.path());
    window.show();
    QSignalSpy changes(&window,&MainWindow::appearanceChangeRequested);
    auto *entry=window.findChild<QPushButton*>("appearanceButton");
    QVERIFY(entry);
    QTimer::singleShot(0,&window,[&]{
        auto *dialog=window.findChild<QDialog*>("appearanceDialog");
        QVERIFY(dialog);
        auto *language=dialog->findChild<QComboBox*>("appearanceLanguage");
        auto *fontSize=dialog->findChild<QComboBox*>("appearanceFontSize");
        QVERIFY(language && fontSize);
        language->setCurrentIndex(language->findData("en"));
        fontSize->setCurrentIndex(fontSize->findData(17));
        dialog->findChild<QPushButton*>("appearanceApply")->click();
    });
    entry->click();
    QCOMPARE(changes.size(),1);
    QCOMPARE(changes[0][0].toString(),QStringLiteral("en"));
    QCOMPARE(changes[0][1].toInt(),17);
    QTimer::singleShot(0,&window,[&]{
        auto *dialog=window.findChild<QDialog*>("appearanceDialog");
        QVERIFY(dialog);
        dialog->findChild<QPushButton*>("appearanceCancel")->click();
    });
    entry->click();
    QCOMPARE(changes.size(),1);
    QCOMPARE(manager.preferences().language,QStringLiteral("zh_CN"));
}

void AppearanceTest::dialogPreview()
{
    AppearanceManager manager;
    QString error;
    QVERIFY2(manager.apply({QStringLiteral("en"),17},false,&error),qPrintable(error));
    MainWindow window;
    QTemporaryDir root;
    window.setScanRoot(root.path());
    window.show();
    QTimer::singleShot(0,&window,[&]{
        auto *dialog=window.findChild<QDialog*>("appearanceDialog");
        QVERIFY(dialog);
        const QString capture=qEnvironmentVariable("DISKLENS_DIALOG_CAPTURE");
        if(!capture.isEmpty()) QVERIFY(dialog->grab().save(capture));
        auto *preview=dialog->findChild<QLabel*>("appearancePreview");
        QVERIFY(preview);
        QCOMPARE(preview->font().pixelSize(),17);
        auto *fontSize=dialog->findChild<QComboBox*>("appearanceFontSize");
        fontSize->setCurrentIndex(fontSize->findData(13));
        QCOMPARE(preview->font().pixelSize(),13);
        dialog->findChild<QPushButton*>("appearanceCancel")->click();
    });
    window.findChild<QPushButton*>("appearanceButton")->click();
}

void AppearanceTest::explorerLargeFontLayout()
{
    QTemporaryDir root;
    QVERIFY(root.isValid());
    for(int i=0;i<8;++i) {
        const QString directory=root.filePath(QStringLiteral("directory-%1").arg(i+1));
        QVERIFY(QDir().mkpath(directory));
        QFile file(directory+QStringLiteral("/payload.bin"));
        QVERIFY(file.open(QIODevice::WriteOnly));
        QCOMPARE(file.write(QByteArray(4096*(i+1),'a')),qint64(4096*(i+1)));
    }
    AppearanceManager manager;
    QVERIFY(manager.apply({QStringLiteral("en"),18}));
    MainWindow window;
    window.setScanRoot(root.path());
    window.resize(1280,860);
    window.findChildren<QPushButton*>("navButton")[4]->click();
    window.show();
    auto *explorer=window.findChild<ExplorerPage*>();
    QTRY_VERIFY_WITH_TIMEOUT(explorer->isSettled(),10000);
    QTest::qWait(80);
    auto *tree=explorer->findChild<QTreeWidget*>("explorerTree");
    QVERIFY(tree);
    QCOMPARE(tree->topLevelItemCount(),8);
    // A translated total must not squeeze all directory names out of view.
    QVERIFY(tree->columnWidth(0)>=tree->fontMetrics().horizontalAdvance(QStringLiteral("directory-8"))+50);
    auto *tabs=explorer->findChild<QTabWidget*>("explorerTabs");
    auto *bar=tabs->findChild<QTabBar*>();
    QVERIFY(bar);
    QFont selectedTabFont=bar->font(); selectedTabFont.setWeight(QFont::DemiBold);
    const QFontMetrics selectedTabMetrics(selectedTabFont);
    for(int i=0;i<bar->count();++i) {
        QVERIFY(bar->tabRect(i).width()>=selectedTabMetrics.horizontalAdvance(bar->tabText(i))+32);
        QVERIFY(bar->tabRect(i).height()>=bar->fontMetrics().height()+12);
    }
    QVERIFY(!explorer->findChild<QWidget*>("explorerMap"));
    tree->topLevelItem(0)->setExpanded(true);
    QCOMPARE(tree->topLevelItem(0)->childCount(),1);
    const QImage chart=tree->viewport()->grab().toImage();
    const QString chartCapture=qEnvironmentVariable("DISKLENS_CHART_CAPTURE");
    if(!chartCapture.isEmpty()) QVERIFY(chart.save(chartCapture));
    // At the largest font, both allocation text and bars fit in parent and
    // expanded child cells. Inspect the production raster, including a scrolled row.
    for(auto *item : {tree->topLevelItem(0),tree->topLevelItem(0)->child(0),tree->topLevelItem(7)}) {
        tree->scrollToItem(item); QCoreApplication::processEvents();
        const QRect row=tree->visualItemRect(item);
        QVERIFY(row.height()>=tree->fontMetrics().height()+12);
        const QRect bounds(tree->columnViewportPosition(1),row.top(),tree->columnWidth(1),row.height());
        const QImage cell=tree->viewport()->grab(bounds).toImage();
        QVERIFY(tree->columnWidth(1)>=tree->fontMetrics().horizontalAdvance(item->text(1))+20);
        int ink=0,blue=0,inkBottom=-1,blueTop=cell.height();
        for(int y=0;y<cell.height();++y) for(int x=0;x<cell.width();++x) {
            const QColor pixel=cell.pixelColor(x,y);
            if(pixel.red()<100 && pixel.green()<125 && pixel.blue()<170) { ++ink; inkBottom=qMax(inkBottom,y); }
            if(pixel.blue()>pixel.red()+50 && pixel.blue()>pixel.green()+30) { ++blue; blueTop=qMin(blueTop,y); }
        }
        QVERIFY(ink>30);
        QVERIFY(blue>10);
        QVERIFY2(inkBottom<blueTop,"Large-font allocation text must fit above the inline bar.");
    }
    tree->setCurrentItem(tree->topLevelItem(0));
    QTest::qWait(30);
    auto *detail=explorer->findChild<QLabel*>("explorerDetail");
    QVERIFY(detail && detail->wordWrap());
    QVERIFY(detail->height()>=detail->fontMetrics().boundingRect(QRect(0,0,detail->width(),1000),Qt::TextWordWrap,detail->text()).height());
    const QString capture=qEnvironmentVariable("DISKLENS_EXPLORER_LAYOUT_CAPTURE");
    if(!capture.isEmpty()) QVERIFY(window.grab().save(capture));
}

void AppearanceTest::modalAndPackagePreviewBlockChanges()
{
    AppearanceManager manager;
    QVERIFY(manager.apply({QStringLiteral("zh_CN"),13}));
    MainWindow window;
    QTemporaryDir root;
    window.setScanRoot(root.path());
    window.show();
    QSignalSpy changes(&window,&MainWindow::appearanceChangeRequested);
    QVERIFY(window.canChangeAppearance());
    QDialog modal(&window);
    modal.setModal(true);
    modal.show();
    QTest::qWait(20);
    QVERIFY(!window.canChangeAppearance());
    QVERIFY(!window.requestAppearance({QStringLiteral("en"),15}));
    QVERIFY(changes.isEmpty());
    modal.hide();
    QVERIFY(window.canChangeAppearance());
    if (QFileInfo::exists(QStringLiteral("/usr/bin/dpkg-query"))) {
        auto *controller=window.findChild<UninstallController*>();
        QVERIFY(controller);
        AppEntry app;
        app.id=QStringLiteral("disklens-nonexistent-preview-fixture");
        app.removable=true;
        app.source=PackageSource::Deb;
        controller->preview(app); // Only starts the read-only metadata command.
        QVERIFY(controller->isBusy());
        QVERIFY(!controller->isExecuting());
        QVERIFY(!window.canChangeAppearance());
        QVERIFY(!window.requestAppearance({QStringLiteral("en"),15}));
        controller->cancelPreview();
        QVERIFY(window.canChangeAppearance());
        QVERIFY(changes.isEmpty());
    }
}

QTEST_MAIN(AppearanceTest)
#include "appearance_test.moc"
