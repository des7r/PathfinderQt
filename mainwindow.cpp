#include "mainwindow.h"
#include <QCloseEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QRandomGenerator>
#include <QIntValidator>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    createUi();
    restoreSettings();
    connectSignals();
}

MainWindow::~MainWindow()
{
    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait();
    }
    worker_ = nullptr;
}


void MainWindow::createUi()
{
    auto* leftPanel = new QWidget(this);
    auto* form = new QFormLayout(leftPanel);

    widthEdit_  = new QLineEdit(leftPanel);
    heightEdit_ = new QLineEdit(leftPanel);

    auto* validator = new QIntValidator(5, 100, this);
    widthEdit_->setValidator(validator);
    heightEdit_->setValidator(validator);
    widthEdit_->setText("20");
    heightEdit_->setText("20");

    densitySlider_ = new QSlider(Qt::Horizontal, leftPanel);
    densitySlider_->setRange(0, 100);
    densitySlider_->setValue(30);

    densityLabel_ = new QLabel("30%", leftPanel);

    generateButton_ = new QPushButton(tr("Сгенерировать"), leftPanel);
    clearButton_    = new QPushButton(tr("Очистить"), leftPanel);

    form->addRow(tr("Ширина:"),  widthEdit_);
    form->addRow(tr("Высота:"),  heightEdit_);
    form->addRow(tr("Плотность:"), densitySlider_);
    form->addRow(tr("Текущее:"), densityLabel_);
    form->addRow(generateButton_);
    form->addRow(clearButton_);

    gridScene_ = new GridScene(this);
    gridView_  = new GridView(this);
    gridView_->setScene(gridScene_);

    auto* central = new QWidget(this);
    auto* h = new QHBoxLayout(central);
    h->addWidget(leftPanel);
    h->addWidget(gridView_, 1);
    setCentralWidget(central);

    statusBar()->showMessage(tr("Готово"));
}

void MainWindow::restoreSettings()
{
    restoreGeometry(settings_.value("geometry").toByteArray());
}

void MainWindow::saveSettings()
{
    settings_.setValue("geometry", saveGeometry());
}

void MainWindow::connectSignals()
{
    connect(generateButton_, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(clearButton_,    &QPushButton::clicked, this, &MainWindow::onClear);

    connect(densitySlider_, &QSlider::valueChanged, this, [=](int val) {
        densityLabel_->setText(QString("%1%").arg(val));
    });

    connect(gridScene_, &GridScene::gridChanged, this, &MainWindow::onRecomputePreviewFromA);
    connect(gridScene_, &GridScene::startChanged, this, &MainWindow::onRecomputePreviewFromA);
    connect(gridScene_, &GridScene::endChanged,   this, &MainWindow::startPathWorker);
}


void MainWindow::closeEvent(QCloseEvent* e)
{
    saveSettings();
    QMainWindow::closeEvent(e);
}


void MainWindow::onGenerate()
{
    bool okW = false;
    bool okH = false;
    int w = widthEdit_->text().toInt(&okW);
    int h = heightEdit_->text().toInt(&okH);

    if (!okW || !okH) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введите числа для ширины и высоты"));
        return;
    }

    bool adjusted = false;
    if (w < 5 || h < 5) {
        w = std::max(w, 5);
        h = std::max(h, 5);
        adjusted = true;
    }
    if (w > 100 || h > 100) {
        w = std::min(w, 100);
        h = std::min(h, 100);
        adjusted = true;
    }

    if (adjusted) {
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Допустимый диапазон: 5..100.\n"
                                "Значения подправлены автоматически."));
        widthEdit_->setText(QString::number(w));
        heightEdit_->setText(QString::number(h));
    }

    QVector<QVector<bool>> walls(h);
    for (int y = 0; y < h; ++y) {
        walls[y] = QVector<bool>(w, false);
    }

    QRandomGenerator rng(QRandomGenerator::global()->generate());
    int density = densitySlider_->value();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            walls[y][x] = (rng.bounded(100) < density);
        }
    }

    gridScene_->buildGrid(walls);
    statusBar()->showMessage(tr("Сетка %1x%2 создана").arg(w).arg(h), 3000);

    onRecomputePreviewFromA();
}

void MainWindow::onClear()
{
    gridScene_->clearWallsAndPath();
    statusBar()->showMessage(tr("Сетка очищена"), 1500);
}

void MainWindow::startPathWorker()
{
    if (!gridScene_->hasStart() || !gridScene_->hasEnd())
        return;

    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait();
        worker_ = nullptr;
    }

    workerThread_ = new QThread(this);
    worker_ = new PathFinder(gridScene_->widthCells(), gridScene_->heightCells());
    worker_->setGrid(gridScene_->walls());
    worker_->setEndpoints(gridScene_->start(), gridScene_->end());
    worker_->moveToThread(workerThread_);

    connect(workerThread_, &QThread::started, worker_, &PathFinder::computeBfsPath);
    connect(worker_, &PathFinder::pathFound, this, &MainWindow::onPathReady);
    connect(worker_, &PathFinder::finished, workerThread_, &QThread::quit);
    connect(worker_, &PathFinder::finished, worker_, &PathFinder::deleteLater);
    connect(workerThread_, &QThread::finished, workerThread_, &QThread::deleteLater);
    connect(workerThread_, &QThread::finished, this, [this] { workerThread_ = nullptr; });

    workerThread_->start();
}

void MainWindow::onPathReady(const QVector<QPoint>& path)
{
    if (path.isEmpty()) {
        QMessageBox::information(this, tr("Нет пути"), tr("Путь не найден"));
    } else {
        gridScene_->setFinalPath(path);
        statusBar()->showMessage(tr("Путь найден: %1 шагов").arg(path.size()), 3000);
    }
}

void MainWindow::onRecomputePreviewFromA()
{
    if (!gridScene_->hasStart()) {
        gridScene_->clearPreview();
        return;
    }
    gridScene_->recomputePreviewTree();
}
