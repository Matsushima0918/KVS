/*****************************************************************************/
/**
 *  @file   ExtractVertices.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "ExtractVertices.h"
#include <kvs/DebugNew>
#include <kvs/File>
#include <kvs/PipelineModule>
#include <kvs/VisualizationPipeline>
#include <kvs/ExtractVertices>
#include <kvs/Application>
#include <kvs/Screen>
#include <kvs/Label>
#include <kvs/ColorMapBar>
#include <kvs/OrientationAxis>
#include <kvs/TargetChangeEvent>
#include "CommandName.h"
#include "FileChecker.h"


namespace kvsview
{

namespace ExtractVertices
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new Argument class.
 *  @param  argc [in] argument count
 *  @param  argv [in] argument values
 */
/*===========================================================================*/
Argument::Argument( int argc, char** argv ):
    kvsview::Argument::Common( argc, argv, "ExtractVertices")
{
    // Parameters for the isosurface class.
    addOption( kvsview::ExtractVertices::CommandName, kvsview::ExtractVertices::Description, 0 );
    addOption( "s", "Point size. (default: 1)", 1, false );
    addOption( "t", "Transfer function file. (optional: <filename>)", 1, false );
    addOption( "T", "Transfer function file with range adjustment. (optional: <filename>)", 1, false );
}

/*===========================================================================*/
/**
 *  @brief  Returns a transfer function.
 *  @param  volume [in] pointer to the volume data
 *  @return transfer function
 */
/*===========================================================================*/
const kvs::TransferFunction Argument::transferFunction( const kvs::VolumeObjectBase* volume )
{
    if ( this->hasOption("t") )
    {
        const auto filename = this->optionValue<std::string>("t");
        return( kvs::TransferFunction( filename ) );
    }
    else if ( this->hasOption("T") )
    {
        const auto filename = this->optionValue<std::string>("T");
        kvs::TransferFunction tfunc( filename );
        tfunc.adjustRange( volume );
        return( tfunc );
    }
    else
    {
        const size_t resolution = 256;
        return( kvs::TransferFunction( resolution ) );
    }
}

/*===========================================================================*/
/**
 *  @brief  Executes main process.
 */
/*===========================================================================*/
int Main::exec()
{
    // Parse specified arguments.
    kvsview::ExtractVertices::Argument arg( m_argc, m_argv );
    if( !arg.parse() ) { return (false); }

    // Viewer application.
    kvs::Application app( m_argc, m_argv );
    kvs::Screen screen( &app );
    screen.setSize( 512, 512 );
    screen.setTitle( kvsview::CommandName + " - " + ExtractVertices::CommandName );
    screen.show();

    // Check the input volume data.
    m_input_name = arg.value<std::string>();
    if ( !(kvsview::FileChecker::ImportableStructuredVolume( m_input_name ) ||
           kvsview::FileChecker::ImportableUnstructuredVolume( m_input_name ) ) )
    {
        kvsMessageError() << m_input_name << " is not volume data." << std::endl;
        return (false);
    }

    // Visualization pipeline.
    kvs::VisualizationPipeline pipe( m_input_name );
    pipe.import();

    // Verbose information.
    const kvs::Indent indent(4);
    if ( arg.verboseMode() )
    {
        pipe.object()->print( std::cout << std::endl << "IMPORTED OBJECT" << std::endl, indent );
    }

    // Create extract vertices module.
    const auto* volume = kvs::VolumeObjectBase::DownCast( pipe.object() );
    const auto tfunc = arg.transferFunction( volume );
    kvs::PipelineModule mapper( new kvs::ExtractVertices );
    const kvs::Real32 size = arg.size();
    mapper.get<kvs::ExtractVertices>()->setSize( size );
    mapper.get<kvs::ExtractVertices>()->setTransferFunction( tfunc );
    pipe.connect( mapper );

    // Execute the visualization pipeline.
    if ( !pipe.exec() )
    {
        kvsMessageError() << "Cannot execute the visulization pipeline." << std::endl;
        return (false);
    }
    screen.registerObject( &pipe );

    // Verbose information.
    if ( arg.verboseMode() )
    {
        pipe.object()->print( std::cout << std::endl << "RENDERERED OBJECT" << std::endl, indent );
        pipe.print( std::cout << std::endl << "VISUALIZATION PIPELINE" << std::endl, indent );
    }

    // Apply the specified parameters to the global and the visualization pipeline.
    arg.applyTo( screen, pipe );
    arg.applyTo( screen );

    // Label (fps).
    kvs::Label label( &screen );
    label.setMargin( 10 );
    label.anchorToTopLeft();
    label.screenUpdated(
        [&]()
        {
            const auto* r = screen.scene()->renderer();
            const auto f = kvs::String::From( r->timer().fps(), 4 );
            label.setText( std::string( "FPS: " + f ).c_str() );
        } );
    label.show();

    const auto min_value = static_cast<kvs::Real32>( volume->minValue() );
    const auto max_value = static_cast<kvs::Real32>( volume->maxValue() );

    // Colormap bar.
    kvs::ColorMapBar colormap_bar( &screen );
    colormap_bar.setWidth( 150 );
    colormap_bar.setHeight( 60 );
    colormap_bar.setColorMap( tfunc.colorMap() );
    if ( !tfunc.hasRange() )
    {
        colormap_bar.setRange( min_value, max_value );
    }
    colormap_bar.anchorToBottomRight();
    colormap_bar.show();

    // Orientation axis.
    kvs::OrientationAxis orientation_axis( &screen, screen.scene() );
    orientation_axis.setMargin( 0 );
    orientation_axis.setSize( 100 );
    orientation_axis.setAxisLength( 3.2f );
    orientation_axis.setBoxTypeToSolid();
    orientation_axis.enableAntiAliasing();
    orientation_axis.anchorToBottomLeft();
    orientation_axis.show();

    // Target change event
    kvs::TargetChangeEvent target_change_event;
    screen.addEvent( &target_change_event );

    return ( arg.clear(), app.run() );
}

} // end of namespace ExtractVertices

} // end of namespace kvsview
