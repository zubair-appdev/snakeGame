#include "mainwindow.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QKeyEvent>
#include <QFont>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , dir(Right)
    , gridSize(20)
    , gameOver(false)
    , score(0)
{
    setFixedSize(400, 400);

    gridWidth = width() / gridSize;
    gridHeight = height() / gridSize;

    // Initialize snake
    snake.append(QPoint(gridWidth / 2, gridHeight / 2));

    placeFood();

    // Place 10 obstacles randomly
    placeObstacles(10);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::gameLoop);
    timer->start(150);

    // Restart button (hidden initially)
    restartButton = new QPushButton("Restart", this);
    restartButton->setGeometry(width()/2 - 50, height()/2 - 15, 100, 30);
    restartButton->hide();
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::restartGame);
}

MainWindow::~MainWindow()
{
}

void MainWindow::placeFood()
{
    while (true)
    {
        int x = QRandomGenerator::global()->bounded(gridWidth);
        int y = QRandomGenerator::global()->bounded(gridHeight);
        QPoint p(x, y);

        if (!snake.contains(p) && !obstacles.contains(p))
        {
            food = p;
            break;
        }
    }
}

void MainWindow::placeObstacles(int count)
{
    obstacles.clear();

    QPoint center(gridWidth / 2, gridHeight / 2);  // Snake starts here

    while (obstacles.size() < count)
    {
        int x = QRandomGenerator::global()->bounded(gridWidth);
        int y = QRandomGenerator::global()->bounded(gridHeight);
        QPoint p(x, y);

        // Define a wider safe zone to prevent nearby obstacles
        int dx = abs(p.x() - center.x());
        int dy = abs(p.y() - center.y());
        bool inSafeZone = dx <= 3 && dy <= 3;

        // Avoid placing on center or near snake's initial area
        if (p == center || inSafeZone)
            continue;

        if (!obstacles.contains(p))
            obstacles.append(p);
    }
}





bool MainWindow::isCollision(const QPoint &point)
{
    if (obstacles.contains(point))
        return true;

    for (int i = 1; i < snake.size(); ++i)
    {
        if (snake[i] == point)
            return true;
    }

    return false;
}


void MainWindow::moveSnake()
{
    // Apply the pending direction change once per frame
    dir = pendingDir;

    if (gameOver)
        return;

    QPoint head = snake.first();

    switch (dir)
    {
    case Up:    head.ry() -= 1; break;
    case Down:  head.ry() += 1; break;
    case Left:  head.rx() -= 1; break;
    case Right: head.rx() += 1; break;
    }

    // Wrap around edges
    if (head.x() < 0) head.setX(gridWidth - 1);
    else if (head.x() >= gridWidth) head.setX(0);
    if (head.y() < 0) head.setY(gridHeight - 1);
    else if (head.y() >= gridHeight) head.setY(0);

    // Check collisions with self or obstacles
    if (isCollision(head))
    {
        gameOver = true;
        timer->stop();
        restartButton->show();
        return;
    }

    snake.prepend(head);

    if (head == food)
    {
        score += 10; // increase score on eating food
        placeFood();
    }
    else
    {
        snake.removeLast();
    }
}

void MainWindow::gameLoop()
{
    moveSnake();
    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Background
    painter.fillRect(rect(), Qt::black);

    // Draw food
    painter.setBrush(Qt::red);
    painter.drawRect(food.x() * gridSize, food.y() * gridSize, gridSize, gridSize);

    // Draw obstacles
    painter.setBrush(Qt::gray);
    for (const QPoint &p : obstacles)
    {
        painter.drawRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
    }

    // Draw snake
    painter.setBrush(Qt::green);
    for (const QPoint &p : snake)
    {
        painter.drawRect(p.x() * gridSize, p.y() * gridSize, gridSize, gridSize);
    }

    // Draw score text
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(10, 25, QString("Score: %1").arg(score));

    // Draw game over message
    if (gameOver)
    {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Game                   Over");
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (gameOver)
    {
        QMainWindow::keyPressEvent(event);
        return;
    }

    switch (event->key())
    {
    case Qt::Key_Up:
        if (dir != Down) pendingDir = Up;
        break;
    case Qt::Key_Down:
        if (dir != Up) pendingDir = Down;
        break;
    case Qt::Key_Left:
        if (dir != Right) pendingDir = Left;
        break;
    case Qt::Key_Right:
        if (dir != Left) pendingDir = Right;
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}


void MainWindow::restartGame()
{
    // Reset game state
    snake.clear();
    snake.append(QPoint(gridWidth / 2, gridHeight / 2));
    dir = Right;
    pendingDir = Right;
    score = 0;
    gameOver = false;
    placeFood();
    placeObstacles(10);
    restartButton->hide();
    timer->start(150);
    update();

    // Ensure main window receives key events again
    this->setFocus();
}

