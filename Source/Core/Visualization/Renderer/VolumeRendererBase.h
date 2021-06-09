/****************************************************************************/
/**
 *  @file   VolumeRendererBase.h
 *  @author Naohisa Sakamoto
 */
/****************************************************************************/
#pragma once
#include <kvs/DebugNew>
#include <kvs/RendererBase>
#include <kvs/TransferFunction>
#include <kvs/FrameBuffer>
#include <kvs/ValueArray>
#include <kvs/Shader>


namespace kvs
{

/*==========================================================================*/
/**
 *  Volume renderer base class.
 */
/*==========================================================================*/
class VolumeRendererBase : public kvs::RendererBase
{
    kvsModule( kvs::VolumeRendererBase, Renderer );

private:
    size_t m_window_width; ///< window width
    size_t m_window_height; ///< window height
    float m_device_pixel_ratio; ///< device pixel ratio
    kvs::ValueArray<kvs::Real32> m_depth_data; ///< depth data as float type
    kvs::ValueArray<kvs::UInt8> m_color_data; ///< color (RGBA) data as uchar type
    kvs::FrameBuffer m_depth_buffer; ///< depth buffer
    kvs::FrameBuffer m_color_buffer; ///< color (RGBA) buffer
    kvs::TransferFunction m_tfunc; ///< transfer function
    kvs::Shader::ShadingModel* m_shader; ///< shading method
    const kvs::ObjectBase* m_object; ///< rendering object

public:
    VolumeRendererBase();
    virtual ~VolumeRendererBase();

    virtual void exec(
        kvs::ObjectBase* object,
        kvs::Camera* camera = NULL,
        kvs::Light* light  = NULL ) = 0;

    size_t windowWidth() const { return m_window_width; }
    size_t windowHeight() const { return m_window_height; }
    size_t framebufferWidth() const { return static_cast<size_t>( m_window_width * m_device_pixel_ratio ); }
    size_t framebufferHeight() const { return static_cast<size_t>( m_window_height * m_device_pixel_ratio ); }
    float devicePixelRatio() const { return m_device_pixel_ratio; }
    template <typename ShadingType>
    void setShader( const ShadingType shader );
    void setTransferFunction( const kvs::TransferFunction& tfunc ) { m_tfunc = tfunc; }
    const kvs::TransferFunction& transferFunction() const { return m_tfunc; }

    void setTwoSideLightingEnabled( const bool enable = true ) { m_shader->two_side_lighting = enable; }
    bool isTwoSideLightingEnabled() const { return m_shader->two_side_lighting; }
    void enableTwoSideLighting() { this->setTwoSideLightingEnabled( true ); }
    void disableTwoSideLighting() { this->setTwoSideLightingEnabled( false ); }

protected:
    kvs::ValueArray<kvs::UInt8>& colorData() { return m_color_data; }
    kvs::ValueArray<kvs::Real32>& depthData() { return m_depth_data; }
    kvs::Shader::ShadingModel& shader() { return *m_shader; }
    kvs::TransferFunction& transferFunction() { return m_tfunc; }
    const kvs::ObjectBase* object() const { return m_object; }
    void setWindowSize( const size_t width, const size_t height ) { m_window_width = width; m_window_height = height; }
    void setDevicePixelRatio( const float dpr ) { m_device_pixel_ratio = dpr; }
    void setObject( const kvs::ObjectBase* object ) { m_object = object; }
    bool isWindowCreated() { return m_window_width == 0 && m_window_height == 0; }
    bool isWindowResized( size_t w, size_t h ) { return m_window_width != w || m_window_height != h; }
    bool isObjectChanged( const kvs::ObjectBase* o ) { return m_object != o; }
    void allocateDepthData( const size_t size );
    void allocateColorData( const size_t size );
    void fillDepthData( const kvs::Real32 value );
    void fillColorData( const kvs::UInt8 value );
    void readImage();
    void drawImage();
};

template <typename ShadingType>
inline void VolumeRendererBase::setShader( const ShadingType shader )
{
    if ( m_shader )
    {
        delete m_shader;
        m_shader = NULL;
    }

    m_shader = new ShadingType( shader );
    if ( !m_shader )
    {
        kvsMessageError("Cannot create a specified shader.");
    }
};

} // end of namespace kvs
