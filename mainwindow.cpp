#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <shape.h>
#include <QWidgetAction>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->installEventFilter(this);
    /* Создаем управляющий фигурами класс */
    manager = new ManagerShapes();


    /* Создаем кнопки и связи */
    pb_Rectangle = new QPushButton("Прямоугольник", ui->centralwidget);
    pb_Rectangle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_Rectangle, &QPushButton::clicked, this, [this](){this->CurrentType = TypeShapes::Rectangle; manager->m_StateMachine = StateMachine::ReadyDraw; });

    pb_Triangle = new QPushButton("Треугольник", ui->centralwidget);
    pb_Triangle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_Triangle, &QPushButton::clicked, this, [this](){this->CurrentType = TypeShapes::Triangle; manager->m_StateMachine = StateMachine::ReadyDraw; });

    pb_Ellipse = new QPushButton("Эллипс", ui->centralwidget);
    pb_Ellipse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_Ellipse, &QPushButton::clicked, this, [this](){this->CurrentType = TypeShapes::Ellipse; manager->m_StateMachine = StateMachine::ReadyDraw; });

    pb_ConnectShape = new QPushButton("Связь", ui->centralwidget);
    pb_ConnectShape->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_ConnectShape, &QPushButton::clicked, this, [this](){manager->m_StateMachine = StateMachine::CreateConnect; });

    pb_MoveShape = new QPushButton("Перемещение", ui->centralwidget);
    pb_MoveShape->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_MoveShape, &QPushButton::clicked, this, [this](){manager->m_StateMachine = StateMachine::SelectShape; });

    pb_DeleteShape = new QPushButton("Удаление", ui->centralwidget);
    pb_DeleteShape->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_DeleteShape, &QPushButton::clicked, this, [this](){manager->m_StateMachine = StateMachine::DeleteShape;});

    pb_Save = new QPushButton("Сохранить", ui->centralwidget);
    pb_Save->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_Save, &QPushButton::clicked, this, [this](){this->saveToFile(); });

    pb_Load = new QPushButton("Загразить", ui->centralwidget);
    pb_Load->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(pb_Load, &QPushButton::clicked, this, [this](){this->loadFromFile(); });


    /* Создаем и настраиваем UI */
    PaintWidget = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout(ui->centralwidget);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    /* Добавляем кнопки в горизонтальный слой */
    auto createSep = [] {QFrame *line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        return line;
    };

    buttonLayout->addWidget(pb_Rectangle);
    buttonLayout->addWidget(pb_Triangle);
    buttonLayout->addWidget(pb_Ellipse);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createSep());
    buttonLayout->addStretch();
    buttonLayout->addWidget(pb_ConnectShape);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createSep());
    buttonLayout->addStretch();
    buttonLayout->addWidget(pb_MoveShape);
    buttonLayout->addWidget(pb_DeleteShape);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createSep());
    buttonLayout->addStretch();
    buttonLayout->addWidget(pb_Save);
    buttonLayout->addWidget(pb_Load);

    /* Добавляем виджет для рисования */
    layout->addLayout(buttonLayout); // Сначала кнопки (будут сверху)
    layout->addWidget(PaintWidget);
    PaintWidget->setStyleSheet("border: 2px solid red;");
    PaintWidget->installEventFilter(this);
    PaintWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    PaintWidget->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveToFile()
{
    QJsonArray shapesArray;

    /* Заполняем массив с данными фигур */
    for (const auto& shape : manager->v_Shapes) {
        QJsonObject obj;
        obj["type"] = shape->getType();

        obj["StartX"] = shape->getStartPosition().x();
        obj["StartY"] = shape->getStartPosition().y();

        obj["FinalX"] = shape->getFinalPosition().x();
        obj["FinalY"] = shape->getFinalPosition().y();

        /* Проверяем наличие связи между фигурами */
        if(!shape->m_OtherShape.isEmpty())
        {
            QJsonArray index;
            for(auto otherShape : shape->m_OtherShape)
            {
                if(auto lockedShape = otherShape.lock())
                {
                    index.append(manager->v_Shapes.indexOf(lockedShape));
                }
            }
            obj["indexOtherShape"] = index;
        }
        else
        {
            obj["indexOtherShape"] = -100;

        }

        shapesArray.append(obj);
    }

    QJsonDocument doc(shapesArray);
    QFile file("data");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }

}

