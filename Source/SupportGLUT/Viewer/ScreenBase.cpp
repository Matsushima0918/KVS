/*****************************************************************************/
/**
 *  @file   ScreenBase.cpp
 *  @author Naohisa Sakamoto
 */
/*****************************************************************************/
#include "ScreenBase.h"
#include <kvs/Message>
#include <kvs/Assert>
#include <kvs/MouseEvent>
#include <kvs/KeyEvent>
#include <kvs/WheelEvent>
#include <kvs/EventListener>
#include <kvs/glut/GLUT>
#include <kvs/glut/Application>
#include <SupportGLUT/Viewer/KVSMouseButton.h>
#include <SupportGLUT/Viewer/KVSKey.h>
#include <cstdlib>
#include <kvs/EventTimer>
#include <map>


namespace
{

const size_t MaxNumberOfScreens = 256;
kvs::glut::ScreenBase* Context[ MaxNumberOfScreens ] = {};

/*===========================================================================*/
/**
 *  @brief  Function that is called when the application is terminated.
 */
/*===========================================================================*/
void ExitFunction()
{
    for ( size_t i = 0; i < MaxNumberOfScreens; i++)
    {
        if ( Context[i] ) Context[i]->~ScreenBase();
    }
}

class EventTimer : public kvs::EventTimer
{
private:
    static int m_counter;
    static std::map<int,EventTimer*> m_timers;
    friend void TimerFunc( int id );

public:
    EventTimer( kvs::EventListener* listener );
    virtual ~EventTimer();
    void start( int msec );
    void nortify();
};

int EventTimer::m_counter = 0;
std::map<int,EventTimer*> EventTimer::m_timers;

void TimerFunc( int id )
{
    auto* t = EventTimer::m_timers.find( id )->second;
    if ( t )
    {
        t->nortify();
        glutTimerFunc( t->interval(), TimerFunc, t->id() );
    }
}

EventTimer::EventTimer( kvs::EventListener* listener ):
    kvs::EventTimer( listener )
{
    this->setID( EventTimer::m_counter++ );
    EventTimer::m_timers.insert( std::make_pair( this->id(), this ) );
}

EventTimer::~EventTimer()
{
    EventTimer::m_timers.erase( this->id() );
}

void EventTimer::start( int msec )
{
    if ( msec < 0 ) { return; }
    if ( this->isStopped() )
    {
        this->setInterval( msec );
        this->setStopped( false );
        glutTimerFunc( this->interval(), TimerFunc, this->id() );
    }
}

void EventTimer::nortify()
{
    if ( !this->isStopped() )
    {
        if ( this->eventListener() )
        {
            this->eventListener()->onEvent( this->timeEvent() );
        }
    }
}

}


