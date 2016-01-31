/*
 * File:   Form.cpp
 * Author: sova
 */

#include "Form.h"
#include <QDebug>
#include <QHash>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <cstdlib>

const int CIRCLE_WIDTH = 30;
const int CIRCLE_OFFSET = CIRCLE_WIDTH / 2;
const int CELL_SIZE = 70;
const int CELL_SIZE_OFFSET = CELL_SIZE / 2;
const int CELLS_X = 9;
const int CELLS_Y = 6;

Form::Form() {
    widget.setupUi(this);
    connect(widget.pushSolve, SIGNAL(clicked()),
            this, SLOT(solve()));
    connect(widget.pushClear, SIGNAL(clicked()),
            this, SLOT(clear()));
    connect(widget.pushStop, SIGNAL(clicked()),
            this, SLOT(stop()));
    connect(widget.pushNext, SIGNAL(clicked()),
            this, SLOT(next()));
    connect(widget.pushPrevious, SIGNAL(clicked()),
            this, SLOT(previous()));

    scene = widget.graphicsView->scene();
    if (scene == NULL) {
        scene = new QGraphicsScene();
        widget.graphicsView->setScene(scene);
    }
    ellipse1 = NULL;
    stopMethod = false;
    init();
}

void Form::init() {
    for (int i = 0; i <= CELLS_Y; i++) {
        scene->addLine(0, CELL_SIZE * i, CELL_SIZE * CELLS_X - 1, CELL_SIZE * i);
    }
    for (int i = 0; i <= CELLS_X; i++) {
        scene->addLine(CELL_SIZE * i, 0, CELL_SIZE * i, CELL_SIZE * CELLS_Y - 1);
    }
}

inline int Form::getX(int x) {
    int modx = x % CELL_SIZE;
    x -= modx + ((modx <= CELL_SIZE_OFFSET) ? 0 : -CELL_SIZE);
    return x;
}

inline int Form::getY(int y) {
    int mody = y % CELL_SIZE;
    y -= mody + ((mody <= CELL_SIZE_OFFSET) ? 0 : -CELL_SIZE);
    return y;
}

void Form::mousePressEvent(QMouseEvent* e) {
    if (!widget.pushStop->isEnabled()) {
        int x = e->x() - widget.graphicsView->x();
        int y = e->y() - widget.graphicsView->y();
        x = getX(x);
        y = getY(y);
        if ((x > CELL_SIZE_OFFSET) && (y > CELL_SIZE_OFFSET) && (x < (CELL_SIZE * CELLS_X - CELL_SIZE_OFFSET)) && (y < (CELL_SIZE * CELLS_Y - CELL_SIZE_OFFSET))) {
            if (widget.pushNext->isEnabled() || widget.pushPrevious->isEnabled()) {
                clearPaths();
            }
            QGraphicsEllipseItem* ellipse = findQGraphicsEllipseItem(x, y);
            if (widget.radioVertices->isChecked()) {
                if (ellipse != NULL) {
                    deleteEllipse(ellipse);
                } else {
                    drawEllipse(x, y);
                }
            } else {
                if (ellipse != NULL) {
                    if (ellipse1 == NULL) {
                        setEllipseAsActive(ellipse);
                    } else {
                        if (ellipse1 == ellipse) {
                            setEllipseAsNonActive(ellipse);
                        } else {
                            int x2 = ellipse1->rect().x() + CIRCLE_OFFSET;
                            int y2 = ellipse1->rect().y() + CIRCLE_OFFSET;
                            QGraphicsLineItem* line = findQGraphicsLineItem(x, y, x2, y2);
                            if (line == NULL) {
                                setEllipseAsNonActive(ellipse1);
                                drawLine(x, y, x2, y2);
                            } else {
                                deleteLine(line);
                            }
                        }
                    }
                }
            }
        }
    }
}

QGraphicsEllipseItem* Form::findQGraphicsEllipseItem(int x, int y) {
    for (int i = 0; i < ellipses.size(); i++) {
        QGraphicsEllipseItem* ellipse = ellipses[i];
        int x0 = ellipse->rect().x();
        int y0 = ellipse->rect().y();
        if (((x0 + CIRCLE_OFFSET) == x) && ((y0 + CIRCLE_OFFSET) == y)) {
            return ellipse;
        }
    }
    return NULL;
}

QGraphicsLineItem* Form::findQGraphicsLineItem(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < lines.size(); i++) {
        QGraphicsLineItem* line = lines[i];
        int ix1 = line->line().x1();
        int iy1 = line->line().y1();
        int ix2 = line->line().x2();
        int iy2 = line->line().y2();
        if ((x1 == ix1) && (y1 == iy1) && (x2 == ix2) && (y2 == iy2)) {
            return line;
        }
        if ((x1 == ix2) && (y1 == iy2) && (x2 == ix1) && (y2 == iy1)) {
            return line;
        }
    }
    return NULL;
}

