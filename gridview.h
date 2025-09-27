#pragma once
#include <QGraphicsView>
#include <QWheelEvent>

// виджет поля с поддержкой зума колесиком
class GridView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GridView(QWidget* parent = nullptr) : QGraphicsView(parent) {
        setMouseTracking(true);
    }

protected:
    void wheelEvent(QWheelEvent* e) override {
        double factor = (e->angleDelta().y() > 0) ? 1.15 : 1.0 / 1.15;
        QPointF posInScene = mapToScene(e->position().toPoint());
        scale(factor, factor);
        QPointF delta = mapToScene(e->position().toPoint()) - posInScene;
        translate(delta.x(), delta.y());
    }
};
