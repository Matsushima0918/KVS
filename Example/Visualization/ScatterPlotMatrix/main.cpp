/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @author Naohisa Sakamoto
 *  @brief  Example program for kvs::ScatterPlotRenderer class.
 */
/*****************************************************************************/
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <kvs/TableObject>
#include <kvs/ScatterPlotMatrixRenderer>
#include <kvs/Axis2DMatrix>


/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument counter
 *  @param  argv [i] argument values
 *  @return true, if the main process is done succesfully
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    kvs::glut::Application app( argc, argv );
    kvs::glut::Screen screen( &app );
    screen.setTitle( "kvs::ScatterPlotMatrixRenderer" );

    const size_t nsamples = 50;
    const float min_value = 0.0f;
    const float max_value = 1.0f;
    const auto random_x = kvs::ValueArray<float>::Random( nsamples, min_value, max_value );
    const auto random_y = kvs::ValueArray<float>::Random( nsamples, min_value, max_value );
    const auto random_z = kvs::ValueArray<float>::Random( nsamples, min_value, max_value );

    auto* object = new kvs::TableObject();
    object->addColumn( kvs::AnyValueArray( random_x ), "X value" );
    object->addColumn( kvs::AnyValueArray( random_y ), "Y value" );
    object->addColumn( kvs::AnyValueArray( random_z ), "Z value" );

    object->setMinValue( 0, min_value );
    object->setMaxValue( 0, max_value );

    object->setMinValue( 1, min_value );
    object->setMaxValue( 1, max_value );

    object->setMinValue( 2, min_value );
    object->setMaxValue( 2, max_value );

    const int left = 65;
    const int top = 35;
    const int right = 30;
    const int bottom = 50;
    const kvs::Margins margins( left, top, right, bottom );

    auto* renderer = new kvs::ScatterPlotMatrixRenderer();
    renderer->setMargins( margins );
    renderer->setPointSize( 4.0f );

    auto* axis = new kvs::Axis2DMatrix();
    axis->setTitle( "Title" );
    axis->setMargins( margins );

    screen.registerObject( object, axis );
    screen.registerObject( object, renderer );
    screen.show();

    return app.run();
}