void Form::drawEllipse(int x, int y) {
    if (widget.pushSolve->isEnabled()) {
        QBrush brush;
        brush.setColor(Qt::red);
        brush.setStyle(Qt::SolidPattern);
        QGraphicsEllipseItem* item = scene->addEllipse(x - CIRCLE_OFFSET, y - CIRCLE_OFFSET, CIRCLE_WIDTH, CIRCLE_WIDTH, Qt::SolidLine, brush);
        ellipses.push_back(item);
    }
}

void Form::drawLine(int x, int y, int x2, int y2) {
    if (widget.pushSolve->isEnabled()) {
        QBrush brushLine;
        brushLine.setColor(Qt::blue);
        brushLine.setStyle(Qt::SolidPattern);
        QPen pen;
        pen.setBrush(brushLine);
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(9);
        QGraphicsLineItem* item = scene->addLine(x, y, x2, y2, pen);
        lines.push_back(item);
    }
}

void Form::deleteEllipse(QGraphicsEllipseItem* item) {
    for (int j = 0; j < ellipses.size(); j++) {
        if (item == ellipses[j]) {
            int ix = item->rect().x() + CIRCLE_OFFSET;
            int iy = item->rect().y() + CIRCLE_OFFSET;
            ellipses.remove(j);
            scene->removeItem(item);
            ellipse1 = NULL;

            QVector<QGraphicsLineItem*> deleteItems;
            for (int k = 0; k < lines.size(); k++) {
                QLineF qline = lines[k]->line();
                if (((qline.x1() == ix) && (qline.y1() == iy)) ||
                        ((qline.x2() == ix) && (qline.y2() == iy))) {
                    deleteItems.push_back(lines[k]);
                }
            }
            foreach(QGraphicsLineItem* it, deleteItems) {
                deleteLine(it);
            }
            break;
        }
    }
}

void Form::deleteLine(QGraphicsLineItem* item) {
    for (int j = 0; j < lines.size(); j++) {
        if (item == lines[j]) {
            lines.remove(j);
            scene->removeItem(item);
            if (ellipse1 != NULL) {
                setEllipseAsNonActive(ellipse1);
            }
            break;
        }
    }
}

void Form::selectLine() {
    QGraphicsLineItem* item = lines[success[0][activeLine]];
    if (item != NULL) {
        QPen pen1;
        pen1.setColor(Qt::green);
        pen1.setStyle(Qt::SolidLine);
        pen1.setWidth(9);
        item->setPen(pen1);
    }
}

void Form::unselectLine() {
    QGraphicsLineItem* item = lines[success[0][activeLine]];
    unselectLine(item);
}

void Form::unselectLine(QGraphicsLineItem* item) {
    QPen pen1;
    pen1.setColor(Qt::blue);
    pen1.setStyle(Qt::SolidLine);
    pen1.setWidth(9);
    item->setPen(pen1);
}

void Form::setEllipseAsActive(QGraphicsEllipseItem* item) {
    QBrush brush;
    brush.setColor(Qt::green);
    brush.setStyle(Qt::SolidPattern);
    item->setBrush(brush);
    ellipse1 = item;
}

void Form::setEllipseAsNonActive(QGraphicsEllipseItem* item) {
    QBrush brush;
    brush.setColor(Qt::red);
    brush.setStyle(Qt::SolidPattern);
    item->setBrush(brush);
    ellipse1 = NULL;
}

void Form::mouseMoveEvent(QMouseEvent*/* e*/) {
//    int x = e->x() - widget.graphicsView->x();
//    int y = e->y() - widget.graphicsView->y();
//    x = getX(x);
//    y = getY(y);
//    statusBar()->showMessage(QString("%1 - %2").arg(x).arg(y));
}

void Form::solve() {
//    QTimer* timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(calculate()));
//    timer->setSingleShot(true);
//    timer->start(10);
    calculate();
}

