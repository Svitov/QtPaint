#include "managershapes.h"

ManagerShapes::ManagerShapes()
{

}

void ManagerShapes::CreateShape(QPoint startPoint, QPoint finalPoint, TypeShapes type)
{
    v_Shapes.append(std::make_shared<Shape>(type, startPoint));
    v_Shapes.last()->CreateShape(startPoint, finalPoint);

}

//void ManagerShapes::CreateShape(QPoint startPoint, QPoint finalPoint, TypeShapes type, int index)
//{
//    if(index >= 0 & index < v_Shapes.size())
//    {
//        v_Shapes[index] = std::make_shared<Shape>(type, startPoint);
//        v_Shapes[index]->CreateShape(startPoint, finalPoint);
//        v_Shapes[index]->EndResizeOrMove();
//    }
//}

void ManagerShapes::ResizeShape(QPoint finalPoint)
{
    if(!v_Shapes.isEmpty())
    {
        v_Shapes.last()->CreateShape(v_Shapes.last()->getStartPosition(), finalPoint);
    }
}

void ManagerShapes::EndResizeOrMove()
{
    if (!v_Shapes.isEmpty())
    {
        v_Shapes.last()->EndResizeOrMove();
    }
}

void ManagerShapes::ResetMove()
{
    if (!v_Shapes.isEmpty())
    {
        v_Shapes.last()->ResetMove();
    }
}

void ManagerShapes::SearchShapeConnect(QPoint point)
{
    auto lockedShape = m_CurrentShape.lock();
    if (lockedShape) {
        lockedShape->m_OtherPoint = point;
    }
}

void ManagerShapes::ResetConnect()
{
    m_StateMachine = StateMachine::CreateConnect;
    auto lockedShape = m_CurrentShape.lock();
    if (lockedShape) {
        lockedShape->m_OtherPoint = QPoint(-100, -100);
    }
}

bool ManagerShapes::FindShape(QPoint point)
{
    if(!v_Shapes.isEmpty())
    {
        std::shared_ptr<Shape> tempShape = nullptr;

        for(auto shape : v_Shapes)
        {
            if(shape->containsPoint(point, Qt::OddEvenFill))
            {
                tempShape = shape;
            }
        }

        if(tempShape)
        {
            switch (m_StateMachine) {
            case StateMachine::SelectShape :
            {
                int index = v_Shapes.indexOf(tempShape);

                v_Shapes.takeAt(index);
                v_Shapes.append(tempShape);

                m_StateMachine = StateMachine::MoveShape;
                StartMove = point;
                return 1;
            }
            case StateMachine::DeleteShape :
            {
                v_Shapes.removeOne(tempShape);

                return 1;
            }
            case StateMachine::CreateConnect :
            {
                m_CurrentShape = tempShape;
                auto lockedShape = m_CurrentShape.lock();
                if (lockedShape) {
                    lockedShape->m_OtherPoint = point;
                }
                m_StateMachine = StateMachine::SearchShapeConnect;

                return 1;
            }
            case StateMachine::SearchShapeConnect :
            {
                auto lockedShape = m_CurrentShape.lock();

                if(lockedShape != tempShape)
                {
                    if (lockedShape) {
                        lockedShape->m_OtherPoint = QPoint(-100, -100);
                        lockedShape->m_OtherShape = tempShape;
                    }
                    m_StateMachine = StateMachine::CreateConnect;
                }
                return 1;
            }

            default:
                break;
            }
        }
    }

    if(m_StateMachine == StateMachine::SearchShapeConnect)
    {
        ResetConnect();
    }
    return 0;
}

void ManagerShapes::MoveShape(QPoint point)
{
    if(!v_Shapes.isEmpty())
    {
        v_Shapes.last()->translate(point - StartMove);
        StartMove = point;
    }
}
