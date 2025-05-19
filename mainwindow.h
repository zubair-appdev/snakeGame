#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPoint>
#include <QPushButton>
#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();
    void restartGame();

private:
    enum Direction { Up, Down, Left, Right };
    Direction dir;
    Direction pendingDir;
    QVector<QPoint> snake;
    QPoint food;
    QVector<QPoint> obstacles;
    QTimer *timer;
    int gridSize;
    int gridWidth;
    int gridHeight;
    bool gameOver;
    int score;
    QPushButton *restartButton;

    void placeFood();
    void placeObstacles(int count);
    void moveSnake();
    bool isCollision(const QPoint &point);
};

#endif // MAINWINDOW_H