void Form::calculate() {
    QString title = QString::fromUtf8("Solve");
    QString question = QString::fromUtf8("Solve?");
    int answer =
            QMessageBox::question(this, title, question, QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        clearPaths();
        widget.pushClear->setDisabled(true);
        widget.pushSolve->setDisabled(true);
        widget.pushNext->setDisabled(true);
        widget.pushPrevious->setDisabled(true);
        widget.radioEdges->setDisabled(true);
        widget.radioVertices->setDisabled(true);
        widget.pushStop->setEnabled(true);
        statusBar()->showMessage(QString("calculating..."));
        success.clear();
        const int SIZE = ellipses.size();
        Matrix = new int [SIZE * SIZE];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Matrix[SIZE * i + j] = -1;
            }
        }
        int ex, ey, e1, e2, lx1, lx2, ly1, ly2;
        for (int i = 0; i < lines.size(); i++) {
            lx1 = lines[i]->line().x1();
            lx2 = lines[i]->line().x2();
            ly1 = lines[i]->line().y1();
            ly2 = lines[i]->line().y2();
            e1 = -1, e2 = -1;
            for (int j = 0; j < ellipses.size(); j++) {
                ex = ellipses[j]->rect().x() + CIRCLE_OFFSET;
                ey = ellipses[j]->rect().y() + CIRCLE_OFFSET;
                if ((ex == lx1) && (ey == ly1)) {
                    e1 = j;
                }
                if ((ex == lx2) && (ey == ly2)) {
                    e2 = j;
                }
                if ((e1 != -1) && (e2 != -1)) {
                    Matrix[SIZE * e1 + e2] = i;
                    Matrix[SIZE * e2 + e1] = i;
                    break;
                }
            }
        }
        QVector<int> path;
        for (int i = 0; i < ellipses.size(); i++) {
            nextStep(i, path);
        }
        delete[] Matrix;
        Matrix = NULL;
        statusBar()->showMessage(QString("finished"), 5000);
        widget.pushClear->setDisabled(false);
        widget.pushSolve->setDisabled(false);
        widget.pushNext->setEnabled(false);
        widget.pushPrevious->setEnabled(false);
        widget.radioEdges->setDisabled(false);
        widget.radioVertices->setDisabled(false);
        widget.pushStop->setEnabled(false);
        stopMethod = false;
        QString title = QString::fromUtf8("Result");
        QString textValid = QString::fromUtf8("Success paths: %1").arg(success.size());
        QString textInvalid = QString::fromUtf8("No any paths");
        if (success.size() > 0) {
            QMessageBox::information(NULL, title, textValid);
            if (lines.size() > 0) {
                widget.pushNext->setEnabled(true);
                activeLine = 0;
                selectLine();
            }
        } else {
            QMessageBox::warning(NULL, title, textInvalid);
        }
    }
}

void Form::nextStep(int step, QVector<int> path) {
    if (stopMethod) {
        return;
    }
    QVector<int> possible;
    QHash<int, int> paths;
    foreach (int item, path) {
        paths.insert(item, 0);
    }
    const int SIZE = ellipses.size();
    int line;
    for (int i = 0; i < SIZE; i++) {
        line = Matrix[SIZE * i + step];
        if (line > -1) {
            if (!paths.contains(line)) {
                possible.push_back(i);
            }
        }
    }
    if (possible.size() == 0) {
        if (path.size() == lines.size()) {
//            qDebug() << path;
            success.push_back(path);
        }
        return;
    }
    const int SIZE2 = possible.size();
    for (int i = 0; i < SIZE2; i++) {
        QVector<int> path1(path);
        path1.push_back(Matrix[SIZE * possible[i] + step]);
        nextStep(possible[i], path1);
    }
    QCoreApplication::processEvents();
}

void Form::clear() {
    QString title = QString::fromUtf8("Clear");
    QString question = QString::fromUtf8("Clear form?");
    int answer =
            QMessageBox::question(this, title, question, QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        foreach (QGraphicsEllipseItem* ellipse, ellipses) {
            deleteEllipse(ellipse);
        }
        clearPaths();
        widget.radioVertices->setChecked(true);
    }
}

void Form::stop() {
    stopMethod = true;
}

void Form::closeEvent(QCloseEvent *event) {
    stopMethod = true;
    event->accept();
}

void Form::next() {
    widget.pushPrevious->setEnabled(true);
    if ((activeLine + 1) < lines.size()) {
        unselectLine();
        activeLine++;
        selectLine();
    }
    if ((activeLine + 1) == lines.size()) {
        widget.pushNext->setDisabled(true);
    }
}

//void Form::current() {
//    widget.pushPrevious->setEnabled(true);
//    if ((activeLine + 1) < lines.size()) {
//        unselectLine();
//        activeLine++;
//        selectLine();
//    }
//    if ((activeLine + 1) == lines.size()) {
//        widget.pushNext->setDisabled(true);
//    }
//}

void Form::previous() {
    widget.pushNext->setEnabled(true);
    if (activeLine > 0) {
        unselectLine();
        activeLine--;
        selectLine();
    }
    if (activeLine == 0) {
        widget.pushPrevious->setDisabled(true);
    }
}

void Form::clearPaths() {
    widget.pushNext->setDisabled(true);
    widget.pushPrevious->setDisabled(true);
    success.clear();
    activeLine = 0;
    foreach (QGraphicsLineItem* item, lines) {
        unselectLine(item);
    }
    foreach (QGraphicsEllipseItem* item, ellipses) {
        setEllipseAsNonActive(item);
    }
}

//void Form::animate() {
//    if (!widget.pushPrevious->isEnabled() && !widget.pushNext->isEnabled()) {
//        return;
//    }
//    if (!widget.pushPrevious->isEnabled()) {
//        activeLine = 0;
//        selectLine();
//        QTimer::singleShot(1000, this, SLOT(animate()));
//    } else {
//        if (widget.pushNext->isEnabled()) {
//            next();
//            QTimer::singleShot(1000, this, SLOT(animate()));
//        }
//    }
//}
