#include "pathfinder.h"
#include <algorithm>

void PathFinder::computeBfsPath()
{
    QVector<QPoint> path;

    if (w_ <= 0 || h_ <= 0 || !inBounds(a_.x(), a_.y()) || !inBounds(b_.x(), b_.y())) {
        emit pathFound(path);
        emit finished();
        return;
    }

    if (a_ == b_) {
        path.push_back(a_);
        emit pathFound(path);
        emit finished();
        return;
    }

    QVector<QVector<bool>> visited;
    visited.resize(h_);
    for (int y = 0; y < h_; ++y) {
        visited[y].resize(w_);
        for (int x = 0; x < w_; ++x) {
            visited[y][x] = false;
        }
    }

    QVector<QVector<QPoint>> parent;
    parent.resize(h_);
    for (int y = 0; y < h_; ++y) {
        parent[y].resize(w_);
        for (int x = 0; x < w_; ++x) {
            parent[y][x] = QPoint(-1, -1);
        }
    }

    QVector<QPoint> q;
    q.reserve(w_ * h_);

    visited[a_.y()][a_.x()] = true;
    parent[a_.y()][a_.x()] = a_;
    q.push_back(a_);

    QVector<QPoint> directions = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    int qi = 0;
    bool found = false;

    while (qi < q.size() && !found) {
        QPoint cur = q[qi++];
        for (const QPoint& d : directions) {
            int nx = cur.x() + d.x();
            int ny = cur.y() + d.y();

            if (!inBounds(nx, ny) || walls_[ny][nx] || visited[ny][nx])
                continue;

            visited[ny][nx] = true;
            parent[ny][nx] = cur;
            q.push_back(QPoint(nx, ny));

            if (QPoint(nx, ny) == b_) {
                found = true;
                break;
            }
        }
    }

    if (found) {
        QPoint cur = b_;
        while (cur != a_) {
            path.push_back(cur);
            cur = parent[cur.y()][cur.x()];
        }
        path.push_back(a_);
        std::reverse(path.begin(), path.end());
    }

    emit pathFound(path);
    emit finished();
}
