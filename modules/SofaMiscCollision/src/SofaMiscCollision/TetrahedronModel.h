/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_COLLISION_TETRAHEDRONCOLLISIONMODEL_H
#define SOFA_COMPONENT_COLLISION_TETRAHEDRONCOLLISIONMODEL_H
#include <SofaMiscCollision/config.h>

#include <SofaMeshCollision/BarycentricContactMapper.h>
#include <sofa/core/CollisionModel.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/VecTypes.h>

#include <map>

namespace sofa::component::collision
{

class TetrahedronCollisionModel;

class Tetrahedron : public core::TCollisionElementIterator<TetrahedronCollisionModel>
{
public:
    Tetrahedron(TetrahedronCollisionModel* model, Index index);
    Tetrahedron() {};
    explicit Tetrahedron(const core::CollisionElementIterator& i);

    const type::Vector3& p1() const;
    const type::Vector3& p2() const;
    const type::Vector3& p3() const;
    const type::Vector3& p4() const;
    int p1Index() const;
    int p2Index() const;
    int p3Index() const;
    int p4Index() const;

    const type::Vector3& p1Free() const;
    const type::Vector3& p2Free() const;
    const type::Vector3& p3Free() const;
    const type::Vector3& p4Free() const;

    const type::Vector3& v1() const;
    const type::Vector3& v2() const;
    const type::Vector3& v3() const;
    const type::Vector3& v4() const;

    type::Vector3 getBary(const type::Vector3& p) const;
    type::Vector3 getDBary(const type::Vector3& v) const;

    type::Vector3 getCoord(const type::Vector3& b) const;
    type::Vector3 getDCoord(const type::Vector3& b) const;

};

class SOFA_MISC_COLLISION_API TetrahedronCollisionModel : public core::CollisionModel
{
public:
    SOFA_CLASS(TetrahedronCollisionModel, core::CollisionModel);

    typedef defaulttype::Vec3Types InDataTypes;
    typedef defaulttype::Vec3Types DataTypes;
    typedef DataTypes::VecCoord VecCoord;
    typedef DataTypes::VecDeriv VecDeriv;
    typedef DataTypes::Coord Coord;
    typedef DataTypes::Deriv Deriv;
    typedef Tetrahedron Element;
    friend class Tetrahedron;

protected:
    struct TetrahedronInfo
    {
        type::Vector3 coord0;
        type::Matrix3 coord2bary;
        type::Matrix3 bary2coord;
    };

    sofa::type::vector<TetrahedronInfo> elems;
    const sofa::core::topology::BaseMeshTopology::SeqTetrahedra* tetra;

    core::behavior::MechanicalState<defaulttype::Vec3Types>* mstate;

    sofa::core::topology::BaseMeshTopology* m_topology;

    int m_topologyRevision; ///< internal revision number to check if topology has changed.

protected:

    TetrahedronCollisionModel();

    virtual void updateFromTopology();
    void addTetraToDraw(const Tetrahedron& t, std::vector<sofa::type::Vector3>& tetraVertices, std::vector<sofa::type::Vector3>& normalVertices);
public:
    void init() override;

    // -- CollisionModel interface

    void resize(Size size) override;

    void computeBoundingTree(int maxDepth=0) override;

    //virtual void computeContinuousBoundingTree(double dt, int maxDepth=0);

    void draw(const core::visual::VisualParams*, Index index) override;

    void draw(const core::visual::VisualParams* vparams) override;

    core::behavior::MechanicalState<defaulttype::Vec3Types>* getMechanicalState() { return mstate; }

