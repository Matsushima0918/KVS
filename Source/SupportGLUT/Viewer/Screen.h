/****************************************************************************/
/**
 *  @file   Screen.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include "ScreenBase.h"
#include <kvs/Scene>
#include <kvs/RGBColor>
#include <kvs/ColorImage>
#include <kvs/InteractorBase>


namespace kvs
{

class ObjectBase;
class RendererBase;
class VisualizationPipeline;

namespace glut
{

class Application;

/*===========================================================================*/
/**
 *  @brief  GLUT screen class.
 */
/*===========================================================================*/
class Screen : public kvs::glut::ScreenBase
{
public:
    typedef kvs::glut::ScreenBase BaseClass;
    typedef kvs::Scene::ControlTarget ControlTarget;

    static Screen* DownCast( kvs::ScreenBase* screen );
    static const Screen* DownCast( const kvs::ScreenBase* screen );

private:
    kvs::Scene* m_scene; ///< scene
    kvs::InteractorBase* m_interactor; ///< interactor

public:
    Screen( kvs::glut::Application* application = 0 );
    virtual ~Screen();

    kvs::Scene* scene();
    const kvs::Scene* scene() const { return m_scene; }

    void setPosition( const int x, const int y );
    void setSize( const int width, const int height );
    void setGeometry( const int x, const int y, const int width, const int height );
    void setBackgroundColor( const kvs::RGBColor& color );
    void setBackgroundColor( const kvs::RGBColor& color1, const kvs::RGBColor& color2 );
    void setBackgroundImage( const kvs::ColorImage& image );
    void setControlTarget( const ControlTarget target );
    void setControlTargetToObject();
    void setControlTargetToCamera();
    void setControlTargetToLight();

    const std::pair<int,int> registerObject( kvs::ObjectBase* object, kvs::RendererBase* renderer = 0 );
    const std::pair<int,int> registerObject( kvs::VisualizationPipeline* pipeline );

    virtual void setEvent( kvs::EventListener* event, const std::string& name = "" );
    virtual void addEvent( kvs::EventListener* event, const std::string& name = "" );

    virtual void enable();
    virtual void disable();
    virtual void reset();
    virtual kvs::ColorImage capture() const;

    virtual void initializeEvent();
    virtual void paintEvent();
    virtual void resizeEvent( int width, int height );
    virtual void mousePressEvent( kvs::MouseEvent* event );
    virtual void mouseMoveEvent( kvs::MouseEvent* event );
    virtual void mouseReleaseEvent( kvs::MouseEvent* event );
    virtual void mouseDoubleClickEvent( kvs::MouseEvent* event );
    virtual void wheelEvent( kvs::WheelEvent* event );
    virtual void keyPressEvent( kvs::KeyEvent* event );
    virtual void keyRepeatEvent( kvs::KeyEvent* event );
    virtual void keyReleaseEvent( kvs::KeyEvent* event );
};

} // end of namespace glut

} // end of namespace kvs
