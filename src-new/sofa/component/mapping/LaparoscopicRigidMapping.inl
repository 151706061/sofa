#ifndef SOFA_COMPONENT_MAPPING_LAPAROSCOPICRIGIDMAPPING_INL
#define SOFA_COMPONENT_MAPPING_LAPAROSCOPICRIGIDMAPPING_INL

#include <sofa/component/mapping/LaparoscopicRigidMapping.h>
#include <sofa/helper/io/Mesh.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>
#include <string>



namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;

template <class BaseMapping>
void LaparoscopicRigidMapping<BaseMapping>::init()
{
    this->BaseMapping::init();
}

template <class BaseMapping>
void LaparoscopicRigidMapping<BaseMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
    out.resize(1);
    out[0].getOrientation() = in[0].getOrientation() * rotation;
    out[0].getCenter() = pivot + in[0].getOrientation().rotate(Vector3(in[0].getTranslation(),0,0));
}

template <class BaseMapping>
void LaparoscopicRigidMapping<BaseMapping>::applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& /*in*/ )
{
    out.resize(1);
    out[0].getVOrientation() = Vector3(); //rotation * in[0].getVOrientation();
    out[0].getVCenter() = Vector3(); //in[0].getOrientation().rotate(Vec<3,Real>(in[0].getVTranslation(),0,0));
}

template <class BaseMapping>
void LaparoscopicRigidMapping<BaseMapping>::applyJT( typename In::VecDeriv& /*out*/, const typename Out::VecDeriv& /*in*/ )
{
}

template <class BaseMapping>
void LaparoscopicRigidMapping<BaseMapping>::draw()
{
    if (!getShow(this)) return;
}

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
