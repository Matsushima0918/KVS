/*****************************************************************************/
/**
 *  @file   Axis2D.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "Axis2D.h"
#include "ValueAxis.h"
#include <kvs/OpenGL>
#include <kvs/Camera>
#include <kvs/Light>
#include <kvs/ObjectBase>
#include <kvs/String>
#include <kvs/RGBColor>
#include <kvs/RGBAColor>
#include <kvs/TableObject>
#include <kvs/UIColor>
#include <utility>
#include <vector>


namespace
{

const std::vector<std::pair<GLint,GLushort>> StripplePatterns(
    {
        { 1, 0xFFFF }, // Solid
        { 1, 0xF0F0 }, // Dash
        { 2, 0xAAAA }, // Dot
        { 2, 0xFAFA }  // DashDot
    } );
}

namespace kvs
{

/*===========================================================================*/
/**
 *  @brief  Constructs a new Axis2D class.
 */
/*===========================================================================*/
Axis2D::Axis2D():
    m_margins( 30 ),
    m_title( "" ),
    m_title_font( kvs::Font::Sans, kvs::Font::Bold, 22.0f ),
    m_title_offset( 5 ),
    m_background_visible( true ),
    m_background_color( kvs::UIColor::Gray5() ),
    m_border_visible( true ),
    m_border_color( kvs::UIColor::Label() ),
    m_border_width( 2 ),
    m_gridline_visible( false ),
    m_gridline_pattern( Solid ),
    m_gridline_color( kvs::UIColor::Background() ),
    m_gridline_width( 2 ),
    m_ngridlines( 5, 5 )
{
    m_axes.push_back( new kvs::ValueAxis( kvs::ValueAxis::Bottom ) ); // default x-axis
    m_axes.push_back( new kvs::ValueAxis( kvs::ValueAxis::Left ) ); // default y-axis
}

/*===========================================================================*/
/**
 *  @brief  Destroys the Axis2D class.
 */
/*===========================================================================*/
Axis2D::~Axis2D()
{
    for ( auto axis : m_axes ) { if ( axis ) delete axis; }
}

/*===========================================================================*/
/**
 *  @brief  Render 2D axes.
 *  @param  object [in] pointer to object
 *  @param  camera [in] pointer to camera
 *  @param  light [in] pointer ot light
 */
/*===========================================================================*/
void Axis2D::exec( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light )
{
    kvs::TableObject* table = kvs::TableObject::DownCast( object );

    BaseClass::startTimer();

    kvs::OpenGL::WithPushedAttrib attrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
    m_painter.begin( screen() );
    {
        const float dpr = camera->devicePixelRatio();
        const int width = camera->windowWidth();
        const int height = camera->windowHeight();
        const kvs::Rectangle rect = m_margins.content( width, height );

        this->updateAxes( table );
        this->drawBackground( rect, dpr );
        this->drawGridlines( rect, dpr );
        this->drawBorder( rect, dpr );
        this->drawAxes( rect );
        this->drawTitle( rect );
    }
    m_painter.end();

    BaseClass::stopTimer();
}

/*===========================================================================*/
/**
 *  @brief  Draw the axis title
 *  @param  rect [in] plot region
 */
/*===========================================================================*/
void Axis2D::drawTitle( const kvs::Rectangle& rect )
{
    if ( m_title.size() > 0 )
    {
        const float x0 = rect.x0();
        const float x1 = rect.x1();
        const float y0 = rect.y0();
        const kvs::FontMetrics& metrics = m_painter.fontMetrics();
        const kvs::Font font = m_painter.font();
        {
            m_painter.setFont( m_title_font );
            const float px = ( x0 + x1 - metrics.width( m_title ) ) * 0.5f;
            const float py = y0 - m_title_offset;
            m_painter.drawText( kvs::Vec2( px, py ), m_title );
        }
        m_painter.setFont( font );
    }
}

/*===========================================================================*/
/**
 *  @brief  Draw background of the plot region.
 *  @param  rect [in] plot region
 *  @param  dpr [in] device pixel ratio
 */
/*===========================================================================*/
void Axis2D::drawBackground( const kvs::Rectangle& rect, const float dpr )
{
    if ( !m_background_visible ) { return; } // invisible

    if ( m_background_color.a() > 0.0f )
    {
        kvs::OpenGL::WithPushedAttrib attrib( GL_CURRENT_BIT | GL_ENABLE_BIT );
        kvs::OpenGL::Enable( GL_BLEND );
        kvs::OpenGL::SetBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        const float x0 = rect.x0();
        const float x1 = rect.x1();
        const float y0 = rect.y0();
        const float y1 = rect.y1();
        kvs::OpenGL::Begin( GL_QUADS );
        kvs::OpenGL::Color( m_background_color );
        kvs::OpenGL::Vertex( kvs::Vec2( x0, y0 ) * dpr );
        kvs::OpenGL::Vertex( kvs::Vec2( x1, y0 ) * dpr );
        kvs::OpenGL::Vertex( kvs::Vec2( x1, y1 ) * dpr );
        kvs::OpenGL::Vertex( kvs::Vec2( x0, y1 ) * dpr );
        kvs::OpenGL::End();
    }
}