namespace kvs
{

namespace glut
{

/*===========================================================================*/
/**
 *  @brief  Returns the pointer to the glut screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return pointer to the glut screen base
 */
/*===========================================================================*/
ScreenBase* ScreenBase::DownCast( kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( screen );
}

/*===========================================================================*/
/**
 *  @brief  Returns the const pointer to the glut screen base downcasted from the screen base.
 *  @param  screen [in] the screen base.
 *  @return const pointer to the glut screen base
 */
/*===========================================================================*/
const ScreenBase* ScreenBase::DownCast( const kvs::ScreenBase* screen )
{
    return dynamic_cast<ScreenBase*>( const_cast<kvs::ScreenBase*>( screen ) );
}

/*===========================================================================*/
/**
 *  @brief  Display function for glutDisplayFunc.
 */
/*===========================================================================*/
void DisplayFunction()
{
    const int id = glutGetWindow();
    ::Context[id]->paintEvent();
}

/*===========================================================================*/
/**
 *  @brief  Resize function for glutReshapeFunc.
 *  @param  width [in] window width
 *  @param  height [in] window height
 */
/*===========================================================================*/
void ResizeFunction( int width, int height )
{
    const int id = glutGetWindow();
    ::Context[id]->resizeEvent( width, height );
}

/*===========================================================================*/
/**
 *  @brief  Mouse function for glutMouseFunc.
 *  @param  button [in] button ID
 *  @param  state [in] state ID
 *  @param  x [in] x coordinate of the mouse on the window coordinate
 *  @param  y [in] y coordinate of the mouse on the window coordinate
 */
/*===========================================================================*/
void MouseFunction( int button, int state, int x, int y )
{
    const int id = glutGetWindow();
    const int modifier = kvs::glut::KVSKey::Modifier( glutGetModifiers() );
    button = kvs::glut::KVSMouseButton::Button( button );
    state = kvs::glut::KVSMouseButton::State( state );
    ::Context[id]->m_mouse_event->setButton( button );
    ::Context[id]->m_mouse_event->setState( state );
    ::Context[id]->m_mouse_event->setPosition( x, y );
    ::Context[id]->m_mouse_event->setModifiers( modifier );

    switch ( state )
    {
    case kvs::MouseButton::Down:
        ::Context[id]->m_elapse_time_counter.stop();
        if ( ::Context[id]->m_elapse_time_counter.sec() < 0.2f )
        {
            ::Context[id]->m_mouse_event->setAction( kvs::MouseButton::DoubleClicked );
            ::Context[id]->mouseDoubleClickEvent( ::Context[id]->m_mouse_event );
        }
        else
        {
            ::Context[id]->m_mouse_event->setAction( kvs::MouseButton::Pressed );
            ::Context[id]->mousePressEvent( ::Context[id]->m_mouse_event );
        }
        ::Context[id]->m_elapse_time_counter.start();
        break;
    case kvs::MouseButton::Up:
        ::Context[id]->m_mouse_event->setAction( kvs::MouseButton::Released );
        ::Context[id]->mouseReleaseEvent( ::Context[id]->m_mouse_event );
        break;
    default: break;
    }

    ::Context[id]->m_wheel_event->setPosition( x, y );
    switch( button )
    {
    case kvs::MouseButton::WheelUp:
        ::Context[id]->m_wheel_event->setDirection( 1 );
        ::Context[id]->wheelEvent( ::Context[id]->m_wheel_event );
        break;
    case kvs::MouseButton::WheelDown:
        ::Context[id]->m_wheel_event->setDirection( -1 );
        ::Context[id]->wheelEvent( ::Context[id]->m_wheel_event );
        break;
    default: break;
    }
}

/*===========================================================================*/
/**
 *  @brief  Mouse move function for glutMotionFunc.
 *  @param  x [in] x coordinate value of the mouse cursor on the window coordinate
 *  @param  y [in] y coordinate value of the mouse cursor on the window coordinate
 */
/*===========================================================================*/
void MouseMoveFunction( int x, int y )
{
    const int id = glutGetWindow();
    ::Context[id]->m_mouse_event->setPosition( x, y );
    ::Context[id]->m_mouse_event->setAction( kvs::MouseButton::Moved );
    ::Context[id]->mouseMoveEvent( ::Context[id]->m_mouse_event );
}

/*===========================================================================*/
/**
 *  @brief  Key press function for glutKeyboardFunc.
 *  @param  key [in] key code
 *  @param  x [in] x coordinate value of the mouse cursor on the window coordinate
 *  @param  y [in] y coordinate value of the mouse cursor on the window coordinate
 */
/*===========================================================================*/
void KeyPressFunction( unsigned char key, int x, int y )
{
    const int id = glutGetWindow();
    const int code = kvs::glut::KVSKey::ASCIICode( key );
    const int mods = kvs::glut::KVSKey::Modifier( glutGetModifiers() );
    ::Context[id]->m_key_event->setKey( code );
    ::Context[id]->m_key_event->setPosition( x, y );
    ::Context[id]->m_key_event->setModifiers( mods );
    switch ( ::Context[id]->m_key_event->action() )
    {
    case kvs::Key::NoAction:
    case kvs::Key::Released:
        ::Context[id]->m_key_event->setAction( kvs::Key::Pressed );
        ::Context[id]->keyPressEvent( ::Context[id]->m_key_event );
        break;
    case kvs::Key::Pressed:
    case kvs::Key::Repeated:
        ::Context[id]->m_key_event->setAction( kvs::Key::Repeated );
        ::Context[id]->keyRepeatEvent( ::Context[id]->m_key_event );
        break;
    default:
        break;
    }
}

/*===========================================================================*/
/**
 *  @brief  Special key press function for glutSpecialFunc.
 *  @param  key [in] key code
 *  @param  x [in] x coordinate value of the mouse cursor on the window coordinate
 *  @param  y [in] y coordinate value of the mouse cursor on the window coordinate
 */
/*===========================================================================*/
void SpecialKeyPressFunction( int key, int x, int y )
{
    const int id = glutGetWindow();
    const int code = kvs::glut::KVSKey::SpecialCode( key );
    ::Context[id]->m_key_event->setKey( code );
    ::Context[id]->m_key_event->setPosition( x, y );
    switch ( ::Context[id]->m_key_event->action() )
    {
    case kvs::Key::NoAction:
    case kvs::Key::Released:
        ::Context[id]->m_key_event->setAction( kvs::Key::Pressed );
        ::Context[id]->keyPressEvent( ::Context[id]->m_key_event );
        break;
    case kvs::Key::Pressed:
    case kvs::Key::Repeated:
        ::Context[id]->m_key_event->setAction( kvs::Key::Repeated );
        ::Context[id]->keyRepeatEvent( ::Context[id]->m_key_event );
        break;
    default:
        break;
    }
}

/*===========================================================================*/
/**
 *  @brief  Key release function for glutKeyboardUpFunc.
 *  @param  key [in] key code
 *  @param  x [in] x coordinate value of the mouse cursor on the window coordinate
 *  @param  y [in] y coordinate value of the mouse cursor on the window coordinate
 */
/*===========================================================================*/
void KeyReleaseFunction( unsigned char key, int x, int y )
{
    const int id = glutGetWindow();
    const int code = kvs::glut::KVSKey::ASCIICode( key );
    const int mods = kvs::glut::KVSKey::Modifier( glutGetModifiers() );
    ::Context[id]->m_key_event->setKey( code );
    ::Context[id]->m_key_event->setPosition( x, y );
    ::Context[id]->m_key_event->setModifiers( mods );
    ::Context[id]->m_key_event->setAction( kvs::Key::Released );
    ::Context[id]->keyReleaseEvent( ::Context[id]->m_key_event );
}

/*===========================================================================*/
/**
 *  @brief  Key release function for glutSpecialUpFunc.
 *  @param  key [in] key code
 *  @param  x [in] x coordinate value of the mouse cursor on the window coordinate
 *  @param  y [in] y coordinate value of the mouse cursor on the window coordinate
 */
/*===========================================================================*/
void SpecialKeyReleaseFunction( int key, int x, int y )
{
    const int id = glutGetWindow();
    const int code = kvs::glut::KVSKey::SpecialCode( key );
    ::Context[id]->m_key_event->setKey( code );
    ::Context[id]->m_key_event->setPosition( x, y );
    ::Context[id]->m_key_event->setAction( kvs::Key::Released );
    ::Context[id]->keyReleaseEvent( ::Context[id]->m_key_event );
}

/*===========================================================================*/
/**
 *  @brief  Constructs a new ScreenBase class.
 *  @param  application [in] pointer to the application
 */
/*===========================================================================*/
ScreenBase::ScreenBase( kvs::glut::Application* application ):
    m_id( -1 ),
    m_mouse_event( 0 ),
    m_key_event( 0 ),
    m_wheel_event( 0 )
//    m_is_fullscreen( false )
{
    if ( application ) application->attach( this );

    m_mouse_event = new kvs::MouseEvent();
    m_key_event = new kvs::KeyEvent();
    m_wheel_event = new kvs::WheelEvent();

    m_elapse_time_counter.start();
}

/*===========================================================================*/
/**
 *  @brief  Destructs the ScreenBase class.
 */
/*===========================================================================*/
ScreenBase::~ScreenBase()
{
    delete m_mouse_event;
    delete m_key_event;
    delete m_wheel_event;

    ::Context[ m_id ] = NULL;
    glutDestroyWindow( m_id );
}

/*===========================================================================*/
/**
 *  @brief  Sets an event.
 *  @param  event [in] event
 *  @param  name [in] name of the event
 */
/*===========================================================================*/
void ScreenBase::setEvent( kvs::EventListener* event, const std::string& name )
{
    if ( event->eventType() & kvs::EventBase::TimerEvent )
    {
        event->setEventTimer( new ::EventTimer( event ) );
    }
    BaseClass::setEvent( event, name );
}

/*===========================================================================*/
/**
 *  @brief  Adds an event.
 *  @param  event [in] event
 *  @param  name [in] name of the event
 */
/*===========================================================================*/
void ScreenBase::addEvent( kvs::EventListener* event, const std::string& name )
{
    if ( event->eventType() & kvs::EventBase::TimerEvent )
    {
        event->setEventTimer( new ::EventTimer( event ) );
    }
    BaseClass::addEvent( event, name );
}

/*===========================================================================*/
/**
 *  @brief  Creates the screen.
 */
/*===========================================================================*/
void ScreenBase::create()
{
    KVS_ASSERT( m_id == -1 );

    // Initialize display mode.
    unsigned int mode = 0;
    if ( displayFormat().doubleBuffer() ) mode |= GLUT_DOUBLE; else mode |= GLUT_SINGLE;
    if ( displayFormat().colorBuffer() ) mode |= GLUT_RGBA; else mode |= GLUT_INDEX;
    if ( displayFormat().depthBuffer() ) mode |= GLUT_DEPTH;
    if ( displayFormat().accumulationBuffer() ) mode |= GLUT_ACCUM;
    if ( displayFormat().stencilBuffer() ) mode |= GLUT_STENCIL;
    if ( displayFormat().stereoBuffer() ) mode |= GLUT_STEREO;
    if ( displayFormat().multisampleBuffer() ) mode |= GLUT_MULTISAMPLE;
    if ( displayFormat().alphaChannel() ) mode |= GLUT_ALPHA;
    glutInitDisplayMode( mode );

    // Set screen geometry.
    if ( BaseClass::x() < 0 && BaseClass::y() < 0 )
    {
        // Centering
        const int desk_width = glutGet( GLUT_SCREEN_WIDTH );
        const int desk_height = glutGet( GLUT_SCREEN_HEIGHT );
        const int px = ( desk_width - BaseClass::width() ) / 2;
        const int py = ( desk_height - BaseClass::height() ) / 2;
        const int offset = 20;
        static int counter = 0;
        this->setPosition( px + offset * counter, py + offset * counter );
        counter++;
    }
    glutInitWindowPosition( BaseClass::x(), BaseClass::y() );
    glutInitWindowSize( BaseClass::width(), BaseClass::height() );

    // Create window.
    glutCreateWindow( BaseClass::title().c_str() );

    // Set to the global context.
    m_id = glutGetWindow();
    ::Context[ m_id ] = this;

    // Initialize GLEW.
#if defined( KVS_ENABLE_GLEW )
    GLenum result = glewInit();
    if ( result != GLEW_OK )
    {
        const GLubyte* message = glewGetErrorString( result );
        kvsMessageError( "GLEW initialization failed: %s.", message );
    }
#endif

    // Create paint device.
    BaseClass::paintDevice()->create();

    // Set device pixel ratio.
    const kvs::Vec4 vp = kvs::OpenGL::Viewport();
    BaseClass::setDevicePixelRatio( vp[2] / BaseClass::width() );

    // Register the exit function.
    static bool flag = true;
    if ( flag ) { atexit( ::ExitFunction ); flag = false; }

    // Register callback functions.
    glutMouseFunc( MouseFunction );
    glutMotionFunc( MouseMoveFunction );
    glutKeyboardFunc( KeyPressFunction );
    glutSpecialFunc( SpecialKeyPressFunction );
    glutKeyboardUpFunc( KeyReleaseFunction );
    glutSpecialUpFunc( SpecialKeyReleaseFunction );
    glutDisplayFunc( DisplayFunction );
    glutReshapeFunc( ResizeFunction );
}

/*===========================================================================*/
/**
 *  @brief  Shows the screen.
 *  @return window ID
 */
/*===========================================================================*/
void ScreenBase::show()
{
    BaseClass::show();

#if 1 // KVS_ENABLE_DEPRECATED
    if ( m_id == -1 ) this->create();
    else {
#endif

    glutSetWindow( m_id );
    glutShowWindow();

#if 1 // KVS_ENABLE_DEPRECATED
    }
#endif
}

/*===========================================================================*/
/**
 *  @brief  Shows the window as full-screen.
 */
/*===========================================================================*/
void ScreenBase::showFullScreen()
{
    if ( BaseClass::isFullScreen() ) { return; }
    //if ( m_is_fullscreen ) return;

    BaseClass::showFullScreen();
    //m_is_fullscreen = true;

    const int x = glutGet( (GLenum)GLUT_WINDOW_X );
    const int y = glutGet( (GLenum)GLUT_WINDOW_Y );
    BaseClass::setPosition( x, y );

    glutFullScreen();
}

/*===========================================================================*/
/**
 *  @brief  Shows the window as normal screen.
 */
/*===========================================================================*/
void ScreenBase::showNormal()
{
    if ( !BaseClass::isFullScreen() ) { return; }
    //if ( !m_is_fullscreen ) return;

    BaseClass::showNormal();
    //m_is_fullscreen = false;

    glutReshapeWindow( BaseClass::width(), BaseClass::height() );
    glutPositionWindow( BaseClass::x(), BaseClass::y() );
}

/*===========================================================================*/
/**
 *  @brief  Hides the window.
 */
/*===========================================================================*/
void ScreenBase::hide()
{
    BaseClass::hide();

    glutSetWindow( m_id );
    glutHideWindow();
}

/*===========================================================================*/
/**
 *  @brief  Pops up the window.
 */
/*===========================================================================*/
void ScreenBase::popUp()
{
    glutPopWindow();
}

/*===========================================================================*/
/**
 *  @brief  Pushes down the window.
 */
/*===========================================================================*/
void ScreenBase::pushDown()
{
    glutPushWindow();
}

/*===========================================================================*/
/**
 *  @brief  Requires that the screen needs to be redrawn as soon as possible.
 */
/*===========================================================================*/
void ScreenBase::redraw()
{
    const int id = glutGetWindow();
    glutSetWindow( m_id );
    glutPostRedisplay();
    glutSetWindow( id );
}

/*===========================================================================*/
/**
 *  @brief  Resizes the window.
 *  @param  width [in] resized window width
 *  @param  height [in] resized window height
 */
/*===========================================================================*/
void ScreenBase::resize( int width, int height )
{
    BaseClass::setSize( width, height );
    glutReshapeWindow( BaseClass::width(), BaseClass::height() );
}

/*===========================================================================*/
/**
 *  @brief  Triggers redrawing the screen directory.
 */
/*===========================================================================*/
void ScreenBase::draw()
{
    if ( m_id == -1 ) { this->create(); }
    this->paintEvent();
}

/*===========================================================================*/
/**
 *  @brief  Checks whether the window is full-screen or not.
 *  @return true, if the window is full-screen
 */
/*===========================================================================*/
//bool ScreenBase::isFullScreen() const
//{
//    return m_is_fullscreen;
//}

/*===========================================================================*/
/**
 *  @brief  Returns the captured image of the current screen.
 *  @return captured image
 */
/*===========================================================================*/
kvs::ColorImage ScreenBase::capture() const
{
    const size_t width = BaseClass::width();
    const size_t height = BaseClass::height();
    kvs::ValueArray<kvs::UInt8> buffer( width * height * 3 );

    kvs::OpenGL::SetReadBuffer( GL_FRONT );
    kvs::OpenGL::ReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data() );
    return kvs::ColorImage( width, height, buffer );
}

} // end of namespace glut

} // end of namespace kvs
