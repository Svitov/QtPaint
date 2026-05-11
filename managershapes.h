#ifndef MANAGERSHAPES_H
#define MANAGERSHAPES_H

#include <shape.h>

/**
 * @brief Состояния работы менеджера фигур.
 *
 * Описывает все возможные этапы жизненного цикла менеджера фигур.
 */
enum StateMachine
{
    ReadyDraw,              ///< Готов к рисованию
    Draw,                   ///< Режим рисования/изменения размеров фигур
    CreateConnect,          ///< Режим поиска первой фигуры связи
    SearchShapeConnect,     ///< Режим поиска второй фигуры связи
    SelectShape,            ///< Режим выбора фигуры
    MoveShape,              ///< Режим передвижения фигуры
    DeleteShape             ///< Режим удаления фигуры
};

/**
 * @brief Класс управления фигурами.
 *
 * Этот класс предоставляет менеджера фигур,
 * который выполняет обработку команд от главного интерфейса и
 * выполняет действия с фигурами согласно своему состоянию
 *
 * @author Свитов
 * @version 1.0
 * @date 2026-05-07
 */
class ManagerShapes
{
public:
    ManagerShapes();
    StateMachine m_StateMachine = StateMachine::ReadyDraw;

    /**
     * @brief Метод строит фигуру в зависимости от заданной начальной и конечной точки.
     * @param startPoint Начальная точка.
     * @param finalPoint Конечная точка.
     * @param type       Тип фигуры.
     */
    void CreateShape(QPoint startPoint, QPoint finalPoint, TypeShapes type);
    /**
     * @brief Метод перестройки фигуры в зависимости от заданнойконечной точки.
     * @param finalPoint Конечная точка.
     */
    void ResizeShape(QPoint finalPoint);
    /**
     * @brief Метод для фиксации текущих изменений фигуры.
     */
    void EndResizeOrMove();
    /**
     * @brief Метод для сброса фигуры.
     */
    void ResetMove();
    /**
     * @brief Метод поиск фигуры в заданной точке для создания связи.
     * @param point Заданная точка.
     */
    void SearchShapeConnect(QPoint point);
    /**
     * @brief Метод для сброса связи.
     */
    void ResetConnect();
    /**
     * @brief Метод поиск фигуры в заданной точке.
     * @param point Заданная точка.
     */
    bool FindShape(QPoint point);
    /**
     * @brief Метод смещения фигуры в зависимости от заданной точки.
     * @param point Заданная точка.
     */
    void MoveShape(QPoint point);

    QPoint StartMove = QPoint(0, 0);
    QList<std::shared_ptr<Shape>> v_Shapes;
    std::weak_ptr<Shape> m_CurrentShape;


};

#endif // MANAGERSHAPES_H
