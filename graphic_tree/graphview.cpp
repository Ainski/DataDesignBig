#include "graphview.h"

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QFile>
#include <QTextStream>
#include <QWheelEvent>
#include <QtMath>
#include <QPen>
#include <QBrush>
#include <QDebug>

GraphView::GraphView(QWidget *parent)
    : QGraphicsView(parent), m_scene(new QGraphicsScene(this))
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(QColor(248,248,248));
}

void GraphView::clearGraph()
{
    m_scene->clear();
    m_nodeItems.clear();
}

bool GraphView::loadFromFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        clearGraph();
        return false;
    }
    QTextStream in(&f);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    qDebug() << content << '\n';
    f.close();
    loadFromText(content);
    return true;
}

void GraphView::loadFromText(const QString &edgeListText)
{
    QSet<int> nodes;
    QVector<QPair<int,int>> edges;

    const QStringList lines = edgeListText.split("\n", Qt::KeepEmptyParts);
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();
        qDebug() << "line: " << line << '\n';
        if (line.isEmpty()) continue;
        if (line.startsWith('#')) continue;

        const QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        qDebug() << "parts.size: " << parts.size() << '\n';

        bool ok1 = false, ok2 = false;
        int u = parts.at(0).toInt(&ok1);
        int v = parts.at(1).toInt(&ok2);
        if (!ok1 || !ok2) continue;
        qDebug() << "u: " << u << " v: " << v << '\n';
        edges.append(qMakePair(u, v));
        nodes.insert(u);
        nodes.insert(v);
    }

    drawGraph(nodes, edges);
}

void GraphView::drawGraph(const QSet<int> &nodes, const QVector<QPair<int,int>> &edges)
{
    clearGraph();

    // layout positions
    QMap<int, QPointF> nodeToPos;
    applyCircularLayout(nodes, nodeToPos, 220.0, QPointF(0,0));

    // draw edges first
    QPen edgePen(QColor(50,50,50));
    edgePen.setWidthF(1.5);
    for (const auto &e : edges) {
        if (!nodeToPos.contains(e.first) || !nodeToPos.contains(e.second)) continue;
        const QPointF a = nodeToPos.value(e.first);
        const QPointF b = nodeToPos.value(e.second);
        m_scene->addLine(QLineF(a, b), edgePen);
    }

    // draw nodes
    const double r = 14.0;
    QBrush nodeBrush(QColor(66, 133, 244));
    QPen nodePen(QColor(255,255,255));
    nodePen.setWidthF(1.2);

    for (auto it = nodeToPos.constBegin(); it != nodeToPos.constEnd(); ++it) {
        const int id = it.key();
        const QPointF c = it.value();
        auto *circle = m_scene->addEllipse(c.x() - r, c.y() - r, 2*r, 2*r, nodePen, nodeBrush);
        circle->setFlag(QGraphicsItem::ItemIsSelectable, true);
        m_nodeItems.insert(id, circle);

        auto *label = m_scene->addSimpleText(QString::number(id));
        QFont f = label->font();
        f.setPointSize(8);
        f.setBold(true);
        label->setFont(f);
        QRectF br = label->boundingRect();
        label->setPos(c.x() - br.width()/2.0, c.y() - br.height()/2.0);
        label->setBrush(Qt::white);
        label->setZValue(10);
    }

    QRectF itemsRect = m_scene->itemsBoundingRect().adjusted(-40, -40, 40, 40);
    m_scene->setSceneRect(itemsRect);
    qDebug() << "items count:" << m_scene->items().size() << " sceneRect:" << itemsRect;
    resetTransform();
    fitInView(itemsRect, Qt::KeepAspectRatio);
    centerOn(itemsRect.center());
    viewport()->update();
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (!m_scene->items().isEmpty()) {
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void GraphView::applyCircularLayout(const QSet<int> &nodes, QMap<int, QPointF> &nodeToPos,
                                    double radius, const QPointF &center)
{
    QList<int> ids = nodes.values();
    std::sort(ids.begin(), ids.end());
    const int n = ids.size();
    if (n == 0) return;
    for (int i = 0; i < n; ++i) {
        const double theta = (2.0 * M_PI * i) / n;
        const double x = center.x() + radius * std::cos(theta);
        const double y = center.y() + radius * std::sin(theta);
        nodeToPos.insert(ids[i], QPointF(x, y));
    }
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0/scaleFactor, 1.0/scaleFactor);
    }
    event->accept();
}


