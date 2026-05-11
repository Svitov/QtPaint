#ifndef SHAPE_H
#define SHAPE_H

#include <QPolygon>
#include <memory>

/**
 * @brief Доступные фигуры.
 *
 * Описывает все возможные фигуры на данный момент.
 */
enum TypeShapes : int
{
    Rectangle,          ///< Прямоугольник
    Triangle,           ///< Треугольник
    Ellipse             ///< Эллипс
};

/**
 * @brief Класс фигуры.
 *
 * Этот класс предоставляет универсальный объект для создания
 *  и редактирования фигур
 *
 * @author Свитов
 * @version 1.0
 * @date 2026-05-07
 */
class Shape : public QPolygon
{

public:
    /**
     * @brief Конструктор фигуры.
     * @param type Тип фигуры.
     * @param StartPosition Начальная точка отрисовки.
     */
    explicit Shape(TypeShapes type, QPoint StartPosition) : m_Type(type), m_StartPosition(StartPosition){};

    /**
     * @brief Строит фигуру.
     *
     * Метод строит фигуру в зависимости от заданной начальной и конечной точки.
     *
     * @param startPoint Начальная точка.
     * @param finalPoint Конечная точка.
     */
    void CreateShape(QPoint startPoint, QPoint finalPoint);

    /**
     * @brief Окончание передвижения/изменения размеров фигуры.
     *
     * Метод фиксирует начальную и конечную точки.
     */
    void EndResizeOrMove();
    /**
     * @brief Окончание передвижения.
     *
     * Метод строит в изначальной точке.
     */
    void ResetMove(){CreateShape(m_StartPosition,m_FinalPosition); };
    /**
     * @brief Получение начачльной точки.
     *
     * Метод безопасного получения начальной точки.
     */
    QPoint getStartPosition() const { return m_StartPosition; }
    /**
     * @brief Получение конечной точки.
     *
     * Метод безопасного получения конечной точки.
     *
     * @return QPoint, Возвращает конечную точку.
     */
    QPoint getFinalPosition() const { return m_FinalPosition; }


    std::weak_ptr<Shape> m_OtherShape;
    QPoint m_OtherPoint = QPoint(-100, -100);
    TypeShapes getType() const { return m_Type; }


private:

    TypeShapes m_Type = TypeShapes::Rectangle;
    QPoint m_StartPosition = QPoint(-100, -100);
    QPoint m_FinalPosition = QPoint(-100, -100);


};

#endif // SHAPE_H
