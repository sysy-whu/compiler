#include "ArmDag.h"
#include "../util/MyConstants.h"

inline ArmDAGValue::ArmDAGValue(ArmDAGNode *node, unsigned resno)
        : Node(node), ResNo(resno) {}

inline void ArmDAGUse::set(const ArmDAGValue &V) {
  if (Val.getNode())
    removeFromList();
  Val = V;
  if (V.getNode())
    V.getNode()->addUse(*this);
}


inline void ArmDAGUse::setInitial(const ArmDAGValue &V) {
  Val = V;
  V.getNode()->addUse(*this);
}

inline void ArmDAGUse::setNode(ArmDAGNode *N) {
  if (Val.getNode())
    removeFromList();
  Val.setNode(N);
  if (N)
    N->addUse(*this);
}