/*===========================================================================*/
/**
 *  @brief  Draw border of the plot region.
 *  @param  rect [in] plot region
 *  @param  dpr [in] device pixel ratio
 */
/*===========================================================================*/
void Axis2D::drawBorder( const kvs::Rectangle& rect, const float dpr )
{
    if ( !m_border_visible ) { return; } // invisible

    if ( m_border_color.a() > 0.0f )
    {
        const float x0 = rect.x0();
        const float x1 = rect.x1();
        const float y0 = rect.y0();
        const float y1 = rect.y1();
        const int d = int( m_border_width * 0.5 );
        kvs::OpenGL::SetLineWidth( m_border_width * dpr );
        kvs::OpenGL::Begin( GL_LINES );
        kvs::OpenGL::Color( m_border_color );
        kvs::OpenGL::Vertices( kvs::Vec2( x0 - d, y1 ) * dpr, kvs::Vec2( x1 + d, y1 ) * dpr ); // bottom
        kvs::OpenGL::Vertices( kvs::Vec2( x0, y1 + d ) * dpr, kvs::Vec2( x0, y0 - d ) * dpr ); // left
        kvs::OpenGL::Vertices( kvs::Vec2( x0 - d, y0 ) * dpr, kvs::Vec2( x1 + d, y0 ) * dpr ); // top
        kvs::OpenGL::Vertices( kvs::Vec2( x1, y1 + d ) * dpr, kvs::Vec2( x1, y0 - d ) * dpr ); // right
        kvs::OpenGL::End();
    }
}

/*===========================================================================*/
/**
 *  @brief  Draw gridlines.
 *  @param  rect [in] plot region
 *  @param  dpr [in] device pixel ratio
 */
/*===========================================================================*/
void Axis2D::drawGridlines( const kvs::Rectangle& rect, const float dpr )
{
    if ( !m_gridline_visible ) { return; } // invisible

    if ( m_gridline_width > 0.0f )
    {
        const float x0 = rect.x0();
        const float x1 = rect.x1();
        const float y0 = rect.y0();
        const float y1 = rect.y1();
        const GLint stipple_factor = ::StripplePatterns[m_gridline_pattern].first;
        const GLint stipple_pattern = ::StripplePatterns[m_gridline_pattern].second;

        kvs::OpenGL::WithPushedAttrib attrib( GL_CURRENT_BIT | GL_ENABLE_BIT );

        kvs::OpenGL::Enable( GL_LINE_STIPPLE );
        kvs::OpenGL::SetLineStipple( stipple_factor, stipple_pattern );

        kvs::OpenGL::SetLineWidth( m_gridline_width );
        kvs::OpenGL::Color( m_gridline_color );
        kvs::OpenGL::Begin( GL_LINES );

        // Vertical gridlines.
        float x = x0 * dpr;
        const float dx = ( ( x1 - x0 ) / ( m_ngridlines[0] - 1 ) ) * dpr;
        for ( size_t i = 0; i < m_ngridlines[0]; ++i, x += dx )
        {
            kvs::OpenGL::Vertices( kvs::Vec2( x, y0 * dpr ), kvs::Vec2( x, y1 * dpr ) );
        }

        // Horizontal gridlines.
        float y = y1 * dpr;
        const float dy = ( ( y1 - y0 ) / ( m_ngridlines[1] - 1 ) ) * dpr;
        for ( size_t i = 0; i < m_ngridlines[1]; ++i, y -= dy )
        {
            kvs::OpenGL::Vertices( kvs::Vec2( x0 * dpr, y ), kvs::Vec2( x1 * dpr, y ) );
        }

        kvs::OpenGL::End();
    }
}

/*===========================================================================*/
/**
 *  @brief  Draw axis.
 *  @param  rect [in] plot region
 *  @param  axis [in] pointer to the axis
 */
/*===========================================================================*/
void Axis2D::drawAxis( const kvs::Rectangle& rect, kvs::ValueAxis* axis )
{
    if ( axis->isVisible() )
    {
        axis->setRect( rect );
        axis->draw( m_painter );
    }
}

/*===========================================================================*/
/**
 *  @brief  Draw axes.
 *  @param  rect [in] plot region
 */
/*===========================================================================*/
void Axis2D::drawAxes( const kvs::Rectangle& rect )
{
    for ( auto axis : m_axes ) { this->drawAxis( rect, axis ); }
}

/*===========================================================================*/
/**
 *  @brief  Update axes.
 *  @param  table [in] pointer to the table object
 */
/*===========================================================================*/
void Axis2D::updateAxes( const kvs::TableObject* table )
{
    const size_t naxes = m_axes.size();
    for ( size_t i = 0; i < naxes; ++i )
    {
        auto axis = m_axes[i];
        if ( axis->label().empty() ) { axis->setLabel( table->label(i) ); }
        if ( !axis->hasRange() ) { axis->setRange( table->minValue(i), table->maxValue(i) ); }
    }
}

} // end of namespace kvs
