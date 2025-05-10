#include "types.h"

using namespace std;

shared_ptr<Type> IntType = make_shared<PrimitiveType>(TypeKind::INT);
shared_ptr<Type> FloatType = make_shared<PrimitiveType>(TypeKind::FLOAT);
shared_ptr<Type> StringType = make_shared<PrimitiveType>(TypeKind::STRING);
shared_ptr<Type> BoolType = make_shared<PrimitiveType>(TypeKind::BOOL);
shared_ptr<Type> VoidType = make_shared<PrimitiveType>(TypeKind::VOID);
shared_ptr<Type> ErrorType = make_shared<PrimitiveType>(TypeKind::ERROR);