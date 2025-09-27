#pragma once
#include <QMainWindow>
#include <QSettings>
#include <QThread>

#include "gridview.h"
#include "gridscene.h"
#include "pathfinder.h"

class QLineEdit;
class QSlider;
class QLabel;
class QPushButton;

// главное окно - слева панель управления, справа поле.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* e) override;

private slots:
    void onGenerate();
    void onClear();
    void onPathReady(const QVector<QPoint>& path);
    void onRecomputePreviewFromA();

private:
    void createUi();
    void restoreSettings();
    void saveSettings();
    void connectSignals();
    void startPathWorker();

    QLineEdit* widthEdit_ = nullptr;
    QLineEdit* heightEdit_ = nullptr;
    QSlider* densitySlider_ = nullptr;
    QLabel* densityLabel_ = nullptr;
    QPushButton* generateButton_ = nullptr;
    QPushButton* clearButton_ = nullptr;
    GridScene* gridScene_ = nullptr;
    GridView* gridView_ = nullptr;
    QThread* workerThread_ = nullptr;
    PathFinder* worker_ = nullptr;
    QSettings settings_{"Acme", "PathfinderQtTest"};
};
