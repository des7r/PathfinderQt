#pragma once
#include <QObject>
#include <QPoint>
#include <QVector>

// поиск кратчайшего пути BFS в отдельном потоке
class PathFinder : public QObject {
    Q_OBJECT
public:
    explicit PathFinder(int w, int h, QObject* parent = nullptr)
        : QObject(parent), w_(w), h_(h) {}

    void setGrid(const QVector<QVector<bool>>& walls) {
        walls_ = walls;
    }
    void setEndpoints(const QPoint& a, const QPoint& b) {
        a_ = a;
        b_ = b;
    }

public slots:
    void computeBfsPath();

signals:
    void pathFound(const QVector<QPoint>& path);
    void finished();

private:
    bool inBounds(int x, int y) const {
        return x >= 0 && y >= 0 && x < w_ && y < h_;
    }

    int w_, h_;
    QVector<QVector<bool>> walls_;
    QPoint a_{-1,-1}, b_{-1,-1};
};
