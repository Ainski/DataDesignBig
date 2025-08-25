#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QSet>
#include <QPair>
#include <QVector>
#include <QPointF>
#include <QString>
#include <QGraphicsEllipseItem>
class QWheelEvent;

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = nullptr);

    void clearGraph();
    bool loadFromFile(const QString &path);
    void loadFromText(const QString &edgeListText);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawGraph(const QSet<int> &nodes, const QVector<QPair<int,int>> &edges);
    void applyCircularLayout(const QSet<int> &nodes, QMap<int, QPointF> &nodeToPos,
                             double radius = 200.0, const QPointF &center = QPointF(0,0));

private:
    QGraphicsScene *m_scene;
    QMap<int, QGraphicsEllipseItem*> m_nodeItems;
};

#endif // GRAPHVIEW_H


