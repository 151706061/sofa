/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_LIGHT
#define SOFA_COMPONENT_LIGHT
#include "config.h"

#include <sofa/core/visual/VisualModel.h>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/visual/VisualModel.h>

#ifdef SOFA_HAVE_GLEW
#include <sofa/helper/gl/FrameBufferObject.h>
#include <SofaOpenglVisual/OglShader.h>
#endif

namespace sofa
{

namespace component
{

namespace visualmodel
{

/**
 *  \brief Utility to cast Light into a Opengl scene.
 *
 *  This class must be used in a scene with one LightManager object.
 *  This abstract class defines lights (i.e basically id and color)
 *  The inherited lights are:
 *   - Directional light (direction);
 *   - Positional light (position);
 *   - Spot light (position, direction, cutoff...).
 *
 */


class SOFA_OPENGL_VISUAL_API Light : public sofa::core::visual::VisualModel
{
public:
    SOFA_CLASS(Light, core::visual::VisualModel);
protected:
    GLint m_lightID;
    GLuint m_shadowTexWidth, m_shadowTexHeight;

#ifdef SOFA_HAVE_GLEW
    helper::gl::FrameBufferObject m_shadowFBO;
    helper::gl::FrameBufferObject m_blurHFBO;
    helper::gl::FrameBufferObject m_blurVFBO;
    static const std::string PATH_TO_GENERATE_DEPTH_TEXTURE_VERTEX_SHADER;
    static const std::string PATH_TO_GENERATE_DEPTH_TEXTURE_FRAGMENT_SHADER;
    static const std::string PATH_TO_BLUR_TEXTURE_VERTEX_SHADER;
    static const std::string PATH_TO_BLUR_TEXTURE_FRAGMENT_SHADER;
    OglShader::SPtr m_depthShader;
    OglShader::SPtr m_blurShader;
#endif
    GLfloat m_lightMatProj[16];
    GLfloat m_lightMatModelview[16];

    void computeShadowMapSize();
    void blurDepthTexture();
public:
    Data<sofa::defaulttype::Vector3> d_color;
    Data<GLuint> d_shadowTextureSize;
    Data<bool> d_drawSource;
    Data<double> d_zNear, d_zFar;
    Data<bool> d_shadowsEnabled;
    Data<bool> d_softShadows;
    Data<unsigned short> d_textureUnit;

protected:
    Light();
    virtual ~Light();
public:
    void setID(const GLint& id);

    //VisualModel
    virtual void initVisual();
    void init();
    virtual void drawLight();
    virtual void reinit();
    virtual void updateVisual();

    //CastShadowModel
    virtual void preDrawShadow(core::visual::VisualParams* vp);
    virtual void postDrawShadow();
    virtual GLuint getShadowMapSize();
    virtual GLuint getDepthTexture() { return 0 ;}
    virtual GLuint getColorTexture() { return 0 ;}
    virtual const GLfloat* getOpenGLProjectionMatrix() { return NULL ;}
    virtual const GLfloat* getOpenGLModelViewMatrix() { return NULL ;}
    virtual const sofa::defaulttype::Vector3 getPosition() { return sofa::defaulttype::Vector3(0.0,0.0,0.0); }
    virtual unsigned short getShadowTextureUnit() { return d_textureUnit.getValue(); }
    virtual void setShadowTextureUnit(const unsigned short unit) { d_textureUnit.setValue(unit); }

protected:
    bool b_needUpdate;

};

class SOFA_OPENGL_VISUAL_API DirectionalLight : public Light
{
public:
    SOFA_CLASS(DirectionalLight, Light);

    Data<sofa::defaulttype::Vector3> d_direction;

    DirectionalLight();
    virtual ~DirectionalLight();
    virtual void drawLight();
    virtual void draw(const core::visual::VisualParams* vparams);


};

class SOFA_OPENGL_VISUAL_API PositionalLight : public Light
{
public:
    SOFA_CLASS(PositionalLight, Light);

    Data<bool> d_fixed;
    Data<sofa::defaulttype::Vector3> d_position;
    Data<float> d_attenuation;

    PositionalLight();
    virtual ~PositionalLight();
    virtual void drawLight();
    virtual void draw(const core::visual::VisualParams* vparams);
    virtual const sofa::defaulttype::Vector3 getPosition() { return d_position.getValue(); }

};

class SOFA_OPENGL_VISUAL_API SpotLight : public PositionalLight
{
public:
    SOFA_CLASS(SpotLight, PositionalLight);

    Data<sofa::defaulttype::Vector3> d_direction;
    Data<float> d_cutoff;
    Data<float> d_exponent;
    Data<bool> d_lookat;
    Data<helper::vector<float> > d_modelViewMatrix;
    Data<helper::vector<float> > d_projectionMatrix;

    SpotLight();
    virtual ~SpotLight();
    virtual void drawLight();
    virtual void draw(const core::visual::VisualParams* vparams);

    void preDrawShadow(core::visual::VisualParams*  vp);
    GLuint getDepthTexture();
    GLuint getColorTexture();
    const GLfloat* getOpenGLProjectionMatrix();
    const GLfloat* getOpenGLModelViewMatrix();
private:
    void computeClippingPlane(const core::visual::VisualParams* vp, float& zNear, float& zFar);
    void computeOpenGLProjectionMatrix(GLfloat mat[16], float width, float height, float fov, float zNear, float zFar);
    void computeOpenGLModelViewMatrix(GLfloat lightMatModelview[16], const sofa::defaulttype::Vector3 &position, const sofa::defaulttype::Vector3 &direction);

};

} //namespace visualmodel

} //namespace component

} //namespace sofa

#endif //SOFA_COMPONENT_LIGHT
