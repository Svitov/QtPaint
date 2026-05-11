#include "shape.h"
#include <math.h>

void Shape::CreateShape(QPoint startPoint, QPoint finalPoint)
{
    /* Очищаем массив точек перед постройкой/перестройкой */
    this->clear();

    switch (m_Type) {
    case TypeShapes::Rectangle :
    {
        *this << QPoint(startPoint.x(), startPoint.y())
                << QPoint(finalPoint.x(), startPoint.y())
                << QPoint(finalPoint.x(), finalPoint.y())
                << QPoint(startPoint.x(), finalPoint.y());
        break;
    }
    case TypeShapes::Triangle :
    {
        *this << QPoint(startPoint.x() + (finalPoint.x() - startPoint.x()) / 2, startPoint.y())
                << QPoint(finalPoint.x(), finalPoint.y())
                << QPoint(startPoint.x(), finalPoint.y());
        break;
    }
    case TypeShapes::Ellipse :
    {
        int radiusX = (finalPoint.x() - startPoint.x()) / 2;
        int radiusY = (finalPoint.y() - startPoint.y()) / 2;

        int centerX = startPoint.x() + radiusX;
        int centerY = startPoint.y() + radiusY;
        int pointCount = 36;

        for (int i = 0; i < pointCount; ++i) {
            /* Высчитываем угол для каждой точки */
            double al = 2.0 * M_PI * i / pointCount;

            // Calculate x and y coordinates
            int x = centerX + static_cast<int>(radiusX * std::cos(al));
            int y = centerY + static_cast<int>(radiusY * std::sin(al));

            *this << QPoint(x, y);
        }
        break;
    }
    default:
        break;
    }
}

void Shape::EndResizeOrMove()
{
    m_StartPosition = this->boundingRect().topLeft();
    m_FinalPosition = this->boundingRect().bottomRight();

}
