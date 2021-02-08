#ifndef HEMERA_OPERATIONWITHPROGRESS_P_H
#define HEMERA_OPERATIONWITHPROGRESS_P_H

#include <HemeraCore/OperationWithProgress>
#include <HemeraCore/hemeraoperation_p.h>

namespace Hemera {

class OperationWithProgressPrivate : public OperationPrivate
{
public:
    OperationWithProgressPrivate(OperationWithProgress *q, PrivateExecutionOptions pO = PrivateExecutionOption::NoOptions) : OperationPrivate(q, pO), progress(0) {}

    Q_DECLARE_PUBLIC(OperationWithProgress)

    int progress;
    QString statusMessage;
};

}

#endif
