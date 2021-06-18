/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Example program for drawing multiple screens.
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include <kvs/Application>
#include <kvs/Screen>
#include <kvs/PaintEventListener>
#include <kvs/OpenGL>


/*===========================================================================*/
/**
 *  @brief  Drawing a triangle.
 */
/*===========================================================================*/
class TrianglePaintEvent : public kvs::PaintEventListener
{
    void update()
    {
        glBegin( GL_TRIANGLES );
        glColor3ub( 255,   0,   0 ); glVertex3d(  0.0,  3.0, 0.0 );
        glColor3ub(   0, 255,   0 ); glVertex3d(  3.0, -3.0, 0.0 );
        glColor3ub(   0,   0, 255 ); glVertex3d( -3.0, -3.0, 0.0 );
        glEnd();
    }
};

/*===========================================================================*/
/**
 *  @brief  Drawing a rectangle.
 */
/*===========================================================================*/
class RectanglePaintEvent : public kvs::PaintEventListener
{
    void update()
    {
        glBegin( GL_QUADS );
        glColor3ub( 255,   0,   0 ); glVertex3d(  3.0,  3.0, 0.0 );
        glColor3ub(   0, 255,   0 ); glVertex3d(  3.0, -3.0, 0.0 );
        glColor3ub(   0,   0, 255 ); glVertex3d( -3.0, -3.0, 0.0 );
        glColor3ub( 255, 255,   0 ); glVertex3d( -3.0,  3.0, 0.0 );
        glEnd();
    }
};

/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument count
 *  @param  argv [i] argument values
 *  @return true, if the main process is done succesfully
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    kvs::Application app( argc, argv );

    // User specified events.
    TrianglePaintEvent  triangle_paint_event;
    RectanglePaintEvent rectangle_paint_event;

    // Screen for drawing a triangle.
    kvs::Screen screen1( &app );
    screen1.addEvent( &triangle_paint_event );
    screen1.setTitle( "Triangle" );
    screen1.create();

    // Screen for drawing a rectangle.
    kvs::Screen screen2( &app );
    screen2.addEvent( &rectangle_paint_event );
    screen2.setTitle( "Rectangle" );
    screen2.create();

    return app.run();
}
