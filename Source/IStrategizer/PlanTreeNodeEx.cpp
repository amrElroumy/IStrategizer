#ifndef PLANTREENODEEX_H
#include "PlanTreeNodeEx.h"
#endif
#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif
#ifndef GOALEX_H
#include "GoalEx.h"
#endif
#ifndef ACTION_H
#include "Action.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#include "Logger.h"
#include <algorithm>
#include <cassert>
#include <Windows.h>

using namespace IStrategizer;

//////////////////////////////////////////////////////////////////////////
void* PlanTreeNodeEx::operator new (size_t p_size)
{
    void* pNode = HeapAlloc(GetProcessHeap(), 0, p_size);
    return pNode;
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::operator delete (void *p_pNode)
{
    HeapFree(GetProcessHeap(), 0, p_pNode);
}
//////////////////////////////////////////////////////////////////////////
PlanTreeNodeEx::PlanTreeNodeEx(PlanStepEx* p_pPlanStep, PlanTreeNodeEx* p_pSubPlanGoal) :
_pPlanStep(p_pPlanStep), _pSubPlanGoal(p_pSubPlanGoal), _isOpen(true)
{
    if (p_pPlanStep)
    {
        if(BELONG(GoalType, p_pPlanStep->StepTypeId()))
        {
            _type = PTNTYPE_Goal;
        }
        else if(BELONG(ActionType, p_pPlanStep->StepTypeId()))
        {
            _type = PTNTYPE_Action;
        }
    }
    else
    {
        _type = PTNTYPE_nullptr;
    }
}
//////////////////////////////////////////////////////////////////////////
PlanTreeNodeEx& PlanTreeNodeEx::Null()
{
    static PlanTreeNodeEx sentinelNode(nullptr, nullptr);
    return sentinelNode;
}
//////////////////////////////////////////////////////////////////////////
PlanTreeNodeEx* PlanTreeNodeEx::CreatePlanRoot(PlanStepEx *p_pPlanStep)
{
    PlanTreeNodeEx *pPlanRoot = nullptr;

    // Initialize plan root
    pPlanRoot = new PlanTreeNodeEx(p_pPlanStep, &PlanTreeNodeEx::Null());
    pPlanRoot->SetChildrenAsBelongingSubPlanChildren();
    pPlanRoot->AddParent(&PlanTreeNodeEx::Null());
    pPlanRoot->NotifyParentSuccess(&PlanTreeNodeEx::Null());

    return pPlanRoot;
}
//////////////////////////////////////////////////////////////////////////
GoalEx* PlanTreeNodeEx::GetGoal()
{
    assert(BELONG(GoalType, _pPlanStep->StepTypeId()));
    return static_cast<GoalEx*> (_pPlanStep);
}
//////////////////////////////////////////////////////////////////////////
Action* PlanTreeNodeEx::GetAction()
{
    assert(BELONG(ActionType, _pPlanStep->StepTypeId()));
    return static_cast<Action*> (_pPlanStep);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::NotifyParentSuccess(PlanTreeNodeEx *p_pSuccededParent)
{
    _readyParents.insert(p_pSuccededParent);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::Open() 
{
    assert(_isOpen == false);
    _isOpen = true;
    LogInfo("'%s' is opened", _pPlanStep->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::Close()
{
    assert(_isOpen == true);
    _isOpen = false; 
    LogInfo("'%s' is closed", _pPlanStep->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::AddChild(PlanTreeNodeEx *p_pChild)
{
    _children.push_back(p_pChild);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::AddParent(PlanTreeNodeEx *p_pParent)
{
    _parents.push_back(p_pParent);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::CrossUnlinkChildren()
{
    List tempChildren(_children);

    for (List::iterator itr = tempChildren.begin();
        itr != tempChildren.end(); ++itr)
    {
        CrossUnlinkChild(*itr);
    }

    _children.clear();
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::DeleteChild(PlanTreeNodeEx* p_pChild)
{
    List::iterator itrWhere = find(_children.begin(), _children.end(), p_pChild);
    assert(itrWhere != _children.end());

    _children.erase(itrWhere);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::DeleteParent(PlanTreeNodeEx* p_pParent)
{
    List::iterator itrWhere = find(_parents.begin(), _parents.end(), p_pParent);
    assert(itrWhere != _parents.end());

    _parents.erase(itrWhere);

    if (_readyParents.find(p_pParent) != _readyParents.end())
        _readyParents.erase(p_pParent);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::CrossLinkChild(PlanTreeNodeEx* p_pChild)
{
    AddChild(p_pChild);
    p_pChild->AddParent(this);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::CrossUnlinkChild(PlanTreeNodeEx* p_pChild)
{
    DeleteChild(p_pChild);
    p_pChild->DeleteParent(this);
}
//////////////////////////////////////////////////////////////////////////
void PlanTreeNodeEx::SetChildrenAsBelongingSubPlanChildren()
{
    _belongingSubPlanChildren.clear();
    _belongingSubPlanChildren.insert(_belongingSubPlanChildren.begin(), _children.begin(), _children.end());
}