    /// Link to be set to the topology container in the component graph.
    SingleLink<TetrahedronCollisionModel, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

};

inline Tetrahedron::Tetrahedron(TetrahedronCollisionModel* model, Index index)
    : core::TCollisionElementIterator<TetrahedronCollisionModel>(model, index)
{}

inline Tetrahedron::Tetrahedron(const core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<TetrahedronCollisionModel>(static_cast<TetrahedronCollisionModel*>(i.getCollisionModel()), i.getIndex())
{}

inline const type::Vector3& Tetrahedron::p1() const { return model->mstate->read(core::ConstVecCoordId::position())->getValue()[(*(model->tetra))[index][0]]; }
inline const type::Vector3& Tetrahedron::p2() const { return model->mstate->read(core::ConstVecCoordId::position())->getValue()[(*(model->tetra))[index][1]]; }
inline const type::Vector3& Tetrahedron::p3() const { return model->mstate->read(core::ConstVecCoordId::position())->getValue()[(*(model->tetra))[index][2]]; }
inline const type::Vector3& Tetrahedron::p4() const { return model->mstate->read(core::ConstVecCoordId::position())->getValue()[(*(model->tetra))[index][3]]; }

inline const type::Vector3& Tetrahedron::p1Free() const { return model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[(*(model->tetra))[index][0]]; }
inline const type::Vector3& Tetrahedron::p2Free() const { return model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[(*(model->tetra))[index][1]]; }
inline const type::Vector3& Tetrahedron::p3Free() const { return model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[(*(model->tetra))[index][2]]; }
inline const type::Vector3& Tetrahedron::p4Free() const { return model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[(*(model->tetra))[index][3]]; }

inline int Tetrahedron::p1Index() const { return (*(model->tetra))[index][0]; }
inline int Tetrahedron::p2Index() const { return (*(model->tetra))[index][1]; }
inline int Tetrahedron::p3Index() const { return (*(model->tetra))[index][2]; }
inline int Tetrahedron::p4Index() const { return (*(model->tetra))[index][3]; }

inline const type::Vector3& Tetrahedron::v1() const { return model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[(*(model->tetra))[index][0]]; }
inline const type::Vector3& Tetrahedron::v2() const { return model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[(*(model->tetra))[index][1]]; }
inline const type::Vector3& Tetrahedron::v3() const { return model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[(*(model->tetra))[index][2]]; }
inline const type::Vector3& Tetrahedron::v4() const { return model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[(*(model->tetra))[index][3]]; }

inline type::Vector3 Tetrahedron::getBary(const type::Vector3& p) const { return model->elems[index].coord2bary*(p-model->elems[index].coord0); }
inline type::Vector3 Tetrahedron::getDBary(const type::Vector3& v) const { return model->elems[index].coord2bary*(v); }
inline type::Vector3 Tetrahedron::getCoord(const type::Vector3& b) const { return model->elems[index].bary2coord*b + model->elems[index].coord0; }
inline type::Vector3 Tetrahedron::getDCoord(const type::Vector3& b) const { return model->elems[index].bary2coord*b; }

/// Mapper for TetrahedronCollisionModel
template<class DataTypes>
class ContactMapper<TetrahedronCollisionModel, DataTypes> : public BarycentricContactMapper<TetrahedronCollisionModel, DataTypes>
{
public:
    using Index = sofa::Index;
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    Index addPoint(const Coord& P, Index index, Real&)
    {
        Tetrahedron t(this->model, index);
        type::Vector3 b = t.getBary(P);
        return this->mapper->addPointInTetra(index, b.ptr());
    }
};

#if  !defined(SOFA_COMPONENT_COLLISION_TETRAHEDRONCOLLISIONMODEL_CPP)
extern template class SOFA_MISC_COLLISION_API ContactMapper<TetrahedronCollisionModel, sofa::defaulttype::Vec3Types>;

#  ifdef _MSC_VER
// Manual declaration of non-specialized members, to avoid warnings from MSVC.
extern template SOFA_MISC_COLLISION_API void BarycentricContactMapper<TetrahedronCollisionModel, defaulttype::Vec3Types>::cleanup();
extern template SOFA_MISC_COLLISION_API core::behavior::MechanicalState<defaulttype::Vec3Types>* BarycentricContactMapper<TetrahedronCollisionModel, defaulttype::Vec3Types>::createMapping(const char*);
#  endif
#endif

} // namespace sofa::component::collision

#endif