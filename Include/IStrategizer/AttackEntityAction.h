///> [Serializable]
#ifndef ATTACKENTITYACTION_H
#define ATTACKENTITYACTION_H

#include "Action.h"
#include "CellFeature.h"

#ifndef CHECKPOSITIONFILTERCOUNT_H
	#include "CheckPositionFilterCount.h"
#endif
#ifndef AND_H
	#include "And.h"
#endif

///> class=AttackEntityAction
///> parent=Action
class AttackEntityAction : public Action
{

public:
	OBJECT_SERIALIZABLE(AttackEntityAction);

                AttackEntityAction();
	            AttackEntityAction(const PlanStepParameters& p_parameters, CellFeature *p_cellFeature);
    void        Copy(IClonable* p_dest);
	void		Update(unsigned long p_cycles);

protected:
	bool		ExecuteAux(unsigned long p_cycles);
	void		InitializePreConditions();
	void		InitializeAliveConditions();
	void		InitializeSuccessConditions();
	void		InitializePostConditions();
};

#endif	// ATTACKENTITYACTION_H