void MainWindow::loadFromFile()
{
    QFile file("data");
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    /* Читаем все данные из файла */
    QByteArray data = file.readAll();
    file.close();

    /* Превращаем байты в JSON-документ */
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }

    QJsonArray shapesArray = doc.array();
    if(!shapesArray.isEmpty())
    {
        manager->v_Shapes.clear();

        /* Проходим по массиву и создаем фигуры */
        for (int i = 0; i < shapesArray.size(); ++i)
        {
            QJsonObject obj = shapesArray[i].toObject();
            manager->CreateShape(QPoint(obj["StartX"].toInt(), obj["StartY"].toInt()), QPoint(obj["FinalX"].toInt(), obj["FinalY"].toInt()), static_cast<TypeShapes>(obj["type"].toInt()));
            manager->v_Shapes.last()->EndResizeOrMove();
        }
        /* Проходим и устанавливаем связи */
        for (int i = 0; i < shapesArray.size(); ++i)
        {
            QJsonObject obj = shapesArray[i].toObject();

            QJsonArray readArray = obj["indexOtherShape"].toArray();
            for (const QJsonValue &value : readArray)
            {
                manager->v_Shapes[i]->m_OtherShape.append(manager->v_Shapes.at(value.toInt()));
            }
        }

        PaintWidget->update();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    /* Проверяем, что событие — это отрисовка и цель — наш myCanvas */
    if (obj == PaintWidget)
    {
        switch (event->type()) {
        case QEvent::Paint :
        {
            /* Ограничиваем область рисования */
            QPainter painter(PaintWidget);

            /* Включаем сглаживание */
            painter.setRenderHint(QPainter::Antialiasing);

            /* отрисовываем фигуры и свзяи */
            for (auto polygon : manager->v_Shapes) {
                painter.drawPolygon(*polygon);
                if(polygon->m_OtherPoint != QPoint(-100, -100))
                {
                    painter.drawLine(polygon->boundingRect().center(), polygon->m_OtherPoint);
                }
                for(auto otherShape : polygon->m_OtherShape)
                {
                    if(auto lockedShape = otherShape.lock())
                    {
                        painter.drawLine(polygon->boundingRect().center(), lockedShape->boundingRect().center());
                    }
                }
            }

            return true;
        }
        case QEvent::MouseButtonPress :
        {
            MousePressEvent(event);
            break;
        }
        case QEvent::MouseMove :
        {
            MouseMoveEvent(event);
            break;
        }
        case QEvent::MouseButtonRelease :
        {
            if(manager->m_StateMachine == StateMachine::Draw)
            {
                manager->m_StateMachine = StateMachine::ReadyDraw;
            }
            if(manager->m_StateMachine == StateMachine::MoveShape)
            {
                manager->m_StateMachine = StateMachine::SelectShape;
            }

            manager->EndResizeOrMove();

            this->unsetCursor();

            return true;
        }
        case QEvent::KeyPress :
        {
            /* Приводим базовое событие к событию кнопки клавиатуры */
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Escape)
            {
                if(manager->m_StateMachine == StateMachine::Draw)
                {
                    manager->v_Shapes.removeLast();
                    PaintWidget->update();
                }
                if(manager->m_StateMachine == StateMachine::MoveShape)
                {
                    manager->m_StateMachine = StateMachine::SelectShape;
                    manager->ResetMove();
                    PaintWidget->update();
                }
                if(manager->m_StateMachine == StateMachine::SearchShapeConnect)
                {
                    manager->m_StateMachine = StateMachine::CreateConnect;
                    manager->ResetConnect();
                    PaintWidget->update();
                }

                return true;
            }
            break;
        }
        default:
            break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}
void MainWindow::MousePressEvent(QEvent *event)
{
    /* Приводим базовое событие к событию мыши */
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    switch (mouseEvent->button()) {
    case Qt::RightButton :
    {
        if(manager->m_StateMachine == StateMachine::Draw)
        {
            manager->v_Shapes.removeLast();
            PaintWidget->update();
        }
        if(manager->m_StateMachine == StateMachine::MoveShape)
        {
            manager->m_StateMachine = StateMachine::SelectShape;
            manager->ResetMove();
            PaintWidget->update();
        }
        if(manager->m_StateMachine == StateMachine::SearchShapeConnect)
        {
            manager->m_StateMachine = StateMachine::CreateConnect;
            manager->ResetConnect();
            PaintWidget->update();
        }
        break;
    }
    case Qt::LeftButton :
    {
        if(manager->m_StateMachine == StateMachine::ReadyDraw)
        {
            manager->m_StateMachine = StateMachine::Draw;
            manager->CreateShape(mouseEvent->pos(), QPoint(mouseEvent->pos().x() + MIN_SIDE, mouseEvent->pos().y() + MIN_SIDE), CurrentType);

            PaintWidget->update();
        }
        if(manager->m_StateMachine == StateMachine::SelectShape)
        {
            this->setCursor(Qt::ClosedHandCursor);
            manager->FindShape(mouseEvent->pos());
        }
        if(manager->m_StateMachine == StateMachine::DeleteShape)
        {
            manager->FindShape(mouseEvent->pos());
            PaintWidget->update();
        }
        if(manager->m_StateMachine == StateMachine::CreateConnect)
        {
            manager->FindShape(mouseEvent->pos());
            PaintWidget->update();
        }

        if(manager->m_StateMachine == StateMachine::SearchShapeConnect)
        {
            manager->FindShape(mouseEvent->pos());
            PaintWidget->update();
        }

        break;
    }
    default:
        break;
        break;
    }
}

void MainWindow::MouseMoveEvent(QEvent *event)
{
    /* Приводим базовое событие к событию мыши */
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    switch (manager->m_StateMachine) {
    case StateMachine::Draw :
    {
        manager->ResizeShape(mouseEvent->pos());
        PaintWidget->update();

        break;
    }
    case StateMachine::MoveShape :
    {
        manager->MoveShape(mouseEvent->pos());
        PaintWidget->update();

        break;
    }
    case StateMachine::SearchShapeConnect :
    {
        manager->SearchShapeConnect(mouseEvent->pos());
        PaintWidget->update();

        break;
    }
    default:
        break;
    }
}
