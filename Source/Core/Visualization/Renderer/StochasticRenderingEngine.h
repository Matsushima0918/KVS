/*****************************************************************************/
/**
 *  @file   StochasticRenderingEngine.h
 *  @author Jun Nishimura, Naohisa Sakamoto
 */
/*****************************************************************************/
#pragma once
#include <kvs/Shader>
#include <kvs/Texture2D>
#include <kvs/ValueArray>
#include <kvs/Deprecated>


namespace kvs
{

class ObjectBase;
class Camera;
class Light;

/*===========================================================================*/
/**
 *  @brief  StochasticRenderingEngine class.
 */
/*===========================================================================*/
class StochasticRenderingEngine
{
    friend class StochasticRendererBase;
    friend class StochasticRenderingCompositor;

private:
    const kvs::ObjectBase* m_object = nullptr; ///< pointer to the object
    const kvs::Shader::ShadingModel* m_shader = nullptr; ///< pointer to the shader
    bool m_enable_shading = true; ///< shading flag
    size_t m_repetition_level = 1; ///< repetition level
    size_t m_repetition_count = 0; ///< repetition count
    size_t m_random_texture_size = 512; ///< size of the random texture
    kvs::Texture2D m_random_texture{}; ///< random number texture
    kvs::Texture2D m_depth_texture{}; ///< depth texture

public:
    StochasticRenderingEngine() = default;
    virtual ~StochasticRenderingEngine()
    {
        m_random_texture.release();
        m_depth_texture.release();
    }

    size_t repetitionLevel() const { return m_repetition_level; }
    size_t repetitionCount() const { return m_repetition_count; }
    size_t randomTextureSize() const { return m_random_texture_size; }
    const kvs::Texture2D& randomTexture() const { return m_random_texture; }
    const kvs::Texture2D& depthTexture() const { return m_depth_texture; }
    const kvs::ObjectBase* object() const { return m_object; }

    void setShader( const kvs::Shader::ShadingModel* shader ) { m_shader = shader; }
    void setRepetitionLevel( const size_t repetition_level ) { m_repetition_level = repetition_level; }
    void setRandomTextureSize( const size_t size ) { m_random_texture_size = size; }
    void setDepthTexture( const kvs::Texture2D& depth_texture ) { m_depth_texture = depth_texture; }

    bool isShadingEnabled() const { return m_enable_shading; }
    void setShadingEnabled( const bool enable = true ) { m_enable_shading = enable; }

    virtual void release() = 0;
    virtual void create( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light ) = 0;
    virtual void update( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light ) = 0;
    virtual void setup( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light ) = 0;
    virtual void draw( kvs::ObjectBase* object, kvs::Camera* camera, kvs::Light* light ) = 0;

    const kvs::Shader::ShadingModel& shader() const { return *m_shader; }
    void resetRepetitions() { m_repetition_count = 0; }
    void countRepetitions() { m_repetition_count++; }
    void attachObject( const kvs::ObjectBase* object ) { m_object = object; }
    void detachObject() { m_object = nullptr; }
    void createRandomTexture();
    kvs::ValueArray<kvs::UInt16> randomIndices( const size_t nvertices ) const;

public:
    KVS_DEPRECATED( void setEnabledShading( const bool enable ) ) { this->setShadingEnabled( enable ); }
    KVS_DEPRECATED( bool isEnabledShading() const ) { return this->isShadingEnabled(); }
};

} // end of namespace kvs
