#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <managershapes.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

constexpr auto MIN_SIDE = 20;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /// @name Работа с файлами. Сохранение и загрузка
    /// @{
    void saveToFile();
    void loadFromFile();
    /// @}

    /// @name Обработка событий мыши
    /// @{
    void MousePressEvent(QEvent *event);
    void MouseMoveEvent(QEvent *event);
    /// @}

    /// @name UI интерфейс
    /// @{
    QPushButton* pb_Rectangle;
    QPushButton* pb_Triangle;
    QPushButton* pb_Ellipse;
    QPushButton* pb_ConnectShape;
    QPushButton* pb_MoveShape;
    QPushButton* pb_DeleteShape;
    QPushButton* pb_Save;
    QPushButton* pb_Load;
    QWidget *PaintWidget;
    ManagerShapes* manager;
    /// @}

    QPainter painter;
    TypeShapes CurrentType = TypeShapes::Rectangle;

private:
    Ui::MainWindow *ui;


protected:
    /// @name Перехватываем события
    /// @{
    bool eventFilter(QObject *obj, QEvent *event) override;
    /// @}

};
#endif // MAINWINDOW_H
