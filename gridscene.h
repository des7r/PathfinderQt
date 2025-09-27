#pragma once
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QVector>
#include <QSet>
#include <QPoint>

// GridScene управляет сеткой, хранит стены, точки A/B, путь и подсветку
// обрабатывает клики мыши для установки точек и рисует путь
class GridScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GridScene(QObject* parent = nullptr);

    // построение новой сетки из булевой матрицы (false — свободно, true — стена)
    void buildGrid(const QVector<QVector<bool>>& walls);

    // очистка всех стен и найденного пути без изменения размеров сетки
    void clearWallsAndPath();

    // доступ к данным для поиска пути
    const QVector<QVector<bool>>& walls() const {
        return walls_;
    }
    int widthCells()  const {
        return w_;
    }
    int heightCells() const {
        return h_;
    }

    // работа с точками A и B
    bool hasStart() const {
        return start_.x() >= 0;
    }
    bool hasEnd()   const {
        return end_.x()   >= 0;
    }
    QPoint start() const {
        return start_;
    }
    QPoint end()   const {
        return end_;
    }

    // установка финального пути после поиска
    void setFinalPath(const QVector<QPoint>& path);

    // предварительное дерево BFS для подсветки пути при наведении мыши
    void recomputePreviewTree();
    void clearPreview();

signals:
    void gridChanged();
    void startChanged();
    void endChanged();

protected:
    // установка A/B и переключение стен
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    // подсветка кратчайшего пути от A до ячейки под курсором
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void rebuildGraphics();   // создание графических элементов ячеек
    void colorizeCells();     // закрашивание ячеек с учётом стен, пути, A и B
    bool inBounds(int x, int y) const {
        return x >= 0 && y >=0 && x < w_ && y < h_;
    }

    // Восстановление пути из дерева предварительного поиска
    QVector<QPoint> backtrackPreviewPath(const QPoint& target) const;

    int w_ = 0;
    int h_ = 0;
    QVector<QVector<bool>> walls_; // true = стена
    QVector<QVector<QGraphicsRectItem*>> items_;

    QPoint start_{-1,-1}; // точка A
    QPoint end_{-1,-1};   // точка B

    QSet<QPoint> finalPathCells_;
    QVector<QVector<QPoint>> previewParents_; // подсветка при наведении
};
