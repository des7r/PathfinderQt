#include "gridscene.h"
#include <QGraphicsSceneMouseEvent>

static constexpr int CELL = 20;

GridScene::GridScene(QObject* parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 0, 0);
}

void GridScene::buildGrid(const QVector<QVector<bool>>& walls)
{
    clear();
    finalPathCells_.clear();
    start_ = end_ = QPoint(-1, -1);

    walls_ = walls;
    h_ = walls_.size();
    w_ = h_ ? walls_[0].size() : 0;

    rebuildGraphics();
    emit gridChanged();
}

void GridScene::clearWallsAndPath()
{
    if (w_ == 0 || h_ == 0) return;

    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) {
            walls_[y][x] = false;
        }
    }
    finalPathCells_.clear();
    start_ = end_ = QPoint(-1, -1);
    colorizeCells();
    emit gridChanged();
}

void GridScene::rebuildGraphics()
{
    items_.clear();
    items_.resize(h_);

    for (int y = 0; y < h_; ++y) {
        items_[y].resize(w_);
        for (int x = 0; x < w_; ++x) {
            auto* r = addRect(x * CELL, y * CELL, CELL, CELL, QPen(Qt::black));
            r->setBrush(Qt::white);
            r->setAcceptHoverEvents(true);
            items_[y][x] = r;
        }
    }

    setSceneRect(0, 0, w_ * CELL, h_ * CELL);
    colorizeCells();
}

void GridScene::colorizeCells()
{
    for (int y = 0; y < h_; ++y) {
        for (int x = 0; x < w_; ++x) {
            auto* r = items_[y][x];
            r->setBrush(walls_[y][x] ? QColor(50,50,50) : Qt::white);
        }
    }

    for (const QPoint& p : finalPathCells_) {
        if (inBounds(p.x(), p.y()))
            items_[p.y()][p.x()]->setBrush(QColor(200,230,255));
    }

    if (hasStart())
        items_[start_.y()][start_.x()]->setBrush(QColor(120,255,120));
    if (hasEnd())
        items_[end_.y()][end_.x()]->setBrush(QColor(255,140,140));
}

void GridScene::setFinalPath(const QVector<QPoint>& path)
{
    finalPathCells_.clear();
    for (const QPoint& p : path) {
        finalPathCells_.insert(p);
    }
    colorizeCells();
}

void GridScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF pos = event->scenePos();
    int x = int(pos.x()) / CELL;
    int y = int(pos.y()) / CELL;

    if (!inBounds(x, y)) return;

    if (event->button() == Qt::LeftButton) {
        if (!hasStart()) {
            start_ = QPoint(x, y);
            emit startChanged();
        } else if (!hasEnd()) {
            end_ = QPoint(x, y);
            emit endChanged();
        } else {
            start_ = QPoint(x, y);
            end_ = QPoint(-1, -1);
            emit startChanged();
        }
    } else if (event->button() == Qt::RightButton) {
        walls_[y][x] = !walls_[y][x];
        emit gridChanged();
    }

    finalPathCells_.clear();
    colorizeCells();

    QGraphicsScene::mousePressEvent(event);
}

void GridScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!hasStart()) return;

    QPointF pos = event->scenePos();
    int x = int(pos.x()) / CELL;
    int y = int(pos.y()) / CELL;

    if (!inBounds(x, y) || walls_[y][x])
        return;

    auto path = backtrackPreviewPath(QPoint(x, y));
    colorizeCells();

    for (const QPoint& p : path) {
        if (inBounds(p.x(), p.y()))
            items_[p.y()][p.x()]->setBrush(QColor(255,245,180));
    }

    if (hasStart())
        items_[start_.y()][start_.x()]->setBrush(QColor(120,255,120));
    if (hasEnd())
        items_[end_.y()][end_.x()]->setBrush(QColor(255,140,140));

    QGraphicsScene::mouseMoveEvent(event);
}

void GridScene::recomputePreviewTree()
{
    previewParents_.clear();
    previewParents_.resize(h_);
    for (int y = 0; y < h_; ++y) {
        previewParents_[y].resize(w_);
        for (int x = 0; x < w_; ++x) {
            previewParents_[y][x] = QPoint(-1, -1);
        }
    }

    if (!hasStart() || w_ == 0 || h_ == 0)
        return;

    QVector<QPoint> q;
    q.reserve(w_ * h_);
    previewParents_[start_.y()][start_.x()] = start_;
    q.push_back(start_);

    QVector<QPoint> directions = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    int qi = 0;
    while (qi < q.size()) {
        QPoint cur = q[qi++];
        for (const QPoint& d : directions) {
            int nx = cur.x() + d.x();
            int ny = cur.y() + d.y();
            if (!inBounds(nx, ny) || walls_[ny][nx])
                continue;
            if (previewParents_[ny][nx] == QPoint(-1, -1)) {
                previewParents_[ny][nx] = cur;
                q.push_back(QPoint(nx, ny));
            }
        }
    }
}

QVector<QPoint> GridScene::backtrackPreviewPath(const QPoint& target) const
{
    QVector<QPoint> path;
    if (!inBounds(target.x(), target.y()) || previewParents_.isEmpty())
        return path;

    if (previewParents_[target.y()][target.x()] == QPoint(-1, -1))
        return path;

    QPoint cur = target;
    while (cur != start_) {
        path.push_back(cur);
        cur = previewParents_[cur.y()][cur.x()];
        if (!inBounds(cur.x(), cur.y()) || cur == QPoint(-1, -1)) {
            path.clear();
            break;
        }
    }
    if (!path.isEmpty())
        path.push_back(start_);
    return path;
}

void GridScene::clearPreview()
{
    previewParents_.clear();
    colorizeCells();
}
